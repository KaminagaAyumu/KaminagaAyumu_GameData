#pragma once
#include <string>
#include <unordered_map>

class Camera;
class GameManager;
class Map;
class Actor;
class Player;
class EnemyBase;
class Input;

/// <summary>
/// 敵管理用クラス
/// 敵の生成、更新を行う
/// </summary>
class EnemyManager
{
public:
	EnemyManager(std::weak_ptr<Camera> camera,Player* player, std::weak_ptr<GameManager> gameManager, std::weak_ptr<Map> map);
	virtual ~EnemyManager();

	void Init(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera);

	void Update(Input&);

	void Draw();

	/// <summary>
	/// ゲーム内オブジェクトに宝箱を追加する
	/// </summary>
	/// <param name="actors">アクターコンテナの参照</param>
	void PushActors(std::vector<std::weak_ptr<Actor>>& actors);

	size_t GetEnemyNum()const;

	/// <summary>
	/// アイテム化している時間の割合を取得する
	/// </summary>
	/// <returns></returns>
	const float GetItemTimeRate()const;

	/// <summary>
	/// 敵をアイテムに変える処理
	/// </summary>
	void ChangeToItemAll();

	/// <summary>
	/// 敵を指定座標にスポーンさせる
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="spawnNum">スポーンする敵の番号</param>
	void SpawnEnemy(const Position2& pos, int spawnNum);

	/// <summary>
	/// 敵のスポーンの速度を早める(基本初期化時に呼ぶ)
	/// </summary>
	void SpawnFast();

private:

	/// <summary>
	/// 敵の生成位置をマップから読み込む
	/// </summary>
	void LoadSpawnPositions();

	// 座標のデータを受け取って敵を生成
	void SpawnEnemy(const Position2& pos);

	/// <summary>
	/// プレイヤーに最も近いスポーン位置を探す
	/// </summary>
	/// <param name="playerPos">プレイヤーの位置</param>
	/// <returns>敵スポーン位置</returns>
	Position2 SearchNearestSpawnPosition(const Position2& playerPos);

	std::weak_ptr<Camera> m_pCamera; // 敵が使うカメラを参照
	Player* m_pPlayer; // 敵が使うプレイヤーを参照
	std::weak_ptr<GameManager> m_pGameManager; // イベント通知をするクラスを参照
	std::weak_ptr<Map> m_pMap; // 敵生成用にマップを参照
	std::list<std::shared_ptr<EnemyBase>> m_enemies;

	std::vector<Position2> m_spawnPositions; // 敵の生成位置リスト

	std::vector<int> m_graphHandles; // 敵の画像ハンドルリスト

	std::unordered_map<std::string, Animation> m_transformAnimations; // 変身する敵のアニメーションリスト

	int m_frameCount; // フレーム数をカウント
	int m_itemTime; // アイテム状態の時間

	int m_spawnTime; // 敵を生成する時間 

	/// <summary>
	/// 敵がアイテム化状態になっているか
	/// </summary>
	/// <returns>true : アイテム状態 false : アイテム状態以外</returns>
	bool IsChangeToItem() const;
};



