#include "PlayerStateDamage.h"
#include "PlayerStateIdle.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Collider/SphereCollider.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../Common/Effect/EffectManager.h"

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kDamageAnimName = L"Player|Damage";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	constexpr float kKnockBackSpeed = 0.05f;

}

MyLib::PlayerStateDamage::PlayerStateDamage()
{
}

void MyLib::PlayerStateDamage::OnInit(PlayerController* owner)
{
	m_pAnimator = owner->GetAnimator();

	std::shared_ptr<MyLib::Animator> animator = m_pAnimator.lock();

	animator->ChangeAnimation(kDamageAnimName, kAnimSpeed, kAnimBlendFrame, false);

	std::shared_ptr<MyLib::Transform> pTransform = m_pOwner->GetTransform().lock();

	m_knockBackDir = -pTransform->GetDir();

	std::shared_ptr<MyLib::Rigidbody> pRigidbody = m_pOwner->GetRigidbody().lock();

	pRigidbody->SetVelocity(m_knockBackDir * kKnockBackSpeed);

	pRigidbody->SetIsApplyDirection(false);

	// ダメージ処理中は攻撃とガードの当たり判定を非アクティブにする
	std::shared_ptr<MyLib::SphereCollider> pAttackCol = m_pOwner->GetAttackCollider().lock();
	pAttackCol->SetEnable(false);
	std::shared_ptr<MyLib::SphereCollider> pGuardCol = m_pOwner->GetGuardCollider().lock();
	pGuardCol->SetEnable(false);

	auto effectComponent = m_pOwner->GetEffectComponent().lock();

	effectComponent->PlayEffect(L"playerHit.efk");
}

void MyLib::PlayerStateDamage::OnUpdate()
{
	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	if (pAnimator->GetAnimEnd())
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
		return;
	}

}

void MyLib::PlayerStateDamage::OnEnd()
{
	std::shared_ptr<MyLib::Rigidbody> pRigidbody = m_pOwner->GetRigidbody().lock();

	pRigidbody->SetVelocity(Vector3::Zero());

	pRigidbody->SetIsApplyDirection(true);
}
