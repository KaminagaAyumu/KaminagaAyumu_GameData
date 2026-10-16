#include "EnemyStateAttack.h"
#include "EnemyStateSearch.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Controller/Enemy/SkullEnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../MyLib/GameObject.h"
#include "../../../MyLib/Collider/SphereCollider.h"
#include "../../../Common/Sound/SoundManager.h"
#include <string>
#include <cassert>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kDetectAnimName = L"Enemy|Attack";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// 移動するスピード
	constexpr float kMoveSpeed = 0.07f;

	// 攻撃する距離
	constexpr float kAttackRange = 1.0f;

	// エフェクトが出るオフセット
	const Vector3 kEffectOffset = { 0.0f,0.0f,-1.0f };
}

MyLib::EnemyStateAttack::EnemyStateAttack()
{
}

MyLib::EnemyStateAttack::~EnemyStateAttack()
{
}

void MyLib::EnemyStateAttack::OnInit(EnemyController* owner)
{
	m_pOwner = owner;

	m_pAnimator = owner->GetAnimator();

	auto animator = m_pAnimator.lock();

	animator->ChangeAnimation(kDetectAnimName, kAnimSpeed, kAnimBlendFrame, false);

	// 攻撃コライダーを表示させる
	auto skullOwner = dynamic_cast<SkullEnemyController*>(owner);
	auto attackCol = skullOwner->GetAttackCollider().lock();

	if (attackCol)
	{
		attackCol->SetEnable(true);
	}

	// 攻撃時のエフェクトを生成
	auto effectComponent = owner->GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

	effectComponent->PlayEffect(L"enemyBite.efk", kEffectOffset);

	// 攻撃時のSEを再生
	SoundManager::GetInstance().Play("Die", 1.0f, true);
}

void MyLib::EnemyStateAttack::OnUpdate()
{
	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	if (pAnimator->GetAnimEnd())
	{
		m_pStateMachine->ChangeState<EnemyStateSearch>();
		return;
	}
}

void MyLib::EnemyStateAttack::OnEnd()
{
	auto skullOwner = dynamic_cast<SkullEnemyController*>(m_pOwner);
	auto attackCol = skullOwner->GetAttackCollider().lock();

	if (attackCol)
	{
		attackCol->SetEnable(false);
	}
}
