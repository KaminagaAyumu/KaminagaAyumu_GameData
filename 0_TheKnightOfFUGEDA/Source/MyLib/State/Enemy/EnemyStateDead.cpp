#include "EnemyStateDead.h"
#include "../../GameObject.h"
#include "../../Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include <cassert>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kDeadAnimName = L"Enemy|Dead";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// ノックバックの速度
	constexpr float kKnockBackSpeed = 0.1f;

	// エフェクトが出るオフセット
	const Vector3 kDeadEffectOffset = { 0.0f,-1.0f,0.0f };
}

MyLib::EnemyStateDead::EnemyStateDead()
{
}

MyLib::EnemyStateDead::~EnemyStateDead()
{
}

void MyLib::EnemyStateDead::OnInit(EnemyController* owner)
{
	SoundManager::GetInstance().Play("Dead", 1.0f, true);

	m_pOwnerObj = owner->GetOwnerObj();

	m_pAnimator = owner->GetAnimator();

	std::shared_ptr<MyLib::Animator> animator = m_pAnimator.lock();

	animator->ChangeAnimation(kDeadAnimName, kAnimSpeed, kAnimBlendFrame, false);

	std::shared_ptr<MyLib::Transform> pTransform = owner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();

	m_knockBackDir = -pTransform->GetDir();
	m_knockBackDir.Normalize();

	auto effectComponent = m_pOwnerObj.lock()->GetComponent<MyLib::EffectComponent>().lock();
	effectComponent->PlayEffect(L"hit_2.efk");
}

void MyLib::EnemyStateDead::OnUpdate()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

	if (!pRigidbody)
	{
		assert(false && "EnemyStateChase : 敵のRigidbodyの取得に失敗しました");
	}

	pRigidbody->SetVelocity(m_knockBackDir * kKnockBackSpeed);
	pRigidbody->SetIsApplyDirection(false);

	auto pAnimator = m_pAnimator.lock();

	if (pAnimator->GetAnimEnd())
	{
		auto effectComponent = m_pOwnerObj.lock()->GetComponent<MyLib::EffectComponent>().lock();
		effectComponent->PlayEffect(L"enemyDead.efk", kDeadEffectOffset);

		SoundManager::GetInstance().Play("Die", 1.0f, true);

		auto pOwnerObj = m_pOwnerObj.lock();
		pOwnerObj->Destroy();
	}
}

void MyLib::EnemyStateDead::OnEnd()
{
}
