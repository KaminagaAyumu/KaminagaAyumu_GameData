#include "EnemyStateFreeze.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include <string>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kIdleAnimName = L"Enemy|Idle";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::EnemyStateFreeze::EnemyStateFreeze()
{
}

MyLib::EnemyStateFreeze::~EnemyStateFreeze()
{
}

void MyLib::EnemyStateFreeze::OnInit(EnemyController* owner)
{
	auto animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kIdleAnimName, kAnimSpeed, kAnimBlendFrame, true);
}

void MyLib::EnemyStateFreeze::OnUpdate()
{
}

void MyLib::EnemyStateFreeze::OnEnd()
{
}
