#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class PlayerController;
	class Animator;

	/// <summary>
	/// プレイヤー移動状態
	/// </summary>
	class PlayerStateMove : public StateBase<PlayerController>
	{
	public:

		PlayerStateMove();
		virtual ~PlayerStateMove() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Animator> m_pAnimator;


	};
}


