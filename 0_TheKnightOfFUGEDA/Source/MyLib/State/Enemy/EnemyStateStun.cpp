#include "EnemyStateStun.h"
#include "EnemyStateSearch.h"
#include "../../Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include <string>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kStunAnimName = L"Enemy|Dance";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// スタン状態のフレーム数
	constexpr int kStunFrame = 300;
}

MyLib::EnemyStateStun::EnemyStateStun() : 
	m_stunFrame(kStunFrame)
{
}

MyLib::EnemyStateStun::~EnemyStateStun()
{
}

void MyLib::EnemyStateStun::OnInit(EnemyController* owner)
{
	std::shared_ptr<MyLib::Animator> animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kStunAnimName, kAnimSpeed, kAnimBlendFrame, true);

	m_stunFrame = kStunFrame;
}

void MyLib::EnemyStateStun::OnUpdate()
{
	m_stunFrame--;

	if (m_stunFrame <= 0)
	{
		m_pStateMachine->ChangeState<EnemyStateSearch>();
		return;
	}
}

void MyLib::EnemyStateStun::OnEnd()
{
}
