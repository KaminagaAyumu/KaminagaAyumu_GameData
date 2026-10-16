#include "Physics.h"
#include "Component/Transform.h"
#include "Component/Rigidbody.h"
#include "CollisionChecker.h"
#include "../Main/Application.h"
#include <cassert>

namespace
{
	constexpr float kGravity = 1.0f;
}

MyLib::Physics::Physics() : 
	m_isStop(false)
{
	m_pCollisionChecker = std::make_unique<CollisionChecker>();
}

MyLib::Physics& MyLib::Physics::GetInstance()
{
    static Physics instance;
    return instance;
}

void MyLib::Physics::Init(uint32_t level, const BoundingBox& worldAABB)
{
	m_octreeManager.Init(level, worldAABB);
}

void MyLib::Physics::Entry(std::shared_ptr<MyLib::Collidable> collidable)
{
	// 既に登録されているかを確認
	bool isFound = (std::find(m_pCollidables.begin(), m_pCollidables.end(), collidable) != m_pCollidables.end());

	if (isFound)
	{
		// 既に登録されていた場合アサート
		assert(false && "既に登録されている当たり判定が登録されました");
	}
	else
	{
		// 登録する
		m_pCollidables.emplace_back(collidable);
	}
}

void MyLib::Physics::Exit(std::shared_ptr<MyLib::Collidable> collidable)
{
	// 消去された際にExitの当たり判定を呼ぶ
	m_pCollisionChecker->ApplyDestroyed(collidable);

	// 登録されている当たり判定を確認
	auto count = std::erase_if(m_pCollidables, [collidable](std::shared_ptr<MyLib::Collidable> target)
		{
			// 対象の当たり判定と一致したものを消去
			return target == collidable;
		});
}

void MyLib::Physics::RegisterPushBackInfo(MyLib::PushBackInfo info)
{
	// 既に登録されているかを確認
	//bool isFound = (std::find(m_pPushBackInfos.begin(), m_pPushBackInfos.end(), info) != m_pPushBackInfos.end());

	//if (isFound)
	//{
	//	// 既に登録されていた場合アサート
	//	assert(false && "既に登録されている押し戻し情報が登録されました");
	//}
	//else
	//{
	//	// 登録する
	//	m_pPushBackInfos.emplace_back(info);
	//}
	// 登録する
	m_pPushBackInfos.emplace_back(info);
}

void MyLib::Physics::Update()
{
	if (m_isStop) return;

	// Rigidbodyを更新
	UpdateRigidbody();

	// 八分木に再登録
	ReregisterAll();

	std::vector<std::pair<std::shared_ptr<MyLib::Collidable>,
		std::shared_ptr<Collidable>>> pairs;

	// ヒット候補ペアを列挙
	m_octreeManager.GetCollisionPairs(pairs);

	// 実際の形状で当たり判定を行う
	m_pCollisionChecker->CheckCollisions(pairs);

	// 押し戻し情報を設定する
	ApplyPushBacks();

	// 座標の更新を行う
	UpdatePositions();
}

void MyLib::Physics::UpdateRigidbody()
{
	for (auto& col : m_pCollidables)
	{
		// オブジェクトが持つTransformとRigidbodyを取得する
		std::shared_ptr<MyLib::Transform> pTransform = col->GetTransform().lock();
		std::shared_ptr<MyLib::Rigidbody> pRigidbody = col->GetRigidbody().lock();
		// 取得できなかった場合処理をしない
		if (!pTransform || !pRigidbody) continue;

		Vector3 currentPos = pTransform->GetPos();

		Vector3 velocity = pRigidbody->GetVelocity();

		// 未来の座標を設定(タイムスケールに応じて動く量も変わる)
		Vector3 nextPos = currentPos + velocity * Application::GetInstance().GetTimeScale();

		/*if (pRigidbody->IsGravity())
		{
			nextPos += Vector3::Down() * kGravity * m_timeScale;
		}*/

		pRigidbody->SetNextPos(nextPos);
	}
}

void MyLib::Physics::ReregisterAll()
{
	// 全オブジェクトをセルから取り外す
	m_octreeManager.RemoveAll();

	// 現在位置のAABBで再登録
	for (auto& col : m_pCollidables)
	{
		// 当たり判定が有効でない場合登録しない
		if (!col->IsEnable()) continue;

		m_octreeManager.Register(col->GetOFT(), col->GetBoundingBox());
	}
}

void MyLib::Physics::ApplyPushBacks()
{
	for (auto& info : m_pPushBackInfos)
	{
		float massA = info.rigidbodyA->GetMass();
		float massB = info.rigidbodyB->GetMass();

		// 総質量
		float totalMass = massA + massB;

		float weightA = massB / totalMass;
		float weightB = massA / totalMass;

		Vector3 nextPosA = info.rigidbodyA->GetNextPos();
		Vector3 nextPosB = info.rigidbodyB->GetNextPos();

		if (info.rigidbodyA->GetBodyType() == BodyType::Static)
		{
			if (info.rigidbodyB->GetBodyType() != BodyType::Static)
			{
				info.rigidbodyB->SetNextPos(nextPosB - info.normal * info.depth);
			
			
				// 壁の処理テスト
				/*Vector3 wallNormal = -info.normal;
				Vector3 vel = info.rigidbodyB->GetVelocity();

				float dot = Vector3::Dot(vel, wallNormal);
				if (dot < 0.0f)
				{
					vel = vel - wallNormal * dot;
					info.rigidbodyB->SetVelocity(vel);
				}*/
			}
		}
		else if (info.rigidbodyB->GetBodyType() == BodyType::Static)
		{
			if (info.rigidbodyA->GetBodyType() != BodyType::Static)
			{
				info.rigidbodyA->SetNextPos(nextPosA + info.normal * info.depth);
			
				// 壁の処理テスト
				/*Vector3 wallNormal = -info.normal;
				Vector3 vel = info.rigidbodyA->GetVelocity();

				float dot = Vector3::Dot(vel, wallNormal);
				if (dot < 0.0f)
				{
					vel = vel - wallNormal * dot;
					info.rigidbodyA->SetVelocity(vel);
				}*/
			}
		}
		else
		{
			info.rigidbodyA->SetNextPos(nextPosA + info.normal * (weightA * info.depth));
			info.rigidbodyB->SetNextPos(nextPosB - info.normal * (weightB * info.depth));
		}
	}

	m_pPushBackInfos.clear();
}

void MyLib::Physics::UpdatePositions()
{
	for (auto& col : m_pCollidables)
	{
		// オブジェクトが持つTransformとRigidbodyを取得する
		std::shared_ptr<MyLib::Transform> pTransform = col->GetTransform().lock();
		std::shared_ptr<MyLib::Rigidbody> pRigidbody = col->GetRigidbody().lock();
		// 取得できなかった場合処理をしない
		if (!pTransform || !pRigidbody) continue;

		Vector3 finalPos = pRigidbody->GetNextPos();
		pTransform->SetPos(finalPos);

		// デバッグ用の当たり判定の表示
#ifdef _DEBUG
		for (auto& collider : col->GetColliders())
		{
			// 非アクティブの場合処理をしない
			if (!collider->IsEnable()) continue;
			WorldInfo info = col->GetWorldInfo(collider);
			collider->DrawDebug(info.pos, info.rotation);
		}
#endif
		// 向きの変更
		// 速度を取得する
		Vector3 vel = pRigidbody->GetVelocity();

		// 速度がある場合その向きに設定する
		if (vel.Length() > 0.0f && pRigidbody->IsApplyDirection())
		{
			Vector3 dir = vel.Normalized();

			// ワールド正面方向からの回転を設定する
			Quaternion rotation = Quaternion::SetRotation(Vector3::Back(), dir);

			pTransform->SetRotation(rotation);
		}
	}
}