#include "PlayerStateRun.h"
#include "PlayerStateIdle.h"
#include "PlayerStateWalk.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDash.h"
#include "PlayerStateGuard.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Collider/SphereCollider.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../Utility/Input.h"
#include "../../../Common/Sound/SoundManager.h"
#include "DxLib.h"

namespace
{
	// 動く速度
	constexpr float kMoveSpeed = 0.09f;

	// スティックの入力の閾値
	constexpr float kStickThreshold = 0.0001f;

	// 歩きモーションを行うスティックの入力の閾値
	constexpr float kStickWalkThreShold = 0.04f;

	// アニメーション名
	constexpr const std::wstring_view kWalkAnimName = L"Player|Walk";
	constexpr const std::wstring_view kRunAnimName = L"Player|Run";
	constexpr const std::wstring_view kGuardLeftAnimName = L"Player|Sield_walk_left";
	constexpr const std::wstring_view kGuardRightAnimName = L"Player|Sield_walk_right";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;

	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// エフェクトが出るフレーム数
	constexpr int kParticleFrame = 10;

	const Vector3 kEffectOffset = { 0.0f,-1.0f,0.0f };
}

MyLib::PlayerStateRun::PlayerStateRun() :
	m_particleFrame(0)
{
}

void MyLib::PlayerStateRun::OnInit(PlayerController* owner)
{
	m_pAnimator = owner->GetAnimator();

	auto animator = m_pAnimator.lock();


	m_pGuardCol = m_pOwner->GetGuardCollider();

	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	if (input.IsPressed("Guard"))
	{
		// ガードの当たり判定を有効にする
		auto pGuardCol = m_pGuardCol.lock();
		pGuardCol->SetEnable(true);

		auto stick = input.GetXInputData();
		if (stick.leftStick.x > 0.0f)
		{
			animator->ChangeAnimation(kGuardLeftAnimName, kAnimSpeed, kAnimBlendFrame, true);
		}
		else
		{
			animator->ChangeAnimation(kGuardRightAnimName, kAnimSpeed, kAnimBlendFrame, true);
		}
	}
	else
	{
		animator->ChangeAnimation(kRunAnimName, kAnimSpeed, kAnimBlendFrame, true);
	}

	SoundManager::GetInstance().Play("Run", 1.0f, true);
}

void MyLib::PlayerStateRun::OnUpdate()
{

	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	// XInputの入力データを取得
	Input::XInputData stickData = input.GetXInputData();

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
	Vector3 inputForward = forward * stickData.leftStick.y * kMoveSpeed;

	// 左右入力の作成
	Vector3 inputSide = right * stickData.leftStick.x * kMoveSpeed;

	// 入力の合成
	Vector3 vel = inputForward + inputSide;

	//if (input.IsPressed("XButton"))
	//{
	//	vel.y -= kMoveSpeed;
	//}
	//if (input.IsPressed("YButton"))
	//{
	//	vel.y += kMoveSpeed;
	//}

	auto pRigid = m_pOwner->GetRigidbody().lock();

	pRigid->SetVelocity(vel);

	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	std::shared_ptr<MyLib::EffectComponent> pEffect = m_pOwner->GetEffectComponent().lock();

	m_particleFrame++;

	if(m_particleFrame >= kParticleFrame)
	{
		if(pEffect)
		{
			pEffect->PlayOneShotEffect(L"footSmoke.efk", kEffectOffset);
		}
		m_particleFrame = 0;
	}

	if (input.IsTriggered("Guard"))
	{
		// ガードの当たり判定を有効にする
		auto pGuardCol = m_pGuardCol.lock();
		pGuardCol->SetEnable(true);

		auto animator = m_pAnimator.lock();

		if (stickData.leftStick.x > 0.0f)
		{
			animator->ChangeAnimation(kGuardLeftAnimName, kAnimSpeed, kAnimBlendFrame, true);
		}
		else
		{
			animator->ChangeAnimation(kGuardRightAnimName, kAnimSpeed, kAnimBlendFrame, true);
		}
	}
	else if (input.IsReleased("Guard"))
	{
		// ガードの当たり判定を無効にする
		auto pGuardCol = m_pGuardCol.lock();
		pGuardCol->SetEnable(false);

		auto animator = m_pAnimator.lock();

		animator->ChangeAnimation(kRunAnimName, kAnimSpeed, kAnimBlendFrame, true);
	}

	

	if (vel.Length() <= kStickThreshold)
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
		return;
	}
	else if (vel.Length() <= kStickWalkThreShold)
	{
		if (pAnimator)
		{
			m_pStateMachine->ChangeState<MyLib::PlayerStateWalk>();
			return;
		}
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

void MyLib::PlayerStateRun::OnEnd()
{
	// ガードの当たり判定を無効にする
	auto pGuardCol = m_pGuardCol.lock();
	pGuardCol->SetEnable(false);

	SoundManager::GetInstance().Stop("Run");
}