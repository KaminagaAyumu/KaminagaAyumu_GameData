#include "PlayerStateDead.h"
#include "PlayerStateDie.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include <string>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kDeadAnimName = L"Player|Dead";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 1.0f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// ノックバックの速度
	constexpr float kKnockBackSpeed = 0.1f;
}

MyLib::PlayerStateDead::PlayerStateDead()
{
}

void MyLib::PlayerStateDead::OnInit(PlayerController* owner)
{
	m_pAnimator = owner->GetAnimator();

	std::shared_ptr<MyLib::Animator> animator = m_pAnimator.lock();

	animator->ChangeAnimation(kDeadAnimName, kAnimSpeed, kAnimBlendFrame, false);
}

void MyLib::PlayerStateDead::OnUpdate()
{
	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	if (pAnimator->GetAnimEnd())
	{
		m_pStateMachine->ChangeState<PlayerStateDie>();
		return;
	}
}

void MyLib::PlayerStateDead::OnEnd()
{
}
