#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDash.h"
#include "PlayerStateGuard.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../Utility/Input.h"
#include "DxLib.h"

namespace
{
	// 動く速度
	constexpr float kMoveSpeed = 0.09f;

	// スティックの入力の閾値
	constexpr float kStickThreshold = 0.0001f;

	// 歩きモーションを行うスティックの入力の閾値
	constexpr float kStickWalkThreShold = 0.3f;

	// アニメーション名
	constexpr const std::wstring_view kWalkAnimName = L"Player|Walk";
	constexpr const std::wstring_view kRunAnimName = L"Player|Run";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;
}

MyLib::PlayerStateMove::PlayerStateMove()
{
}

void MyLib::PlayerStateMove::OnInit(PlayerController* owner)
{
	m_pAnimator = owner->GetAnimator();

	auto animator = m_pAnimator.lock();

	animator->ChangeAnimation(kWalkAnimName, kAnimSpeed, kAnimBlendFrame, true);
}

void MyLib::PlayerStateMove::OnUpdate()
{

	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	// XInputの入力データを取得
	Input::XInputData stickData = input.GetXInputData();

	// コントローラーの入力を取得して移動できるようにする
	VECTOR move = GetCameraFrontVector();

	Vector3 moveDir = {move.x, 0.0f, move.z};
	// Y方向を初期化
	moveDir.y = 0;

	// 正規化して正面方向を出す
	Vector3 forward = moveDir.Normalized();
	// 外積を使ってmoveDirを正面とした右方向のベクトルを出す
	Vector3 right = Vector3::Cross(Vector3::Up(), forward);
	// 右方向のベクトルを正規化
	right.Normalize();

	// 正面入力の作成
	Vector3 inputForward = forward * stickData.leftStick.y * kMoveSpeed;

	// 左右入力の作成
	Vector3 inputSide = right * stickData.leftStick.x * kMoveSpeed;

	// 入力の合成
	Vector3 vel = inputForward + inputSide;


	///////
	// 以下テスト
	///////
	auto pRigid = m_pOwner->GetRigidbody().lock();

	pRigid->SetVelocity(vel);

	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	if (vel.Length() <= kStickThreshold)
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
		return;
	}
	else if (vel.Length() >= kStickWalkThreShold)
	{
		if (pAnimator)
		{
			pAnimator->ChangeAnimation(kRunAnimName, kAnimSpeed, kAnimBlendFrame, true);
		}
	}
	else
	{
		if (pAnimator)
		{
			pAnimator->ChangeAnimation(kWalkAnimName, kAnimSpeed, kAnimBlendFrame, true);
		}
	}

	// ガードの入力があればガード状態に遷移する
	if (input.IsTriggered("Guard"))
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateGuard>();
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

void MyLib::PlayerStateMove::OnEnd()
{
}