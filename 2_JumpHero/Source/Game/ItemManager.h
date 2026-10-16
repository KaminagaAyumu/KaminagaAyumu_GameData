#pragma once
#include <list>
#include <functional>
#include "../Utility/GameType.h"
#include "../Utility/Geometry.h"

class Actor;
class ItemBase;
class Input;
class Camera;
class GameManager;
class Map;

/// <summary>
/// アイテム管理用クラス
/// アイテムの生成、更新
/// </summary>
class ItemManager
{
public:
	ItemManager(std::weak_ptr<Camera> camera, std::weak_ptr<GameManager> gameManager);
	~ItemManager();

	/// <summary>
	/// 初期化を行う関数
	/// </summary>
	void Init(std::weak_ptr<Camera> camera);

	/// <summary>
	/// シーン上のアイテムの更新処理を行う関数
	/// </summary>
	void Update(Input& input);

	/// <summary>
	/// シーン上のアイテムの描画処理を行う関数
	/// </summary>
	void Draw();

	/// <summary>
	/// 指定した座標にアイテムの追加を行う
	/// </summary>
	/// <param name="pos">表示座標</param>
	/// <param name="itemType">どのアイテムを生成するか</param>
	void SpawnItem(const Position2& pos, Types::ItemType itemType);

	/// <summary>
	/// 指定した座標に動く敵をコインに変えるアイテムを生成
	/// </summary>
	/// <param name="pos">表示座標</param>
	/// <param name="map">現在のマップのポインタ</param>
	void SpawnFloatingChangeToCoin(const Position2& pos, std::weak_ptr<Map> map);

	/// <summary>
	/// ゲーム開始時の最初のアイテム生成
	/// </summary>
	/// <param name="map">現在のマップのポインタ</param>
	void FirstSpawnItem(std::weak_ptr<Map> map);

	/// <summary>
	/// ゲーム内オブジェクトにアイテムを追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<std::weak_ptr<Actor>>& actors);

	/// <summary>
	/// 現在のアイテムの総数を取得する関数
	/// </summary>
	/// <returns>現在のアイテムの総数</returns>
	size_t GetItemNum();

	/// <summary>
	/// 初期配置の風船の数を取得する関数
	/// </summary>
	/// <returns>風船の数</returns>
	int GetFirstBalloonNum() const { return m_firstBalloonNum; }

private:
	std::weak_ptr<Camera> m_pCamera; // アイテムで使うカメラを取得
	std::shared_ptr<GameManager> m_pGameManager; // アイテムで使うゲームマネージャーを取得
	std::list<std::shared_ptr<ItemBase>> m_pItems; // シーン上のアイテム管理用コンテナ
	std::vector<int> m_graphHandles; // アイテムのグラフィックハンドル格納用コンテナ
	int m_firstBalloonNum; // 初期配置の風船の個数

};

