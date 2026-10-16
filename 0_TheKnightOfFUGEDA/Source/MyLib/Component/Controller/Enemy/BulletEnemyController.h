#pragma once
#include "EnemyController.h"

namespace MyLib
{
	class Transform;
	class Rigidbody;
	class Animator;
	class SphereCollider;
	/// <summary>
	/// 弾を撃つ敵の挙動を制御するコンポーネント
	/// </summary>
	class BulletEnemyController : public EnemyController
	{
	public:
		BulletEnemyController();
		virtual ~BulletEnemyController();

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

	private:
		// 敵の体力
		int m_hp;

		std::shared_ptr<MyLib::Transform> m_pModelOffset;	// 敵のモデルのオフセット
	};
}



