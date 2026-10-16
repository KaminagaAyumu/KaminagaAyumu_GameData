#include "PlayerStateGuard.h"
#include "PlayerStateIdle.h"
#include "PlayerStateWalk.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDash.h"
#include "../StateMachine.h"
#include "../../../Utility/Input.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Collider/SphereCollider.h"
#include <string>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kStayAnimName = L"Player|Sield_stay";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.75f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::PlayerStateGuard::PlayerStateGuard()
{
}

void MyLib::PlayerStateGuard::OnInit(PlayerController* owner)
{
	m_pGuardCol = owner->GetGuardCollider();

	// ガードの当たり判定を有効にする
	auto pGuardCol = m_pGuardCol.lock();
	pGuardCol->SetEnable(true);

	std::shared_ptr<MyLib::Animator> animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kStayAnimName, kAnimSpeed, kAnimBlendFrame, true);

	auto pRigidbody = m_pOwner->GetRigidbody().lock();

	pRigidbody->SetVelocity(Vector3::Zero());
}

void MyLib::PlayerStateGuard::OnUpdate()
{
	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	// XInputの入力データを取得
	Input::XInputData stickData = input.GetXInputData();

	if (!input.IsPressed("Guard"))
	{
		// ガードの当たり判定を無効にする
		auto pGuardCol = m_pGuardCol.lock();
		pGuardCol->SetEnable(false);

		m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
		return;
	}

	// スティックの入力がある場合は移動状態に遷移する
	if (stickData.leftStick.Length() != 0.0f)
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateWalk>();
		return;
	}

	if (input.IsTriggered("AButton"))
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateAttack>();
		return;
	}

	if (input.IsTriggered("BButton"))
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateDash>();
		return;
	}
}

void MyLib::PlayerStateGuard::OnEnd()
{
	// ガードの当たり判定を無効にする
	auto pGuardCol = m_pGuardCol.lock();
	pGuardCol->SetEnable(false);
}
