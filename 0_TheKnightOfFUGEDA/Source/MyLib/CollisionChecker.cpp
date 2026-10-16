#include "CollisionChecker.h"
#include "Component/Transform.h"
#include "Component/Rigidbody.h"
#include "Component/Collision/Collidable.h"
#include "Collider/SphereCollider.h"
#include "Collider/CapsuleCollider.h"
#include "Collider/GroundMeshCollider.h"
#include "Collider/WallMeshCollider.h"
#include "Renderer.h"
#include "Physics.h"
#include <set>

namespace
{
	// 三角形の頂点の数
	constexpr int kTriangleVertexNum = 3;

	// 距離の閾値(これよりも距離の大きさが小さい場合押し戻しの距離判定をしない)
	constexpr float kDistanceThreshold = 0.0001f;

	// 法線方向の向きの閾値(法線の内積がこれよりも大きい場合は同じ向きのものとする)
	constexpr float kNormThreshold = 0.7f;

	// 当たった場所の閾値(当たった場所の大きさの二乗がこれより近い時は同じ判定とする)
	constexpr float kClosestPosThreshold = 0.01f;
}

MyLib::CollisionChecker::CollisionChecker()
{
	m_currentColData.clear();
}

void MyLib::CollisionChecker::CheckCollisions(const std::vector<std::pair<std::shared_ptr<MyLib::Collidable>, std::shared_ptr<MyLib::Collidable>>>& pairs)
{
	// キーを保持するためのコンテナ
	// このフレーム内で当たった当たり判定を保持しておく
	std::set<CollisionPairKey> currentCollisionPairs;

	for (auto& [a, b] : pairs)
	{
		// もしaとbが同一であれば処理をしない
		if (a == b) continue;

		std::shared_ptr<Rigidbody> pRigidbodyA = a->GetRigidbody().lock();
		std::shared_ptr<Rigidbody> pRigidbodyB = b->GetRigidbody().lock();
		// Rigidbodyが取得できなければ処理をしない
		if (!pRigidbodyA || !pRigidbodyB) continue;

		for (auto& colliderA : a->GetColliders())
		{
			for (auto& colliderB : b->GetColliders())
			{
				// どちらかの当たり判定が非アクティブの場合処理をしない
				if (!colliderA->IsEnable() || !colliderB->IsEnable()) continue;

				ColResult result = IsColliding(colliderA, a, colliderB, b);
				// 当たっていれば
				if (result.isHit)
				{
					Vector3 normal = result.contacts.empty() ? Vector3::Zero() : result.contacts[0].normal;

					auto key = CreatePairKey(colliderA.get(), colliderB.get());
					// 当たり判定に追加
					currentCollisionPairs.insert(key);

					auto it = m_currentColData.find(key);
					// もし新しい当たり判定データだったら当たった瞬間の関数を呼ぶ
					if (it == m_currentColData.end())
					{
						m_currentColData.emplace(key, CollisionData{ a, b, colliderA, colliderB });

						CollisionInfo enterColInfo;
						enterColInfo.otherCollidable = b;
						enterColInfo.myCollider = colliderA;
						enterColInfo.otherCollider = colliderB;
						enterColInfo.hitNormal = normal;
						a->OnCollideEnter(enterColInfo);

						// Bの当たり判定の情報を設定する
						enterColInfo.otherCollidable = a;
						enterColInfo.myCollider = colliderB;
						enterColInfo.otherCollider = colliderA;
						enterColInfo.hitNormal = -normal;
						b->OnCollideEnter(enterColInfo);
					}

					// どちらもすり抜けないコライダーの場合押し戻し処理を登録する
					if (!colliderA->IsTrigger() && !colliderB->IsTrigger())
					{
						for (auto& contact : result.contacts)
						{
							PushBackInfo pushBackInfo;

							pushBackInfo.normal = contact.normal;
							pushBackInfo.depth = contact.depth;
							pushBackInfo.rigidbodyA = pRigidbodyA;
							pushBackInfo.rigidbodyB = pRigidbodyB;

							MyLib::Physics::GetInstance().RegisterPushBackInfo(pushBackInfo);
						}
					}

					// Aの当たり判定の情報を設定する
					CollisionInfo info;
					info.otherCollidable = b;
					info.myCollider = colliderA;
					info.otherCollider = colliderB;
					info.hitNormal = normal;
					a->OnCollide(info);

					// Bの当たり判定の情報を設定する
					info.otherCollidable = a;
					info.myCollider = colliderB;
					info.otherCollider = colliderA;
					info.hitNormal = -normal;
					b->OnCollide(info);
				}
			}
		}
	}

	// 現在の当たり判定データをすべて確認
	for (auto it = m_currentColData.begin(); it != m_currentColData.end();)
	{
		// 現在のフレームに当たり判定データがない場合当たらなくなった際の処理を行う
		if (currentCollisionPairs.find(it->first) == currentCollisionPairs.end())
		{
			// 保持していた当たり判定データをlock
			std::shared_ptr<MyLib::Collidable> pCollidableA = it->second.collidableA.lock();
			std::shared_ptr<MyLib::Collidable> pCollidableB = it->second.collidableB.lock();
			std::shared_ptr<MyLib::ColliderBase> pColliderA = it->second.colliderA.lock();
			std::shared_ptr<MyLib::ColliderBase> pColliderB = it->second.colliderB.lock();

			if (pCollidableA && pCollidableB && pColliderA && pColliderB)
			{
				// Aの当たり判定の情報を設定する
				CollisionInfo info;
				info.otherCollidable = pCollidableB;
				info.myCollider = pColliderA;
				info.otherCollider = pColliderB;
				pCollidableA->OnCollideExit(info);

				// Bの当たり判定の情報を設定する
				info.otherCollidable = pCollidableA;
				info.myCollider = pColliderB;
				info.otherCollider = pColliderA;
				pCollidableB->OnCollideExit(info);
			}
			it = m_currentColData.erase(it);
		}
		else
		{
			++it;
		}
	}
}

MyLib::CollisionChecker::ColResult MyLib::CollisionChecker::IsColliding(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB)
{
	ColResult result;
	auto shapeA = colA->GetShape();
	auto shapeB = colB->GetShape();

	// 順序を入れ替えたかどうかを判定する
	bool isSwapped = false;

	// 順序によって関数を変えなくていいようにA→Bの順序に並べる
	if (shapeA > shapeB)
	{
		std::swap(colA, colB);
		std::swap(collidableA, collidableB);
		std::swap(shapeA, shapeB);
		isSwapped = true;
	}

	// 球と球の当たり判定の場合
	if (shapeA == MyLib::ColliderBase::ColliderShape::Sphere &&
		shapeB == MyLib::ColliderBase::ColliderShape::Sphere)
	{
		result = IsColSphereSphere(colA, collidableA, colB, collidableB);
	}
	// 球とカプセルの当たり判定の場合
	if (shapeA == MyLib::ColliderBase::ColliderShape::Sphere &&
		shapeB == MyLib::ColliderBase::ColliderShape::Capsule)
	{
		result = IsColSphereCapsule(colA, collidableA, colB, collidableB);
	}
	// 球と床メッシュの当たり判定の場合
	if (shapeA == MyLib::ColliderBase::ColliderShape::Sphere &&
		shapeB == MyLib::ColliderBase::ColliderShape::GroundMesh)
	{
		result = IsColSphereGroundMesh(colA, collidableA, colB, collidableB);
	}
	// 球と壁メッシュの当たり判定の場合
	if (shapeA == MyLib::ColliderBase::ColliderShape::Sphere &&
		shapeB == MyLib::ColliderBase::ColliderShape::WallMesh)
	{
		result = IsColSphereWallMesh(colA, collidableA, colB, collidableB);
	}

	if (isSwapped && result.isHit)
	{
		for (auto& contact : result.contacts)
		{
			contact.normal = -contact.normal;
		}
	}

	return result;
}

MyLib::CollisionChecker::ColResult MyLib::CollisionChecker::IsColSphereSphere(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB)
{
	auto sphereA = std::dynamic_pointer_cast<MyLib::SphereCollider>(colA);
	auto sphereB = std::dynamic_pointer_cast<MyLib::SphereCollider>(colB);
	if (!sphereA || !sphereB) return ColResult{};

	ColResult result;

	MyLib::WorldInfo infoA = collidableA->GetWorldInfo(colA);
	MyLib::WorldInfo infoB = collidableB->GetWorldInfo(colB);

	Vector3 posA = infoA.pos;
	Vector3 posB = infoB.pos;
	float rad = sphereA->GetRadius() + sphereB->GetRadius();

	Vector3 dist = posA - posB;

	//result.normal = dist.Normalized();
	//result.depth = rad - dist.Length();

	result.contacts.push_back({ dist.Normalized(), rad - dist.Length() });

	// 当たり判定フラグを設定する
	result.isHit = dist.SqrLength() <= rad * rad;

	return result;
}

MyLib::CollisionChecker::ColResult MyLib::CollisionChecker::IsColCapsuleCapsule(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB)
{
	std::shared_ptr<MyLib::CapsuleCollider> capsuleA = std::dynamic_pointer_cast<MyLib::CapsuleCollider>(colA);
	std::shared_ptr<MyLib::CapsuleCollider> capsuleB = std::dynamic_pointer_cast<MyLib::CapsuleCollider>(colB);
	if (!capsuleA || !capsuleB) return ColResult{};

	Vector3 lineA = capsuleA->GetLine();
	Vector3 lineB = capsuleB->GetLine();



	return ColResult{};
}

MyLib::CollisionChecker::ColResult MyLib::CollisionChecker::IsColSphereCapsule(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB)
{
	std::shared_ptr<MyLib::SphereCollider> sphere = std::dynamic_pointer_cast<MyLib::SphereCollider>(colA);
	std::shared_ptr<MyLib::CapsuleCollider> capsule = std::dynamic_pointer_cast<MyLib::CapsuleCollider>(colB);
	if (!sphere || !capsule) return ColResult{};

	ColResult result;

	MyLib::WorldInfo infoA = collidableA->GetWorldInfo(colA);
	MyLib::WorldInfo infoB = collidableB->GetWorldInfo(colB);

	Vector3 spherePos = infoA.pos;
	Vector3 capsuleBasePos = infoB.pos;

	Quaternion capsuleRot = infoB.rotation;
	Vector3 capsuleTop = capsuleBasePos + (capsuleRot * capsule->GetTop());
	Vector3 capsuleBottom = capsuleBasePos + (capsuleRot * capsule->GetBottom());

	// カプセルの線分
	Vector3 capsuleAxis = capsuleTop - capsuleBottom;
	Vector3 capsuleBottomToSphere = spherePos - capsuleBottom;

	// カプセルと球の最近点を求める
	float t = Vector3::Dot(capsuleBottomToSphere, capsuleAxis) / capsuleAxis.SqrLength();

	// 最近点の割合を0~1の間に収める(0以下ならカプセルの線分の下端を最近点、1以上ならカプセルの線分の上端を最近点にする)
	t = std::max(0.0f, std::min(1.0f, t));

	// カプセルの線分と球の最近点を求める
	Vector3 nearPos = capsuleBottom + capsuleAxis * t;
	float rad = sphere->GetRadius() + capsule->GetRadius();

	// 球とカプセルの線分の最近点を求める(ここからは球と球の当たり判定と同じことができる)
	Vector3 dist = spherePos - nearPos;

	//result.normal = dist.Normalized();
	//result.depth = rad - dist.Length();

	result.contacts.push_back({ dist.Normalized(), rad - dist.Length() });

	// 当たり判定フラグを設定する
	result.isHit = dist.SqrLength() <= rad * rad;

	return result;
}

MyLib::CollisionChecker::ColResult MyLib::CollisionChecker::IsColSphereGroundMesh(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB)
{
	std::shared_ptr<MyLib::SphereCollider> sphere = std::dynamic_pointer_cast<MyLib::SphereCollider>(colA);
	std::shared_ptr<MyLib::GroundMeshCollider> groundMesh = std::dynamic_pointer_cast<MyLib::GroundMeshCollider>(colB);
	if (!sphere || !groundMesh) return ColResult{};

	ColResult result;
	float maxDepth = -FLT_MAX; // めり込み率

	MyLib::WorldInfo infoA = collidableA->GetWorldInfo(colA);
	MyLib::WorldInfo infoB = collidableB->GetWorldInfo(colB);

	Vector3 spherePos = infoA.pos;
	float rad = sphere->GetRadius();

	const auto& vertices = groundMesh->GetVertices();
	const auto& indices = groundMesh->GetIndices();

	for (size_t i = 0; i < indices.size(); i += kTriangleVertexNum)
	{
		Vector3 pos0 = vertices[indices[i]];
		Vector3 pos1 = vertices[indices[i + 1]];
		Vector3 pos2 = vertices[indices[i + 2]];

		// 三角形との最近点を取得
		Vector3 closestPos = Vector3::GetClosestPositionOnTriangle(spherePos, pos0, pos1, pos2);

		// 最近点と球の現在の位置の距離を計算する
		Vector3 distVec = spherePos - closestPos;
		float dist = distVec.Length();

		// 最近点との距離が球の半径よりも大きい場合当たっていないため次のポリゴンへ
		if (dist > rad) continue;

		Vector3 norm = {};

		// 距離の大きさが小さい場合法線方向に押し戻す
		if (dist > kDistanceThreshold)
		{
			norm = distVec.Normalized();
		}
		else
		{
			Vector3 edge1 = pos1 - pos0;
			Vector3 edge2 = pos2 - pos0;

			norm = Vector3::Cross(edge1, edge2);
			norm.Normalize();
		}

		float depth = rad - dist;

		// 他の壁とのめり込み度チェックフラグ
		bool isMerged = false;

		// 今までの当たっているリザルトをチェック
		for (auto& contact : result.contacts)
		{
			// 法線方向に近いかどうか
			bool isNearNorm = Vector3::Dot(contact.normal, norm) > kNormThreshold;
			// 当たった場所が近いかどうか
			bool isNearPos = (contact.point - closestPos).SqrLength() < kClosestPosThreshold;

			// 法線が同じもしくは当たった場所が近い場合よりめり込んでいるものを判定
			if (isNearNorm || isNearPos)
			{
				// 新しいリザルトの方がめり込んでいる場合そちらのものに更新
				if (depth > contact.depth)
				{
					contact.normal = norm;
					contact.depth = depth;
					contact.point = closestPos;
				}
				isMerged = true;		// リザルトがチェックされたとする
				break;
			}

			//// 法線が同じもしくは当たった場所が近い場合よりめり込んでいるものを判定
			//if (Vector3::Dot(contact.normal, norm) > 0.90f)
			//{
			//	// 新しいリザルトの方がめり込んでいる場合そちらのものに更新
			//	if (depth > contact.depth)
			//	{
			//		contact.normal = norm;
			//		contact.depth = depth;
			//	}
			//	isMerged = true;		// リザルトがチェックされたとする
			//	break;
			//}
		}

		// リザルトのチェックが行われていない場合
		if (!isMerged)
		{
			// 当たったリザルトを設定
			result.isHit = true;
			ContactsInfo info = { norm, depth };
			result.contacts.push_back(info);
		}

	}
	return result;
}

MyLib::CollisionChecker::ColResult MyLib::CollisionChecker::IsColSphereWallMesh(std::shared_ptr<ColliderBase> colA, std::shared_ptr<Collidable> collidableA, std::shared_ptr<ColliderBase> colB, std::shared_ptr<Collidable> collidableB)
{
	std::shared_ptr<MyLib::SphereCollider> sphere = std::dynamic_pointer_cast<MyLib::SphereCollider>(colA);
	std::shared_ptr<MyLib::WallMeshCollider> wallMesh = std::dynamic_pointer_cast<MyLib::WallMeshCollider>(colB);
	if (!sphere || !wallMesh) return ColResult{};

	ColResult result;
	float maxDepth = -FLT_MAX; // めり込み率

	MyLib::WorldInfo infoA = collidableA->GetWorldInfo(colA);
	MyLib::WorldInfo infoB = collidableB->GetWorldInfo(colB);

	Vector3 spherePos = infoA.pos;
	float rad = sphere->GetRadius();

	const auto& vertices = wallMesh->GetVertices();
	const auto& indices = wallMesh->GetIndices();

	for (size_t i = 0; i < indices.size(); i += kTriangleVertexNum)
	{
		Vector3 pos0 = vertices[indices[i]];
		Vector3 pos1 = vertices[indices[i + 1]];
		Vector3 pos2 = vertices[indices[i + 2]];

		// 三角形との最近点を取得
		Vector3 closestPos = Vector3::GetClosestPositionOnTriangle(spherePos, pos0, pos1, pos2);

		// 最近点と球の現在の位置の距離を計算する
		Vector3 distVec = spherePos - closestPos;
		float dist = distVec.Length();

		// 最近点との距離が球の半径よりも大きい場合当たっていないため次のポリゴンへ
		if (dist > rad) continue;

		Vector3 norm = {};

		// 距離の大きさが小さい場合法線方向に押し戻す
		if (dist > kDistanceThreshold)
		{
			norm = distVec.Normalized();
		}
		else
		{
			Vector3 edge1 = pos1 - pos0;
			Vector3 edge2 = pos2 - pos0;

			norm = Vector3::Cross(edge1, edge2);
			norm.Normalize();
		}

		float depth = rad - dist;

		// 他の壁とのめり込み度チェックフラグ
		bool isMerged = false;

		// 今までの当たっているリザルトをチェック
		for (auto& contact : result.contacts)
		{
			// 法線方向に近いかどうか
			bool isNearNorm = Vector3::Dot(contact.normal, norm) > kNormThreshold;
			// 当たった場所が近いかどうか
			bool isNearPos = (contact.point - closestPos).SqrLength() < kClosestPosThreshold;

			// 法線が同じもしくは当たった場所が近い場合よりめり込んでいるものを判定
			if (isNearNorm || isNearPos)
			{
				// 新しいリザルトの方がめり込んでいる場合そちらのものに更新
				if (depth > contact.depth)
				{
					contact.normal = norm;
					contact.depth = depth;
					contact.point = closestPos;
				}
				isMerged = true;		// リザルトがチェックされたとする
				break;
			}

			//// 法線が同じもしくは当たった場所が近い場合よりめり込んでいるものを判定
			//if (Vector3::Dot(contact.normal, norm) > 0.90f)
			//{
			//	// 新しいリザルトの方がめり込んでいる場合そちらのものに更新
			//	if (depth > contact.depth)
			//	{
			//		contact.normal = norm;
			//		contact.depth = depth;
			//	}
			//	isMerged = true;		// リザルトがチェックされたとする
			//	break;
			//}
		}

		// リザルトのチェックが行われていない場合
		if (!isMerged)
		{
			// 当たったリザルトを設定
			result.isHit = true;
			ContactsInfo info = { norm, depth };
			result.contacts.push_back(info);
		}

	}
	return result;
}

void MyLib::CollisionChecker::ApplyDestroyed(std::shared_ptr<Collidable> collidable)
{
	// 現在の当たり判定データをすべて確認
	for (auto it = m_currentColData.begin(); it != m_currentColData.end();)
	{
		// 対象のCollidableコンポーネントを探す
		std::shared_ptr<MyLib::Collidable> pCollidableA = it->second.collidableA.lock();
		std::shared_ptr<MyLib::Collidable> pCollidableB = it->second.collidableB.lock();

		// 当たり判定が対象の当たり判定にない場合次のペアを探す
		if (collidable != pCollidableA && collidable != pCollidableB)
		{
			++it;
			continue;
		}

		// 保持していた当たり判定データをlock
		std::shared_ptr<MyLib::ColliderBase> pColliderA = it->second.colliderA.lock();
		std::shared_ptr<MyLib::ColliderBase> pColliderB = it->second.colliderB.lock();

		// すべて取得できる場合はExit処理を呼ぶ
		if (pCollidableA && pCollidableB && pColliderA && pColliderB)
		{
			// Aの当たり判定の情報を設定する
			CollisionInfo info;
			info.otherCollidable = pCollidableB;
			info.myCollider = pColliderA;
			info.otherCollider = pColliderB;
			pCollidableA->OnCollideExit(info);

			// Bの当たり判定の情報を設定する
			info.otherCollidable = pCollidableA;
			info.myCollider = pColliderB;
			info.otherCollider = pColliderA;
			pCollidableB->OnCollideExit(info);
		}
		it = m_currentColData.erase(it);
	}
}

MyLib::CollisionChecker::CollisionPairKey MyLib::CollisionChecker::CreatePairKey(MyLib::ColliderBase* colliderA, MyLib::ColliderBase* colliderB)
{
	if (colliderA > colliderB)
	{
		std::swap(colliderA, colliderB);
	}

	return CollisionPairKey{ colliderA, colliderB };
}
