#include <memory>
#include "PositionRegistry.h"
#include "../Utility/Map.h"

namespace
{
	constexpr int kEnemySpawnPosChipNo = 1; // 敵のスポーン位置のマップチップ番号
	constexpr int kSpawnPosChipNo = 7; // 敵のスポーン時にカメラが見る位置のマップチップ番号
	constexpr int kGoalPosChipNo = 8; // ゴールを見せる時にカメラが見る位置のマップチップ番号
	constexpr int kBarrierPosChipNo = 10; // バリアのマップチップ番号
}

PositionRegistry::PositionRegistry()
{
	m_areaPos.clear();
	m_chestPos.clear();
	m_barrierPos.clear();
	m_cameraPos.clear();
	m_spawnPos.clear();
}

PositionRegistry::~PositionRegistry()
{
}

bool PositionRegistry::InitPositions(std::shared_ptr<Map> map)
{
	int barrierId = 0; // バリアで使用するためのID
	int chestId = 0; // 宝箱で使用するためのID

	// マップの右端からループ
	for (int x = 0; x < map->GetMapWidth(); x++)
	{
		for (int y = 0; y < map->GetMapHeight(); y++)
		{
			// イベント発火IDと座標を取得
			int chipNo = map->GetEventData(x, y);
			if (map->IsEventFlagTile(chipNo))
			{
				int areaId = chipNo; // エリアのIDを設定
				float tileSize = map->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 areaPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_areaPos[areaId] = areaPos; // IDと座標を設定
			}

			if (chipNo == kSpawnPosChipNo)
			{
				float tileSize = map->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 cameraPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_cameraPos[kSpawnPosChipNo] = cameraPos; // IDと座標を設定
			}
			if (chipNo == kGoalPosChipNo)
			{
				float tileSize = map->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 cameraPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_cameraPos[kGoalPosChipNo] = cameraPos; // IDと座標を設定
			}
			if (chipNo == kBarrierPosChipNo)
			{
				barrierId++;
				float tileSize = map->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 barrierPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				ActivePosition2 barrier = { barrierPos,true };
				m_barrierPos[barrierId] = barrier; // IDと座標を設定
			}

			// 宝箱のIDと座標を取得
			chipNo = map->GetPositioningData(x, y);
			if (map->IsChestTile(chipNo))
			{
				chestId++; // IDを増加(1から始まる)
				float tileSize = map->GetTileSize(); // マップチップのサイズを取得
				// マップチップ座標からゲーム内座標に変換
				Position2 chestPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_chestPos[chestId] = chestPos; // IDと座標を設定
			}

			if (chipNo == kEnemySpawnPosChipNo)
			{
				float tileSize = map->GetTileSize(); // マップチップのサイズを取得
				Position2 spawnPos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				m_spawnPos.push_back(spawnPos);
			}
		}
	}

	// ここまで来たらtrueを返す
	return true;
}

const Position2 PositionRegistry::GetAreaPos(int id) const
{
	auto it = m_areaPos.find(id);
	if (it != m_areaPos.end())
	{
		return it->second;
	}
	return{};
}

const Position2 PositionRegistry::GetChestPos(int id) const
{
	auto it = m_chestPos.find(id);
	if (it != m_chestPos.end())
	{
		return it->second;
	}
	return{};
}

const ActivePosition2 PositionRegistry::GetBarrierPos(int id) const
{
	auto it = m_barrierPos.find(id);
	if (it != m_barrierPos.end())
	{
		return it->second;
	}
	return{};
}

bool PositionRegistry::SetBarrierActive(int id, bool isActive)
{
	auto it = m_barrierPos.find(id);
	if (it == m_barrierPos.end())
	{
		return false;
	}
	// 指定バリアのアクティブ状態を変更する
	it->second.isActive = isActive;
	return true;
}

const Position2 PositionRegistry::GetCameraPos(int id) const
{
	auto it = m_cameraPos.find(id);
	if (it != m_cameraPos.end())
	{
		return it->second;
	}
	return{};
}

const std::vector<Position2>& PositionRegistry::GetSpawnPosAll() const
{
	return m_spawnPos;
}
