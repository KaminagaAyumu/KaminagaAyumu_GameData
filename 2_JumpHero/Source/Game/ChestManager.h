#pragma once
#include <memory>
#include <list>
#include <vector>
#include <unordered_map>
#include "../Utility/Geometry.h"

class Actor;
class Map;
class Input;
class Camera;
class Chest;
class GameManager;

/// <summary>
/// ゲーム中の宝箱を管理するクラス
/// 宝箱の生成、表示を行う
/// </summary>
class ChestManager
{
public:
	ChestManager(std::weak_ptr<Camera> camera, GameManager* gameManager);
	virtual ~ChestManager();

	void Init(std::weak_ptr<Camera> camera);

	void Update(Input& input);

	void Draw();

	// マップのデータを受け取って宝箱を生成
	void SpawnChest(std::weak_ptr<Map> map);

	void OpenChestAtPosition(int tileX, int tileY);

	/// <summary>
	/// ゲーム内オブジェクトに宝箱を追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<std::weak_ptr<Actor>>& actors);

	size_t GetChestNum();

private:
	struct TilePos
	{
		int x;
		int y;
		// 比較できるようにする
		bool operator==(const TilePos& other) const
		{
			return x == other.x && y == other.y;
		}
	};

	struct Hash
	{
		size_t operator()(const TilePos& pos) const
		{
			return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
		}
	};

	int m_chestGraphHandle; // 宝箱の画像ハンドル

	std::weak_ptr<Camera> m_pCamera; // 宝箱で使うカメラを参照
	GameManager* m_pGameManager; // イベント通知をするクラスを参照
	std::list<std::shared_ptr<Chest>> m_chests;
	std::unordered_map<TilePos, Chest*, Hash> m_chestMap; // タイル座標から宝箱を取得するためのマップ

};

