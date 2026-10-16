#include "Physics.h"
#include <cassert>
#include "../Common/Game.h"
#include "MyLib.h"


using namespace MyLib;

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

void Physics::Entry(std::shared_ptr<Collidable> collidable)
{
	// 既に登録されているかどうかを確認
	bool isFound = (std::find(m_pCollidables.begin(), m_pCollidables.end(), collidable) != m_pCollidables.end());

	if (isFound)
	{
		// すでに登録されていた場合アサート
		assert(false && "当たり判定がすでに登録済みです");
	}
	else
	{
		// 登録する
		m_pCollidables.emplace_back(collidable);
	}
}

void Physics::Exit(std::shared_ptr<Collidable> collidable)
{
	// 登録されているCollidableを確認
	auto count = std::erase_if(m_pCollidables, [collidable](std::shared_ptr<Collidable> target)
		{
			// 対象のCollidableと一致したものを消去
			return target == collidable;
		});
}

void Physics::Update()
{
	// 物理更新を止める状態ならば以下の処理は行わない
	if (m_isStop) { return; }

	// すべての物理オブジェクトの未来の位置を設定
	for (auto& item : m_pCollidables)
	{
		// 現在の座標を取得
		auto pos = item->m_rigidbody.GetPos();
		// 現在の速度を取得
		auto velocity = item->m_rigidbody.GetVelocity();
		// タイムスケール基準に速度を調整
		auto scaledVelocity = velocity * m_timeScale;
		// 次のフレームでの座標を取得
		auto nextPos = pos + scaledVelocity;

		// 未来の座標を取得
		item->m_nextPos = nextPos;

		// --------線との押し戻し処理に直す予定--------
		// 未来の座標がマップの外に出た場合
		if (nextPos.x >= Game::kMapMaxX ||
		nextPos.x <= Game::kMapMinX ||
		nextPos.z >= Game::kMapMaxZ ||
		nextPos.z <= Game::kMapMinZ)
		{
			// 弾以外はマップの外に出られないようにする
			if (item->GetTag() != ObjectTag::EnemyBullet)
			{
				// 実際の座標には前の位置を代入
				item->m_rigidbody.SetPos(pos);
			}
			else
			{
				// 実際の座標に代入
				item->m_rigidbody.SetPos(nextPos);
			}
		}
		else
		{
			// 実際の座標に代入
			item->m_rigidbody.SetPos(nextPos);
		}
		// --------線との押し戻し処理に直す予定--------
		
		
#ifdef _DEBUG
		// 座標変化をデバッグで表示
		std::shared_ptr<SphereCollider> sphereData;
		sphereData = static_pointer_cast<SphereCollider>(item->m_pColliderData);
		float radius = sphereData->m_radius;
		DebugDraw::GetInstance().DrawSphere(pos, radius, 0x00ff00);
		DebugDraw::GetInstance().DrawLine(pos, nextPos, 0xff0000);
		DebugDraw::GetInstance().DrawSphere(nextPos, radius, 0xff2200);
#endif
	}

	for (auto& objA : m_pCollidables)
	{
		for (auto& objB : m_pCollidables)
		{
			if (objA != objB)
			{
				std::shared_ptr<SphereCollider> sphereA;
				sphereA = static_pointer_cast<SphereCollider>(objA->m_pColliderData);
				float radiusA = sphereA->m_radius;

				std::shared_ptr<SphereCollider> sphereB;
				sphereB = static_pointer_cast<SphereCollider>(objB->m_pColliderData);
				float radiusB = sphereB->m_radius;

				float rad = radiusA + radiusB;

				Vector3 dist = objA->m_nextPos - objB->m_nextPos;

				if (dist.SqrLength() <= rad * rad)
				{
					objA->OnCollide(objB);
					objB->OnCollide(objA);
				}
			}
		}
	}
}

Physics::Physics()
{
	// 初期状態は更新を止めない
	m_isStop = false;

	// タイムスケールは1.0f
	m_timeScale = 1.0f;
}