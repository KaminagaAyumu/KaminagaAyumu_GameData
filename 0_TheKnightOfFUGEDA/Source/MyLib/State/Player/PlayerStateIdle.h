#pragma once
#include "../StateBase.h"

namespace MyLib
{
	class PlayerController;

	/// <summary>
	/// プレイヤー待機状態
	/// </summary>
	class PlayerStateIdle : public StateBase<PlayerController>
	{
	public:

		PlayerStateIdle();
		virtual ~PlayerStateIdle() = default;

		virtual void OnInit(PlayerController* owner) override;
		virtual void OnUpdate() override;
		virtual void OnEnd() override;

	private:



	};
}


