#include <vector>
#include <list>
#include <memory>
#include "../Utility/Animation.h"
#include "../Utility/Geometry.h"
#include "EnemyManager.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/Map.h"
#include "Actor.h"
#include "EnemyBase.h"
#include "TransformEnemy.h"
#include "Player.h"
#include "../Utility/AnimationLoader.h"
#include "DxLib.h"

namespace
{
	constexpr int kSpawnFireBallNum = 1;
	constexpr int kSpawnSkullNum = 2;

	constexpr int kEnemyFormNum = 2;	// 敵のフォームの総数

	constexpr int kSpawnChipNo = 2; // マップの敵スポーンチップ番号

	constexpr int kSpawnTime = 420; // 敵生成までの時間
	constexpr int kSpawnTimeFast = 180; // 敵生成までの時間

	// 敵のアイテム化時間
	constexpr int kChangeToItemTime = 360;
	
#ifdef _DEBUG
	constexpr int kSpawnPosRadius = 10; // スポーン位置の半径

	constexpr unsigned int kNearSpawnPosColor = 0x00ff00; // プレイヤーから一番近い敵のスポーン位置の色
	constexpr unsigned int kNormalSpawnPosColor = 0xff0000; // 敵のスポーン位置の色

#endif


}

EnemyManager::EnemyManager(std::weak_ptr<Camera> camera, Player* player, std::weak_ptr<GameManager> gameManager, std::weak_ptr<Map> map) :
	m_pCamera(camera),
	m_pPlayer(player),
	m_pGameManager(gameManager),
	m_pMap(map),
	m_frameCount(0),
	m_itemTime(0),
	m_spawnTime(kSpawnTime)
{
	m_enemies.clear(); // 敵リストを初期化
	m_spawnPositions.clear(); // スポーン位置リストを初期化
	LoadSpawnPositions(); // 敵スポーン位置の読み込み

	// 敵画像の読み込み
	int handle = LoadGraph(L"data/img/transform_enemy.png");
	m_graphHandles.push_back(handle);

	// 敵アニメーションの読み込み
	if (AnimationLoader::LoadAnimationData(L"data/animation/transform_enemy.csv", handle, m_transformAnimations))
	{
#ifdef _DEBUG
		printfDx(L"変身敵アニメーションデータのロードに成功\n");
#endif
	}

}

EnemyManager::~EnemyManager()
{
	for (auto& handle : m_graphHandles)
	{
		DeleteGraph(handle);
	}
}

void EnemyManager::Init(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera)
{
	m_pMap = map;
	m_pCamera = camera;
	m_enemies.clear(); // 敵リストを初期化
	m_spawnPositions.clear(); // スポーン位置リストを初期化
	LoadSpawnPositions(); // 敵スポーン位置の読み込み
	m_frameCount = 0; // 経過フレームを初期化
	m_itemTime = 0; // アイテム化時間を初期化
}

void EnemyManager::Update(Input& input)
{
	m_frameCount++;
	m_itemTime--;

	// 敵の生成処理
	if (m_frameCount % m_spawnTime == 0) // 敵のスポーン時間になったら
	{
		if (IsChangeToItem()) // アイテム化状態の場合
		{
#ifdef _DEBUG
			printfDx(L"アイテム化中のため敵が生成できない\n");
#endif
		}
		else
		{
			if (m_spawnPositions.empty())
			{
				return;
			}
			Position2 spawnPos = SearchNearestSpawnPosition(m_pPlayer->GetPos()); // プレイヤーに一番近いスポーン位置を取得
			SpawnEnemy(spawnPos); // 敵を生成
		}
	}

	// 敵の更新
	for (auto& enemy : m_enemies)
	{
		enemy->Update(input);
		if (IsChangeToItem()) // アイテム化している場合
		{
			if(enemy->IsDead()) // アイテム化で取得されたら
			{
				enemy->OnCollected(m_pGameManager);
			}
		}
	}

	// 敵の削除
	if (IsChangeToItem()) // アイテム化状態の場合
	{
		// 敵に存在しないものがある場合削除
		m_enemies.remove_if(
			[](std::shared_ptr<EnemyBase> enemy)
			{
				// 死んだ判定のときにのみ削除
				return enemy->IsDead();
			}
		);
	}
	else // アイテム化状態ではない場合
	{
		// 敵に存在しないものがある場合削除
		m_enemies.remove_if(
			[](std::shared_ptr<EnemyBase> enemy)
			{
				// 死んだもしくは画面外にいるときに削除
				return enemy->IsDead() || !enemy->IsOnScreen();
			}
		);
	}
}

void EnemyManager::Draw()
{
#ifdef _DEBUG
	for(const auto& pos : m_spawnPositions)
	{
		Position2 nearPos = SearchNearestSpawnPosition(m_pPlayer->GetPos());
		if(pos.x == nearPos.x && pos.y == nearPos.y)
		{
			// 最近点は色を変える
			auto camera = m_pCamera.lock();
			int drawX = static_cast<int>(pos.x - camera->scroll.x);
			int drawY = static_cast<int>(pos.y - camera->scroll.y);
			// スポーン位置を表示(デバッグ用)
			DrawCircle(drawX, drawY, kSpawnPosRadius, kNearSpawnPosColor, FALSE);
			continue;
		}
		auto camera = m_pCamera.lock();
		int drawX = static_cast<int>(pos.x - camera->scroll.x);
		int drawY = static_cast<int>(pos.y - camera->scroll.y);
		// スポーン位置を表示(デバッグ用)
		DrawCircle(drawX, drawY, kSpawnPosRadius, kNormalSpawnPosColor, FALSE);
	}
#endif

}

void EnemyManager::LoadSpawnPositions()
{
	// マップを使えるようにlockしておく
	auto pMap = m_pMap.lock();
	for (int y = 0; y < pMap->GetMapHeight(); y++)
	{
		for (int x = 0; x < pMap->GetMapWidth(); x++)
		{
			if (pMap->GetPositioningData(x, y) == kSpawnChipNo)
			{
				// マップチップの1マスのサイズを取得(拡大を含む)
				float tileSize = pMap->GetTileSize();
				// スポーン位置はマップの中心にする
				Position2 pos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				// スポーン位置リストに追加
				m_spawnPositions.push_back(pos);
			}
		}
	}
}

void EnemyManager::SpawnEnemy(const Position2& pos)
{
	// とりあえず生成する敵をランダムに
	auto enemyForm = TransformEnemy::EnemyForm::PlayerSeeker;
	int rand = GetRand(kEnemyFormNum);
	if (rand == kSpawnFireBallNum)
	{
		enemyForm = TransformEnemy::EnemyForm::FireBall;
	}
	else if (rand == kSpawnSkullNum)
	{
		enemyForm = TransformEnemy::EnemyForm::Skull;
	}

	auto enemy = std::make_shared<TransformEnemy>(pos, m_pPlayer, m_pMap, enemyForm, m_transformAnimations);
	enemy->SetCamera(m_pCamera);
	m_enemies.push_back(enemy);
}

Position2 EnemyManager::SearchNearestSpawnPosition(const Position2& playerPos)
{
	if(m_spawnPositions.empty())
	{
		return playerPos; // スポーン位置が存在しない場合はプレイヤー位置を返す
	}
	// 最初のスポーン位置を最近点として設定
	Position2 nearestPos = m_spawnPositions.front();
	for (const auto& spawnPos : m_spawnPositions)
	{
		// プレイヤーから一番近いスポーン位置を探す
		if (Geometry::GetDistance(playerPos, spawnPos) < Geometry::GetDistance(playerPos, nearestPos)) // 現在の最近点よりも近い場合
		{
			// 最近点を更新
			nearestPos = spawnPos;
		}
	}
	return nearestPos;
}

bool EnemyManager::IsChangeToItem()const
{
	// アイテム化状態のカウントが進んでいるときにtrueを返す
	return m_itemTime > 0;
}

void EnemyManager::PushActors(std::vector<std::weak_ptr<Actor>>& actors)
{
	for (auto& enemy : m_enemies)
	{
		actors.push_back(enemy);
	}
}

size_t EnemyManager::GetEnemyNum()const
{
	if (m_enemies.empty())
	{
		return 0;
	}
	return m_enemies.size();
}

const float EnemyManager::GetItemTimeRate()const
{
	// アイテム化している場合
	if (IsChangeToItem())
	{
		// 残り時間の割合を計算して返す
		return static_cast<float>(m_itemTime) / kChangeToItemTime;
	}
	else
	{
		// アイテム化していない場合は0にする
		return 0.0f;
	}
}

void EnemyManager::ChangeToItemAll()
{
	m_itemTime = kChangeToItemTime;
	for (auto& enemy : m_enemies)
	{
		enemy->ChangeToItem(kChangeToItemTime);
	}
}

void EnemyManager::SpawnEnemy(const Position2& pos, int spawnNum)
{
	auto enemyForm = (TransformEnemy::EnemyForm)spawnNum;

	auto enemy = std::make_shared<TransformEnemy>(pos, m_pPlayer, m_pMap, enemyForm, m_transformAnimations);
	enemy->SetCamera(m_pCamera);
	m_enemies.push_back(enemy);
}

void EnemyManager::SpawnFast()
{
	m_spawnTime = kSpawnTimeFast;
}
