#pragma once
#include "../Actor.h"
class EnemyBase : public Actor
{
public:
	EnemyBase(ObjectTag tag, MyLib::ColliderBase::ColliderType type);
	virtual ~EnemyBase();

	virtual void Init()abstract;
	virtual void End()abstract;
	virtual void Update()abstract;
	virtual void Draw()const abstract;

	virtual void OnCollide(std::shared_ptr<Collidable> collider)abstract;

	/// <summary>
	/// 現在の座標を返す
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPos() const { return m_rigidbody.GetPos(); }

	/// <summary>
	/// 敵を倒した際のスコアを取得する
	/// </summary>
	/// <returns></returns>
	virtual const int GetKillScore() const abstract;

	/// <summary>
	/// 敵が通常の状態かを判定する
	/// </summary>
	/// <returns></returns>
	bool IsAlive()const { return m_state == EnemyState::Alive; }

	/// <summary>
	/// 敵が死亡アニメーション中かどうかを判定する
	/// </summary>
	/// <returns></returns>
	bool IsDying()const { return m_state == EnemyState::Dying; }

	/// <summary>
	/// 敵が完全に死亡したかを判定する
	/// </summary>
	/// <returns></returns>
	bool IsDead()const { return m_state == EnemyState::Dead; }

	/// <summary>
	/// カメラの範囲内かどうかをセット
	/// </summary>
	/// <param name="isInCamera"></param>
	void SetIsInCamera(bool isInCamera) { m_isInCamera = isInCamera; }

	/// <summary>
	/// 敵の状態
	/// </summary>
	enum class EnemyState
	{
		Alive, // 通常
		Dying, // 死亡アニメーション中
		Dead   // 完全に死亡
	};

protected:

	// カメラの範囲内かどうか
	bool m_isInCamera;

	// 敵の状態
	EnemyState m_state;
};

