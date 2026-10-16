#pragma once
#include "EnemyController.h"

namespace MyLib
{

	class SphereCollider;

	class SkullEnemyController : public EnemyController
	{
	public:
		SkullEnemyController();
		virtual ~SkullEnemyController();

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;

		std::weak_ptr<SphereCollider> GetAttackCollider() { return m_pAttackCollider; }

	private:

		// HP
		int m_hp;

		std::shared_ptr<MyLib::Transform> m_pModelOffset;	// 敵のモデルのオフセット

		std::weak_ptr<SphereCollider> m_pAttackCollider;

		// スタン状態かどうか
		bool m_isStunned;
	};
}


