#include "EnemyStateBack.h"
#include "EnemyStateRush.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../MyLib/GameObject.h"
#include "../../../Common/Sound/SoundManager.h"
#include <string>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kBackAnimName = L"Enemy|Walk";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// 突進するまでのフレーム数
	constexpr int kBackFrame = 120;

	// 後ろに下がる速度
	constexpr float kBackSpeed = -0.05f;

	const Vector3 kEffectOffsetRight = { 0.5f,0.0f,-0.5f };
	const Vector3 kEffectOffsetLeft = { -0.5f,0.0f,-0.5f };
}

MyLib::EnemyStateBack::EnemyStateBack() : 
	m_backFrame(0)
{
}

MyLib::EnemyStateBack::~EnemyStateBack()
{
}

void MyLib::EnemyStateBack::OnInit(EnemyController* owner)
{
	auto animator = owner->GetAnimator().lock();

	m_pOwner = owner;

	m_pPlayerPos = owner->GetPlayerPos();

	animator->ChangeAnimation(kBackAnimName, kAnimSpeed, kAnimBlendFrame, true);
	auto pTransform = owner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();
	auto pRigidbody = owner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();
	Vector3 playerPos = m_pPlayerPos.lock()->GetPos();
	Vector3 pos = pTransform->GetPos();
	Vector3 toPlayer = pos - playerPos;
	toPlayer.y = 0.0f;
	Quaternion dir = Quaternion::LookRotation(toPlayer, Vector3::Up());
	pTransform->SetRotation(dir);
	pRigidbody->SetVelocity(pTransform->GetDir() * kBackSpeed);
	pRigidbody->SetIsApplyDirection(false);

	m_backFrame = kBackFrame;
}

void MyLib::EnemyStateBack::OnUpdate()
{
	m_backFrame--;


	auto pTransform = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();
	Vector3 playerPos = m_pPlayerPos.lock()->GetPos();
	Vector3 pos = pTransform->GetPos();
	Vector3 toPlayer = pos - playerPos;
	toPlayer.y = 0.0f;
	Quaternion dir = Quaternion::LookRotation(toPlayer, Vector3::Up());
	pTransform->SetRotation(dir);

	if (m_backFrame <= 0)
	{
		auto effectComponent = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

		effectComponent->PlayEffect(L"enemyFocus.efk", kEffectOffsetRight);
		effectComponent->PlayEffect(L"enemyFocus.efk", kEffectOffsetLeft);
		SoundManager::GetInstance().Play("Rush", 1.0f, true);
		m_pStateMachine->ChangeState<EnemyStateRush>();
		return;
	}
}

void MyLib::EnemyStateBack::OnEnd()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();
	pRigidbody->SetVelocity(Vector3::Zero());
	pRigidbody->SetIsApplyDirection(true);
}
