#pragma once
#include "EnemyBase.h"

class Model;
class Player;
class BulletManager;

/// <summary>
/// 敵クラス
/// </summary>
class Enemy : public EnemyBase
{
public:
	Enemy();
	virtual ~Enemy();

	void Init()override;
	void End()override;
	void Update()override;
	void Draw()const override;

	void OnCollide(std::shared_ptr<Collidable> collider)override;

	const int GetKillScore()const override;

	void SetModel(int handle);

	/// <summary>
	/// 生成座標を設定
	/// </summary>
	/// <param name="pos"></param>
	void SetSpawnPos(const Vector3& pos);

	/// <summary>
	/// プレイヤーを取得
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(std::shared_ptr<Player> player);

	/// <summary>
	/// バレットマネージャーを取得
	/// </summary>
	/// <param name="manager"></param>
	void SetBulletManager(std::shared_ptr<BulletManager> manager);

	/// <summary>
	/// ダメージを受けたとき
	/// </summary>
	void Damage();

private:

	// モデルクラス
	std::shared_ptr<Model> m_pModel;

	// 座標情報を取得するときなどに使う
	std::weak_ptr<Player> m_pPlayer;

	// 弾を生成するときに使う
	std::weak_ptr<BulletManager> m_pBulletManager;

	// 現在の向き
	Vector3 m_dir;

	// プレイヤーを検知する半径
	float m_detectRadius;

	// プレイヤーを検知する球の色
	unsigned int m_detectColor;

	// 攻撃するまでの時間
	int m_attackCount;

	// プレイヤーの検知が終わってからの時間
	int m_detectEndCount;

	// プレイヤーを見失う時間
	int m_lostCount;

	// 更新処理用関数ポインタの型定義
	using UpdateFunc_t = void(Enemy::*)();
	// 更新処理用関数
	UpdateFunc_t m_update;


private:
	/// <summary>
	/// プレイヤーが検知範囲に入っているかどうか
	/// </summary>
	bool IsInDetectRange();

	/// <summary>
	/// 弾を生成
	/// </summary>
	void Shoot();

	void IdleUpdate();

	void DetectUpdate();

	void AttackUpdate();

	void LostUpdate();

	void DamageUpdate();

	/// <summary>
	/// モデルの情報を更新する
	/// </summary>
	void UpdateModel();

	/// <summary>
	/// プレイヤーを発見した時の処理
	/// </summary>
	void OnDetect();
};

