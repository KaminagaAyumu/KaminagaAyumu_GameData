#pragma once
#include "Actor.h"
#include <memory>

// プロトタイプ宣言
class Player;
class Map;
class Input;
class GameManager;

/// <summary>
/// 敵の基底クラス
/// 純粋仮想関数が5つあります
/// </summary>
class EnemyBase : public Actor
{
public:
	EnemyBase(Player* player, std::weak_ptr<Map> map);
	virtual ~EnemyBase();
	virtual void Init() abstract;
	virtual void Update(Input&) abstract;
	virtual void Draw() abstract;

	virtual void IsCollision(const Types::CollisionInfo& info) abstract;

	bool IsDead()const { return m_isDead; }

	/// <summary>
	/// 敵が画面内にいるかどうか
	/// </summary>
	/// <returns>true : 画面内にいる false : 画面外にいる</returns>
	bool IsOnScreen()const;

	/// <summary>
	/// 敵をアイテムに変える
	/// </summary>
	/// <param name="time">アイテムに変える時間</param>
	virtual void ChangeToItem(int time)abstract;

	/// <summary>
	/// 敵コインが取得された際の処理
	/// </summary>
	/// <param name="gameManager">ゲームマネージャーのポインタ</param>
	virtual void OnCollected(std::weak_ptr<GameManager> gameManager)abstract;

protected:

	// プレイヤーの生ポインタ
	// 座標を調べるためにしか使わないが、
	// 更新処理で受け取るのが面倒なためコンストラクタで受け取る
	Player* m_pPlayer;

	// マップと当たり判定を取る用(一応weak_ptrにしておく)
	std::weak_ptr<Map> m_pMap;

	// 移動方向
	Vector2 m_direction;

	// 敵が死んだか
	bool m_isDead;

};

