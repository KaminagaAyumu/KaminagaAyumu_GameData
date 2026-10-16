#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class PlayerController;
	class Animator;
	class SphereCollider;

	/// <summary>
	/// プレイヤー移動状態
	/// </summary>
	class PlayerStateWalk : public StateBase<PlayerController>
	{
	public:

		PlayerStateWalk();
		virtual ~PlayerStateWalk() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Animator> m_pAnimator;

		std::weak_ptr<SphereCollider> m_pGuardCol;

	};
}

