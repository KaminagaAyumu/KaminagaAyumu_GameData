#include "PlayerStateFreeze.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kIdleAnimName = L"Player|Idle";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::PlayerStateFreeze::PlayerStateFreeze()
{
}

void MyLib::PlayerStateFreeze::OnInit(PlayerController* owner)
{
	auto animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kIdleAnimName, kAnimSpeed, kAnimBlendFrame, true);
}

void MyLib::PlayerStateFreeze::OnUpdate()
{
}

void MyLib::PlayerStateFreeze::OnEnd()
{
}
