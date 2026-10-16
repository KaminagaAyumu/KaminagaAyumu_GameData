#include "EnemyManager.h"
#include "EnemyBase.h"
#include "../../MyLib/ObjectFactory.h"
#include "../../MyLib/MyMath.h"
#include "../../MyLib/Component/Transform.h"
#include "../../Geometry/Vector3.h"
#include "../../Utility/CSV/EnemyData.h"

namespace
{
	// 視野角のマージン
	constexpr float kFovMargin = 15.0f;

	// ロックオン対象にする敵とプレイヤーの距離の最大値
	constexpr float kMaxLockOnDist = 15.0f;

	// カメラからの距離がこれより近い敵は判定しない(0除算を防ぐ)
	constexpr float kMinDistFromCamera = 1e-4f;

	// ファイルを読み込む際のパスの最大サイズ(文字数)
	constexpr size_t kFilePathMax = 256;
}

EnemyManager::EnemyManager() : 
	m_deadCountThisFrame(0)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init(int stageNo)
{
	wchar_t filePath[kFilePathMax];
	std::swprintf(filePath, kFilePathMax, L"Data/File/CSV/Enemy/enemy_resource_%d.csv", stageNo);

	m_enemyResource.Load(filePath);
	m_enemyResource.ConvertEnemyData();
}

void EnemyManager::Update()
{
	size_t before = m_pEnemies.size();

	// 敵の中で削除されているものを探す
	m_pEnemies.remove_if([](std::weak_ptr<EnemyBase> object)
		{
			// 削除されている際にリストから消去する
			return object.expired();
		});
	m_deadCountThisFrame = static_cast<int>(before - m_pEnemies.size());
}

void EnemyManager::End()
{
	
}

void EnemyManager::CreateEnemy(std::shared_ptr<MyLib::GameObject> player)
{
	for (auto spawn : m_enemyResource.GetEnemyDatas())
	{
		std::shared_ptr<EnemyBase> enemy = std::dynamic_pointer_cast<EnemyBase>(MyLib::ObjectFactory::CreateEnemy(spawn->GetType()));
		enemy->SetPlayer(player);
		enemy->Init();
		enemy->SetPos(spawn->GetPos());
		m_pEnemies.push_back(enemy);
	}

}

bool EnemyManager::IsEnemyDeadAll() const
{
	return m_pEnemies.empty();
}

void EnemyManager::SetCanAct(bool canAct)
{
	for (auto& weakEnemy : m_pEnemies)
	{
		if (auto pEnemy = weakEnemy.lock())
		{
			pEnemy->SetCanAct(canAct);
		}
	}
}

std::weak_ptr<MyLib::Transform> EnemyManager::GetNearEnemyTransform(const Vector3& playerPos, const Vector3& cameraPos, float fovDegree)
{
	// 対象にする敵確保用
	std::weak_ptr<MyLib::Transform> nearestEnemy;
	// 現在の最近傍距離
	float nearestDist = FLT_MAX;

	// Fov(視野角)の半分
	float cosHalfFov = cosf(MyLib::ToRadian((fovDegree + kFovMargin) * 0.5f));

	for (auto& weakEnemy : m_pEnemies)
	{
		auto pEnemy = weakEnemy.lock();
		if (!pEnemy) continue;

		auto pTransform = pEnemy->GetComponent<MyLib::Transform>().lock();
		if (!pTransform) continue;

		// カメラから敵に向かうベクトル
		Vector3 toEnemy = pTransform->GetPos() - cameraPos;
		// カメラからの距離を取得
		float distFromCamera = toEnemy.Length();
		// カメラからの距離が一定より近い場合処理をしない
		if (distFromCamera < kMinDistFromCamera) continue;

		// カメラから敵に向かう方向ベクトル
		Vector3 dirToEnemy = toEnemy * (1.0f / distFromCamera);

		// カメラの正面からの内積をとる(これで角度を求めている)
		float dot = Vector3::Dot(GetCameraFrontVector(), dirToEnemy);

		// 内積が視野角以下ならば処理をしない
		if (dot < cosHalfFov) continue;

		// プレイヤーからの距離を測る
		float dist = Vector3::GetDistance(playerPos, pTransform->GetPos());

		// プレイヤーからの距離が最大値よりも遠い場合処理をしない
		if (dist > kMaxLockOnDist) continue;

		// プレイヤーからの距離が現在の最近傍よりも近い場合更新する
		if (dist < nearestDist)
		{
			nearestDist = dist;
			nearestEnemy = pTransform;
		}
	}
	return nearestEnemy;
}

std::weak_ptr<MyLib::Transform> EnemyManager::ReGetNearEnemyTransform(const Vector3& playerPos, const Vector3& cameraPos, float fovDegree, const Vector3& currentEnemyPos, bool isLeft)
{
	// 対象にする敵確保用
	std::weak_ptr<MyLib::Transform> nearestEnemy;
	// 現在の最近傍距離
	float nearestDist = FLT_MAX;

	// Fov(視野角)の半分
	float cosHalfFov = cosf(MyLib::ToRadian((fovDegree + kFovMargin) * 0.5f));

	for (auto& weakEnemy : m_pEnemies)
	{
		auto pEnemy = weakEnemy.lock();
		if (!pEnemy) continue;

		auto pTransform = pEnemy->GetComponent<MyLib::Transform>().lock();
		if (!pTransform) continue;

		// 現在の最近傍の敵は検知しない
		if (currentEnemyPos == pTransform->GetPos())
		{
			continue;
		}

		// カメラの右方向を取得
		Vector3 cameraRight = Vector3::Cross(Vector3::Up(), GetCameraFrontVector());
		cameraRight.Normalize();

		Vector3 toCandidate = pTransform->GetPos() - currentEnemyPos;

		float side = Vector3::Dot(cameraRight, toCandidate);

		if (isLeft && side >= 0.0f) continue;
		if (!isLeft && side <= 0.0f) continue;

		// カメラから敵に向かうベクトル
		Vector3 toEnemy = pTransform->GetPos() - cameraPos;
		// カメラからの距離を取得
		float distFromCamera = toEnemy.Length();
		// カメラからの距離が一定より近い場合処理をしない
		if (distFromCamera < kMinDistFromCamera) continue;

		// カメラから敵に向かう方向ベクトル
		Vector3 dirToEnemy = toEnemy * (1.0f / distFromCamera);

		// カメラの正面からの内積をとる(これで角度を求めている)
		float dot = Vector3::Dot(GetCameraFrontVector(), dirToEnemy);

		// 内積が視野角以下ならば処理をしない
		if (dot < cosHalfFov) continue;

		// プレイヤーからの距離を測る
		float dist = Vector3::GetDistance(playerPos, pTransform->GetPos());

		// プレイヤーからの距離が最大値よりも遠い場合処理をしない
		if (dist > kMaxLockOnDist) continue;

		// プレイヤーからの距離が現在の最近傍よりも近い場合更新する
		if (dist < nearestDist)
		{
			nearestDist = dist;
			nearestEnemy = pTransform;
		}
	}
	return nearestEnemy;
}
