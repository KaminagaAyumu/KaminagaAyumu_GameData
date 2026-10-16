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
	class PlayerStateRun : public StateBase<PlayerController>
	{
	public:

		PlayerStateRun();
		virtual ~PlayerStateRun() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Animator> m_pAnimator;

		std::weak_ptr<SphereCollider> m_pGuardCol;

		// エフェクトを出すフレーム数
		int m_particleFrame;

	};
}

