#include "EnemyStateShot.h"
#include "EnemyStateSearch.h"
#include "../../GameObject.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/ObjectFactory.h"
#include "../../../Common/Sound/SoundManager.h"
#include <string>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kShotAnimName = L"Enemy|LaunchBullet";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}


MyLib::EnemyStateShot::EnemyStateShot()
{
}

MyLib::EnemyStateShot::~EnemyStateShot()
{
}

void MyLib::EnemyStateShot::OnInit(EnemyController* owner)
{
	m_pAnimator = owner->GetAnimator();

	std::shared_ptr<MyLib::Animator> animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kShotAnimName, kAnimSpeed, kAnimBlendFrame, false);

	MyLib::ObjectFactory::CreateBullet(owner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock()->GetPos(), owner->GetPlayerPos().lock()->GetPos(), owner->GetOwnerObj());

	SoundManager::GetInstance().Play("Shoot", 1.0f, true);
}

void MyLib::EnemyStateShot::OnUpdate()
{
	std::shared_ptr<MyLib::Animator> animator = m_pAnimator.lock();

	if (animator->GetAnimEnd())
	{
		m_pStateMachine->ChangeState<MyLib::EnemyStateSearch>();
		return;
	}
}

void MyLib::EnemyStateShot::OnEnd()
{
}
