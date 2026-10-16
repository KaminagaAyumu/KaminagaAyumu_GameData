#include "EnemyStateSearch.h"
#include "EnemyStateIdle.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include <string>
namespace
{
	// アニメーション名
	constexpr const std::wstring_view kSearchAnimName = L"Enemy|No";

	// このステートのフレーム数
	constexpr int kSearchFrame = 120;

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::EnemyStateSearch::EnemyStateSearch() : 
	m_searchFrame(kSearchFrame)
{
}

MyLib::EnemyStateSearch::~EnemyStateSearch()
{
}

void MyLib::EnemyStateSearch::OnInit(EnemyController* owner)
{
	std::shared_ptr<MyLib::Animator> animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kSearchAnimName, kAnimSpeed, kAnimBlendFrame, true);
}

void MyLib::EnemyStateSearch::OnUpdate()
{
	--m_searchFrame;

	// フレーム数が0以下になったら待機状態に戻る
	if (m_searchFrame < 0)
	{
		m_pStateMachine->ChangeState<MyLib::EnemyStateIdle>();
		return;
	}
}

void MyLib::EnemyStateSearch::OnEnd()
{
}
