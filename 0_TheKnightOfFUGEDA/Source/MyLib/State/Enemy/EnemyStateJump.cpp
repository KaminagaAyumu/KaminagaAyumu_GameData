#include "EnemyStateJump.h"
#include "EnemyStateAttack.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/GameObject.h"
#include <string>
#include <cassert>

namespace
{
	// アニメーション名(あるか不明)
	constexpr const std::wstring_view kJumpAnimName = L"Enemy|Jump";

	// 攻撃する距離
	constexpr float kAttackRange = 1.5f;

	// 移動するスピード
	constexpr float kJumpSpeed = 0.1f;

	// ジャンプの高さ(ジャンプ開始時の高さからの差)
	constexpr float kJumpHeight = 2.0f;

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::EnemyStateJump::EnemyStateJump() :
	m_flow(JumpFlow::Default),
	m_defaultY(0.0f)
{
}

MyLib::EnemyStateJump::~EnemyStateJump()
{
}

void MyLib::EnemyStateJump::OnInit(EnemyController* owner)
{
	auto animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kJumpAnimName, kAnimSpeed, kAnimBlendFrame, true);

	m_targetPos = owner->GetPlayerPos().lock()->GetPos();

	m_defaultY = owner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock()->GetPos().y;
}

void MyLib::EnemyStateJump::OnUpdate()
{
	auto pTransform = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

	if (!pTransform)
	{
		assert(false && "EnemyStateJump : 自身のTransformの取得に失敗しました");
	}

	if (!pRigidbody)
	{
		assert(false && "EnemyStateJump : 敵のRigidbodyの取得に失敗しました");
	}

	pRigidbody->SetIsApplyDirection(false);
	pRigidbody->SetBodyType(BodyType::Dynamic);

	Vector3 toTarget = pTransform->GetPos() - m_targetPos;
	Vector3 moveDir = toTarget.Normalized();
	moveDir.y = 1.0f;
	toTarget.y = 0.0f;

	switch (m_flow)
	{
	case MyLib::EnemyStateJump::JumpFlow::Default:
		m_flow = JumpFlow::JumpStart;
		break;
	case MyLib::EnemyStateJump::JumpFlow::JumpStart:

		pRigidbody->SetVelocity(Vector3::Up() * kJumpSpeed);

		if (pTransform->GetPos().y >= m_defaultY + kJumpHeight)
		{
			m_flow = JumpFlow::Falling;
		}
		break;
	case MyLib::EnemyStateJump::JumpFlow::Falling:

		pRigidbody->SetVelocity(Vector3::Down() * kJumpSpeed);

		if (pTransform->GetPos().y <= m_defaultY)
		{
			// 攻撃のステートに遷移
			m_pStateMachine->ChangeState<EnemyStateAttack>();
			return;
		}

		break;
	default:
		break;
	}
}

void MyLib::EnemyStateJump::OnEnd()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();
	pRigidbody->SetIsApplyDirection(true);
	pRigidbody->SetBodyType(BodyType::Static);
}
