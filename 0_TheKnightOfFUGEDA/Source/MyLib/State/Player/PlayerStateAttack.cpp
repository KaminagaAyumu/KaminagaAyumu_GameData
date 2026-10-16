#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "PlayerStateWalk.h"
#include "PlayerStateDash.h"
#include "PlayerStateGuard.h"
#include "../../../Utility/Input.h"
#include "../../../Utility/CSV/AttackData.h"
#include "../../../MyLib/Component/Controller/Player/PlayerController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Draw/Drawable3D.h"
#include "../../../MyLib/Component/Collision/Collidable.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Collider/SphereCollider.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../Common/Model.h"
#include "../../../Common/Effect/Effect.h"
#include <algorithm>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kFirstAttackAnimName = L"Player|Attack_1";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	constexpr float kMoveSpeed = 0.06f;
}

MyLib::PlayerStateAttack::PlayerStateAttack() : 
	m_frameCount(0),
	m_isNextAttack(false),
	m_currentAttackData(nullptr)
{
}

void MyLib::PlayerStateAttack::OnInit(PlayerController* owner)
{
	m_pAnimator = owner->GetAnimator();

	m_pAttackCol = owner->GetAttackCollider();

	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	pAnimator->ChangeAnimation(kFirstAttackAnimName, kAnimSpeed, kAnimBlendFrame, false);

	m_attackResource = owner->GetAttackResource();
	m_frameCount = 0;
	m_currentAttackData = m_attackResource.GetAttackData(AttackData::AttackID::Attack1);
	m_isNextAttack = false;

	UpdateMoveVelocity();

	PlayAttackEffect();
}

void MyLib::PlayerStateAttack::OnUpdate()
{
	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	// XInputの入力データを取得
	Input::XInputData stickData = input.GetXInputData();

	std::shared_ptr<MyLib::Animator> pAnimator = m_pAnimator.lock();

	m_frameCount++;

	UpdateMoveVelocity();

	std::shared_ptr<MyLib::SphereCollider> pAttackCollider = m_pAttackCol.lock();
	// 現在のフレームが攻撃の発生フレームを超えており、持続フレーム以内ならば攻撃判定を表示する
	pAttackCollider->SetEnable(m_currentAttackData->IsStart(m_frameCount) && m_currentAttackData->IsActive(m_frameCount));

	// 攻撃アニメーションをキャンセルできるフレーム内ならば入力されていたステートに移行する
	if (m_currentAttackData->IsCanselable(m_frameCount))
	{
		// ガードのステートに移行
		if (m_pOwner->IsBuffered("Guard"))
		{
			m_pOwner->ClearBuffer("Guard");
			pAttackCollider->SetEnable(false);
			m_pStateMachine->ChangeState<MyLib::PlayerStateGuard>();
			return;
		}

		// ローリングのステートに移行
		if (m_pOwner->IsBuffered("BButton"))
		{
			m_pOwner->ClearBuffer("BButton");
			pAttackCollider->SetEnable(false);
			m_pStateMachine->ChangeState<MyLib::PlayerStateDash>();
			return;
		}

		// 次の攻撃に移行
		if (m_pOwner->IsBuffered("AButton"))
		{
			if (m_currentAttackData->GetNextAttackID() != AttackData::AttackID::None)
			{
				pAnimator->ChangeAnimation(m_currentAttackData->GetNextAnimName(), kAnimSpeed, kAnimBlendFrame, false);
				m_currentAttackData = m_attackResource.GetAttackData(m_currentAttackData->GetNextAttackID());
				m_frameCount = 0;
				m_isNextAttack = false;
				UpdateMoveVelocity();
				return;
			}
			
			// ここに来るのはコンボの最終段のみのため入力をリセットする
			m_pOwner->ClearBuffer("AButton");
		}

	}

	// 攻撃の入力があった場合
	if (input.IsTriggered("AButton"))
	{
		// コンボが繋がるフレーム内ならば次の入力に移行するフラグを立てる
		if (m_currentAttackData->IsCombo(m_frameCount))
		{
			m_pOwner->ClearBuffer("AButton");
			m_isNextAttack = true;
		}
	}

	// 次の攻撃に進められる場合、アニメーションを変更するフレームを超えていたら次の攻撃を行う
	if (m_isNextAttack && pAnimator->GetCurrentAnimFrame() >= m_currentAttackData->GetAnimChangeFrame())
	{
		// 次の攻撃のデータがない場合アイドル状態に遷移する
		if (m_currentAttackData->GetNextAttackID() == AttackData::AttackID::None)
		{
			m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
			return;
		}

		pAnimator->ChangeAnimation(m_currentAttackData->GetNextAnimName(), kAnimSpeed, kAnimBlendFrame, false);
		m_currentAttackData = m_attackResource.GetAttackData(m_currentAttackData->GetNextAttackID());
		m_frameCount = 0;
		m_isNextAttack = false;
		UpdateMoveVelocity();
		return;
	}

	if (pAnimator->GetAnimEnd())
	{
		m_pStateMachine->ChangeState<MyLib::PlayerStateIdle>();
		pAttackCollider->SetEnable(false);
		return;
	}
}

void MyLib::PlayerStateAttack::OnEnd()
{
	auto pRigid = m_pOwner->GetRigidbody().lock();

	pRigid->SetVelocity(Vector3::Zero());

	if(auto effect = m_pAttackEffect.lock())
	{
		effect->StopEffect();
	}
}

void MyLib::PlayerStateAttack::PlayAttackEffect()
{
	auto drawable3D = m_pOwner->GetDrawable3D().lock();

	std::shared_ptr<Model> swordModel = drawable3D->GetModel(Model::ModelSlot::Weapon);

	auto effectComponent = m_pOwner->GetEffectComponent().lock();

	m_pAttackEffect = effectComponent->PlayEffectWithAnchor(L"slash.efk", [swordModel]() {

		return Matrix4x4::ToMatrix(MV1GetFrameLocalWorldMatrix(swordModel->GetModelHandle(), 0));
	}, Vector3::Zero(), Quaternion::Identity());
}

Vector3 MyLib::PlayerStateAttack::GetAttackDir()
{
	// 入力管理クラスのインスタンスを取得
	Input& input = Input::GetInstance();
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
		Vector3 dir = inputForward + inputSide;

		// 最終的な向きを返す
		return dir;
	}
	else
	{
		return Vector3::Zero();
	}
}

void MyLib::PlayerStateAttack::UpdateMoveVelocity()
{
	Vector3 dir;
	if (m_pOwner->IsLockOn())
	{
		auto pTransform = m_pOwner->GetTransform().lock();

		dir = pTransform->GetDir();
	}
	else
	{
		dir = GetAttackDir();
	}

	float t = std::clamp(static_cast<float>(m_frameCount) / m_currentAttackData->GetMoveEndFrame(), 0.0f, 1.0f);

	float speed = m_currentAttackData->GetInitSpeed() * (1.0f - t) * (1.0f - t);

	auto pRigid = m_pOwner->GetRigidbody().lock();

	pRigid->SetVelocity(dir * speed);
}
