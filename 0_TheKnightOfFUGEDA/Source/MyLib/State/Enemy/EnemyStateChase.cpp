#include "EnemyStateChase.h"
#include "EnemyStateBack.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../MyLib/GameObject.h"
#include <string>
#include <cassert>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kDetectAnimName = L"Enemy|Walk";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// 移動するスピード
	constexpr float kMoveSpeed = 0.04f;

	// 攻撃する距離
	constexpr float kAttackRange = 1.5f;

	// 攻撃するまでのフレーム数
	constexpr int kAttackFrame = 180;

	// エフェクトが出るオフセット
	const Vector3 kEffectOffset = { 0.0f,1.0f,0.0f };
}

MyLib::EnemyStateChase::EnemyStateChase() : 
	m_attackFrame(0)
{
}

MyLib::EnemyStateChase::~EnemyStateChase()
{
}

void MyLib::EnemyStateChase::OnInit(EnemyController* owner)
{
	auto animator = owner->GetAnimator().lock();

	m_pOwner = owner;

	m_pPlayerPos = owner->GetPlayerPos();

	animator->ChangeAnimation(kDetectAnimName, kAnimSpeed, kAnimBlendFrame, true);

	auto effectComponent = owner->GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

	if (owner->TryNotifyDetect())
	{
		// プレイヤー発見時のエフェクトを生成
		auto effectComponent = owner->GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

		effectComponent->PlayEffect(L"detect.efk", kEffectOffset);

		SoundManager::GetInstance().Play("Detect", 1.0f, true);
	}

}

void MyLib::EnemyStateChase::OnUpdate()
{
	auto pTransform = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();
	auto pPlayerPos = m_pPlayerPos.lock();

	if (!pTransform)
	{
		assert(false && "EnemyStateChase : 自身のTransformの取得に失敗しました");
	}

	if (!pPlayerPos)
	{
		assert(false && "EnemyStateChase : プレイヤーのTransformの取得に失敗しました");
	}

	Vector3 playerPos = pPlayerPos->GetPos();
	Vector3 pos = pTransform->GetPos();

	Vector3 toPlayer = pos - playerPos;
	toPlayer.y = 0.0f;

	Quaternion dir = Quaternion::LookRotation(toPlayer, Vector3::Up());
	pTransform->SetRotation(dir);

	Vector3 moveDir = toPlayer.Normalized();
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

	if (!pRigidbody)
	{
		assert(false && "EnemyStateChase : 敵のRigidbodyの取得に失敗しました");
	}

	m_attackFrame++;

	if (m_attackFrame >= kAttackFrame)
	{
		// 攻撃のステートに遷移
		m_pStateMachine->ChangeState<EnemyStateBack>();
		return;
	}

	if (toPlayer.Length() <= kAttackRange)
	{
		pRigidbody->SetVelocity(Vector3::Zero());
	}
	else
	{
		pRigidbody->SetVelocity(-moveDir * kMoveSpeed);
	}
}

void MyLib::EnemyStateChase::OnEnd()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

	if (!pRigidbody)
	{
		assert(false && "EnemyStateChase : 敵のRigidbodyの取得に失敗しました");
	}

	pRigidbody->SetVelocity(Vector3::Zero());
}
