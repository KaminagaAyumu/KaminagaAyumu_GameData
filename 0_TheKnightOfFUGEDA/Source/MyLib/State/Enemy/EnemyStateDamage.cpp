#include "EnemyStateDamage.h"
#include "EnemyStateSearch.h"
#include "../../Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../MyLib/GameObject.h"
#include <cassert>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kHitAnimName = L"Enemy|Hit";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	constexpr float kKnockBackSpeed = 0.075f;

	// エフェクトが出るオフセット
	const Vector3 kEffectOffset = { 0.0f,1.0f,0.0f };
}

MyLib::EnemyStateDamage::EnemyStateDamage()
{
}

MyLib::EnemyStateDamage::~EnemyStateDamage()
{
}

void MyLib::EnemyStateDamage::OnInit(EnemyController* owner)
{
	m_pAnimator = owner->GetAnimator();

	std::shared_ptr<MyLib::Animator> animator = m_pAnimator.lock();

	animator->ChangeAnimation(kHitAnimName, kAnimSpeed, kAnimBlendFrame, false);

	std::shared_ptr<MyLib::Transform> pTransform = owner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();

	m_knockBackDir = -pTransform->GetDir();
	m_knockBackDir.Normalize();

	auto effectComponent = owner->GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

	effectComponent->PlayEffect(L"hit_2.efk");
}

void MyLib::EnemyStateDamage::OnUpdate()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

	if (!pRigidbody)
	{
		assert(false && "EnemyStateChase : 敵のRigidbodyの取得に失敗しました");
	}

	pRigidbody->SetVelocity(m_knockBackDir * kKnockBackSpeed);
	pRigidbody->SetIsApplyDirection(false);

	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	if (pAnimator->GetAnimEnd())
	{
		m_pStateMachine->ChangeState<MyLib::EnemyStateSearch>();
		return;
	}
}

void MyLib::EnemyStateDamage::OnEnd()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

	if (!pRigidbody)
	{
		assert(false && "EnemyStateChase : 敵のRigidbodyの取得に失敗しました");
	}

	pRigidbody->SetVelocity(Vector3::Zero());
	pRigidbody->SetIsApplyDirection(true);
}
