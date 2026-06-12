#pragma once
#include <memory>
#include <list>
#include <vector>
#include "../../Geometry/Vector3.h"
#include "../../Common/Game.h"

class EnemyBase;
class Player;
class Camera;
class BulletManager;

/// <summary>
/// 敵の生成管理クラス
/// </summary>
class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();

	void Init();
	void End();
	/// <summary>
	/// 更新処理(カメラ範囲に入っている敵のみ更新)
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	const int Update(std::shared_ptr<Camera> camera);
	void Draw() const;

	/// <summary>
	/// 敵を全員倒したかどうかを取得する
	/// </summary>
	/// <returns>true : 全員倒した false : 倒していない</returns>
	bool IsKilledAll()const;

	/// <summary>
	/// ステージ番号から敵を配置する
	/// </summary>
	/// <param name="stage">ステージ番号</param>
	void SpawnEnemyToStageNo(StageType stage, std::shared_ptr<Player> player, std::shared_ptr<BulletManager> bulletManager);

private:

	/// <summary>
	/// 敵の種類
	/// </summary>
	enum class EnemyType
	{
		Normal, // 通常の敵
		Move,	// 動く敵
		None,
	};

	/// <summary>
	/// 敵生成情報
	/// </summary>
	struct EnemyInfo
	{
		EnemyType type;
		Vector3 pos;
	};

	// モデルのハンドルコンテナ
	std::vector<int> m_modelHandles;

	// 敵リスト
	std::list<std::shared_ptr<EnemyBase>> m_pEnemies;

	// 敵を配置する座標
	std::vector<std::vector<EnemyInfo>> m_enemySpawnPositions;



private:

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="type">敵の種類</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="bulletManager">弾生成クラスのポインタ</param>
	void CreateEnemy(const Vector3& pos, EnemyManager::EnemyType type, std::shared_ptr<Player> player, std::shared_ptr<BulletManager> bulletManager);


};

