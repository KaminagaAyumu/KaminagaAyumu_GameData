#pragma once
#include "../Actor.h"
#include <memory>
#include <list>

class Model;
class Effect;
class UIImage;

enum class PlayerState
{
	Idle, // 待機
	Walk, // 歩き
	Damage, // ダメージ
	Dead, // 死亡
};

class Player : public Actor
{
public:
	Player();
	virtual ~Player();

	void Init()override;
	void End()override;
	void Update()override;
	void Draw()const override;

	void OnCollide(std::shared_ptr<Collidable> collider) override;

	/// <summary>
	/// カメラのローカル正面方向ベクトルを取得
	/// </summary>
	/// <param name="dir"></param>
	void GetCameraDir(const Vector3 dir);

	/// <summary>
	/// プレイヤーの座標を返す
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPos()const { return m_rigidbody.GetPos(); }

	/// <summary>
	/// ダメージを受けているか
	/// </summary>
	/// <returns></returns>
	bool IsDamage() const;

	/// <summary>
	/// 回避状態かどうかを判定する
	/// </summary>
	/// <returns>true : 回避状態 false : それ以外の状態</returns>
	bool IsAvoid() const;

	/// <summary>
	/// プレイヤーが死んでいるかどうかを判定する
	/// </summary>
	/// <returns>true : 死んでいる false : 死んでいない</returns>
	bool IsDead() const;

	/// <summary>
	/// プレイヤーが死んだかどうかを判定する
	/// </summary>
	/// <returns>true : 死んだ false : 死んでいない</returns>
	bool IsDied() const;

private:
	// モデルのハンドル
	int m_modelHandle;

	// 動いているか
	bool m_isMoving;

	// ダメージを喰らっているか
	bool m_isDamage;

	// 無敵状態か
	bool m_isInvincible;

	// 回避状態か
	bool m_isAvoid;

	// 無敵時間
	int m_invincibleTime;

	// 回避時間
	int m_avoidTime;

	// 体力
	int m_life;

	// 体力UIを動かすための時間カウンタ
	int m_lifeUIAnimCount;

	// 最後に動いていた向き
	Vector3 m_lastMoveDir;

	// ノックバックベクトル
	Vector3 m_knockBack;

	// カメラの正面方向ベクトル
	Vector3 m_cameraDir;

	Vector3 m_forward;
	Vector3 m_right;

	// 表示するモデル
	std::shared_ptr<Model> m_model;

	// 更新の関数ポインタ名を定義
	using UpdateFunc_t = void (Player::*)();
	// 更新処理
	UpdateFunc_t m_update;

	std::weak_ptr<Effect> m_pEffect;

	// 体力のUI
	std::list<std::weak_ptr<UIImage>> m_pLifeUI;

private:

	/// <summary>
	/// 待機状態
	/// </summary>
	void IdleUpdate();

	/// <summary>
	/// 歩き状態
	/// </summary>
	void WalkUpdate();

	/// <summary>
	/// ダメージ状態
	/// </summary>
	void DamageUpdate();

	/// <summary>
	/// 死亡状態
	/// </summary>
	void DeadUpdate();

	/// <summary>
	/// 死亡後の状態
	/// </summary>
	void DieUpdate();

	/// <summary>
	/// 回避状態
	/// </summary>
	void AvoidUpdate();

	/// <summary>
	/// モデルの更新
	/// </summary>
	void UpdateModel();

	/// <summary>
	/// ダメージ判定が始まった時の処理
	/// </summary>
	void OnDamage();
};

