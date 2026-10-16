#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Utility/Input.h"
#include <cmath>

namespace
{
	// ダッシュの状態になるフレーム数
	constexpr int kDashDuration = 20;

	constexpr float kMaxDashSpeed = 0.25f;
	constexpr float kMinDashSpeed = 0.1f;

	// アニメーション名
	constexpr const std::wstring_view kDashAnimName = L"Player|Rolling";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::PlayerStateDash::PlayerStateDash() : 
	m_dashFrame(kDashDuration),
	m_dashSpeed(kMinDashSpeed)
{
}

void MyLib::PlayerStateDash::OnInit(PlayerController* owner)
{
	SoundManager::GetInstance().Play("Rolling", 1.0f, true);

	std::shared_ptr<MyLib::Transform> pTransform = owner->GetTransform().lock();

	std::shared_ptr<MyLib::Rigidbody> pRigidbody = owner->GetRigidbody().lock();

	Input& input = Input::GetInstance();
	// スティックの入力があれば移動する
	Input::XInputData stickData = input.GetXInputData();
	if (stickData.leftStick.Length() != 0.0f)
	{
		// コントローラーの入力を取得して移動できるようにする
		VECTOR move = GetCameraFrontVector();

		Vector3 moveDir = { move.x, 0.0f, move.z };
		// Y方向を初期化
		moveDir.y = 0;

		// 正規化して正面方向を出す
		Vector3 forward = moveDir.Normalized();
		// 外積を使ってmoveDirを正面とした右方向のベクトルを出す
		Vector3 right = Vector3::Cross(Vector3::Up(), forward);
		// 右方向のベクトルを正規化
		right.Normalize();

		// 正面入力の作成
		Vector3 inputForward = forward * stickData.leftStick.y;

		// 左右入力の作成
		Vector3 inputSide = right * stickData.leftStick.x;

		// 入力の合成
		Vector3 vel = inputForward + inputSide;

		m_moveDir = vel;
	}
	else
	{
		m_moveDir = pTransform->GetDir();
	}

	pRigidbody->SetVelocity(m_moveDir * kMinDashSpeed);

	std::shared_ptr<MyLib::Animator> animator = owner->GetAnimator().lock();

	animator->ChangeAnimation(kDashAnimName, kAnimSpeed, kAnimBlendFrame, false);

	std::shared_ptr<MyLib::EffectComponent> pEffect = m_pOwner->GetEffectComponent().lock();
	pEffect->PlayEffect(L"rolling.efk", Vector3::Zero(), pTransform->GetRotation());

	// フレームカウンタと速度を初期状態にする
	m_dashFrame = kDashDuration;
	m_dashSpeed = kMinDashSpeed;
}

void MyLib::PlayerStateDash::OnUpdate()
{
	m_dashFrame--;

	std::shared_ptr<MyLib::Transform> pTransform = m_pOwner->GetTransform().lock();

	std::shared_ptr<MyLib::Rigidbody> pRigidbody = m_pOwner->GetRigidbody().lock();

	float dashRate = static_cast<float>(m_dashFrame) / kDashDuration;


	//m_dashSpeed = std::lerp(kMaxDashSpeed, kMinDashSpeed, dashRate);
	//m_dashSpeed = std::lerp(0.0f, kMaxDashSpeed, dashRate);
	m_dashSpeed = std::lerp(kMaxDashSpeed, 0.0f, dashRate);
	//m_dashSpeed = dashRate * kMaxDashSpeed;


	pRigidbody->SetVelocity(m_moveDir * m_dashSpeed);

	// ダッシュ状態を解除するフレームになったら通常状態に戻る
	if (m_dashFrame < 0)
	{
		std::shared_ptr<MyLib::Rigidbody> pRigidbody = m_pOwner->GetRigidbody().lock();

		pRigidbody->SetVelocity(Vector3::Zero());

		m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
		return;
	}
}

void MyLib::PlayerStateDash::OnEnd()
{
}

