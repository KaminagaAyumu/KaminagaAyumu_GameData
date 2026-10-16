#include "PlayerStateIdle.h"
#include "PlayerStateWalk.h"
#include "PlayerStateAttack.h"
#include "PlayerStateGuard.h"
#include "PlayerStateDash.h"
#include "../StateMachine.h"
#include "../../../Utility/Input.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "DxLib.h"

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kIdleAnimName = L"Player|Idle";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::PlayerStateIdle::PlayerStateIdle()
{
}

void MyLib::PlayerStateIdle::OnInit(PlayerController* owner)
{
	auto animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kIdleAnimName, kAnimSpeed, kAnimBlendFrame, true);
}

void MyLib::PlayerStateIdle::OnUpdate()
{
	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	// XInputの入力データを取得
	Input::XInputData stickData = input.GetXInputData();

	// ガードの入力があればガード状態に遷移する
	if (input.IsPressed("Guard"))
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateGuard>();
		return;
	}

	// スティックの入力がある場合は移動状態に遷移する
	if (stickData.leftStick.Length() != 0.0f)
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateWalk>();
		return;
	}

	if (input.IsTriggered("AButton"))
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateAttack>();
		return;
	}
	
	if (input.IsTriggered("BButton"))
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateDash>();
		return;
	}
}

void MyLib::PlayerStateIdle::OnEnd()
{
}