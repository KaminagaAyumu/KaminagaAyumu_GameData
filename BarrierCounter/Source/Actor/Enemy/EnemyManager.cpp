#include "EnemyManager.h"
#include "EnemyBase.h"
#include "Enemy.h"
#include "MoveEnemy.h"
#include "../../Common/Camera/Camera.h"
#include "DxLib.h"
#include <cassert>

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
	int handle = MV1LoadModel(L"Data/Model/enemy.mv1");
	assert(handle != -1 && "敵モデルのロードに失敗しました");
	m_modelHandles.push_back(handle);
	handle = MV1LoadModel(L"Data/Model/moveEnemy.mv1");
	assert(handle != -1 && "敵モデルのロードに失敗しました");
	m_modelHandles.push_back(handle);

	// 敵リストを初期化
	m_pEnemies.clear();

	// 敵の生成座標をセット
	m_enemySpawnPositions =
	{
		// ステージ1
		{
			{ EnemyType::Normal, { -2000.0f,    80.0f,     0.0f }},
			{ EnemyType::Normal, {     0.0f,    80.0f,  2000.0f }},
			{ EnemyType::Normal, {  2000.0f,    80.0f,     0.0f }}
		},
		// ステージ2
		{
			{ EnemyType::Normal, { -2000.0f,    80.0f,     0.0f }},
			{ EnemyType::Move  , {     0.0f,    80.0f,  2000.0f }},
			{ EnemyType::Normal, {  2000.0f,    80.0f,     0.0f }},
			{ EnemyType::Normal, { -2000.0f,    80.0f, -2000.0f }},
			{ EnemyType::Normal, {  2000.0f,    80.0f, -2000.0f }},
			{ EnemyType::Normal, {     0.0f,    80.0f, -2000.0f }}
		},
		// ステージ3
		{
			{ EnemyType::Normal, { -2000.0f,    80.0f,     0.0f }},
			{ EnemyType::Move  , {     0.0f,    80.0f,  2000.0f }},
			{ EnemyType::Normal, {  2000.0f,    80.0f,     0.0f }},
			{ EnemyType::Normal, { -2000.0f,    80.0f, -2000.0f }},
			{ EnemyType::Normal, {  2000.0f,    80.0f, -2000.0f }},
			{ EnemyType::Normal, {     0.0f,    80.0f, -2000.0f }},
			{ EnemyType::Move  , { -1000.0f,    80.0f, -1000.0f }},
			{ EnemyType::Move  , {  1000.0f,    80.0f,  1000.0f }},
			{ EnemyType::Move  , { -1000.0f,    80.0f, -2000.0f }},
		},
	};

}

void EnemyManager::End()
{
	// モデルハンドルのメモリ開放処理を行う
	for (auto& handle : m_modelHandles)
	{
		MV1DeleteModel(handle);
	}

	// 残っている敵がいる場合はすべての敵の終了処理を行う
	for (auto& enemy : m_pEnemies)
	{
		enemy->End();
	}
}

const int EnemyManager::Update(std::shared_ptr<Camera> camera)
{
	// 敵を倒した際のスコアを取得
	int killScore = 0;

	// 敵の更新
	for (auto& enemy : m_pEnemies)
	{
		// カメラに入っているかどうかの情報をセット
		enemy->SetIsInCamera(camera->IsInCamera(enemy->GetPos()));

		// 更新処理を行う
		enemy->Update();

		// 敵が死んだとき
		if (enemy->IsDead())
		{
			// 敵を倒した際のスコアを加算
			killScore += enemy->GetKillScore();

			// 敵の終了処理を行う
			enemy->End();
		}
	}

	// 敵の中で死んだものを探す
	m_pEnemies.remove_if([](std::shared_ptr<EnemyBase> enemy)
		{
			// 敵が消えた判定の時に削除
			return enemy->IsDead();
		});

	// このフレーム内でのスコアを返す
	return killScore;
}

void EnemyManager::Draw() const
{
	// 敵の描画
	for (const auto& bullet : m_pEnemies)
	{
		bullet->Draw();
	}
}

bool EnemyManager::IsKilledAll() const
{
	return m_pEnemies.empty();
}

void EnemyManager::SpawnEnemyToStageNo(StageType stage, std::shared_ptr<Player> player, std::shared_ptr<BulletManager> bulletManager)
{
	// ステージ番号が設定された座標データの数よりも大きかったら
	if (m_enemySpawnPositions.size() < static_cast<int>(stage))
	{
		printfDx(L"座標情報がありません\n");
		return;
	}

	// ステージ番号に対応した敵の情報を取得
	for (auto info : m_enemySpawnPositions[static_cast<int>(stage)])
	{
		// 敵を生成
		CreateEnemy(info.pos, info.type, player, bulletManager);
	}
}

void EnemyManager::CreateEnemy(const Vector3& pos, EnemyManager::EnemyType type, std::shared_ptr<Player> player, std::shared_ptr<BulletManager> bulletManager)
{
	// 生成する敵を確認
	switch (type)
	{
	case EnemyType::Normal: // 通常の敵
	{
		// 敵の生成
		auto enemy = std::make_shared<Enemy>();
		enemy->Init(); // 初期化
		enemy->SetModel(MV1DuplicateModel(m_modelHandles[static_cast<int>(type)])); // モデルをセット
		enemy->SetSpawnPos(pos); // 座標をセット
		enemy->SetPlayer(player); // プレイヤーをセット
		enemy->SetBulletManager(bulletManager); // 弾マネージャーをセット
		m_pEnemies.push_back(enemy); // 敵リストに追加
	}
		break;
	case EnemyType::Move: // 動く敵
	{
		// 敵の生成
		auto enemy = std::make_shared<MoveEnemy>();
		enemy->Init(); // 初期化
		enemy->SetModel(MV1DuplicateModel(m_modelHandles[static_cast<int>(type)])); // モデルをセット
		enemy->SetSpawnPos(pos); // 座標をセット
		enemy->SetPlayer(player); // プレイヤーをセット
		enemy->SetBulletManager(bulletManager); // 弾マネージャーをセット
		m_pEnemies.push_back(enemy); // 敵リストに追加
	}
	break;
	case EnemyType::None:
		break;
	default:
		break;
	}

}
