#pragma once
#include "../StateBase.h"
#include "../../../Geometry/Vector3.h"

namespace MyLib
{
	class PlayerController;
	class Animator;

	class PlayerStateDamage : public StateBase<PlayerController>
	{
	public:

		PlayerStateDamage();
		virtual ~PlayerStateDamage() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:

		std::weak_ptr<Animator> m_pAnimator;

		// ノックバックの向き
		Vector3 m_knockBackDir;
	};
}


