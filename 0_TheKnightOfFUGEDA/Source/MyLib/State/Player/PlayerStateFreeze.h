#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class PlayerController;

	class PlayerStateFreeze : public StateBase<PlayerController>
	{
	public:

		PlayerStateFreeze();
		virtual ~PlayerStateFreeze() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;
	};
}



