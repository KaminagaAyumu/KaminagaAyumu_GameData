#pragma once
#include "../StateBase.h"
#include "../../../Utility/CSV/PlayerAttackResource.h"
#include "../../../Geometry/Vector3.h"

class AttackData;
class Effect;

namespace MyLib
{
	class PlayerController;
	class Animator;
	class Collidable;
	class SphereCollider;

	/// <summary>
	/// プレイヤー攻撃状態
	/// </summary>
	class PlayerStateAttack : public StateBase<PlayerController>
	{
	public:
		PlayerStateAttack();
		virtual ~PlayerStateAttack() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Animator> m_pAnimator;

		std::weak_ptr<SphereCollider> m_pAttackCol;

		std::weak_ptr<Effect> m_pAttackEffect;

		PlayerAttackResource m_attackResource;

		int m_frameCount;

		bool m_isNextAttack;

		AttackData* m_currentAttackData;

	private:

		void PlayAttackEffect();

		/// <summary>
		/// 攻撃を行う向きを返す
		/// </summary>
		/// <returns></returns>
		Vector3 GetAttackDir();

		/// <summary>
		/// 移動速度の更新を行う
		/// </summary>
		void UpdateMoveVelocity();
	};

}


