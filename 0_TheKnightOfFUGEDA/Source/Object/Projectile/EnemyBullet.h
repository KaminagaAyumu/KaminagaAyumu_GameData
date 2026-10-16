#pragma once
#include "../../MyLib/GameObject.h"
#include "../../MyLib/Collider/SphereCollider.h"
#include <memory>

class Effect;

/// <summary>
/// 敵が発射する弾のクラス
/// </summary>
class EnemyBullet : public MyLib::GameObject
{
public:

	EnemyBullet();
	virtual ~EnemyBullet();

	/// <summary>
	/// 初期化処理
	/// 生成位置とターゲットを取得
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="target"></param>
	void Init(const Position3& pos, const Vector3& target, std::weak_ptr<MyLib::GameObject> shooter);
	void Update();
	void End();

private:

	std::shared_ptr<MyLib::SphereCollider> m_pReflectCollider;
	std::weak_ptr<MyLib::GameObject> m_pShooter;

	std::weak_ptr<Effect> m_pBulletEffect;

};