#include "EnemyStateRush.h"
#include "EnemyStateSearch.h"
#include "EnemyStateAttack.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Rigidbody.h"
#include "../../../MyLib/GameObject.h"
#include <string>
#include <cassert>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kRushAnimName = L"Enemy|Walk";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.65f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// 突進速度
	constexpr float kRushSpeed = 0.15f;

	// 攻撃する距離
	constexpr float kAttackRange = 1.5f;

	// 突進を行う最大フレーム数
	constexpr int kRushFrame = 60;
}

MyLib::EnemyStateRush::EnemyStateRush() : 
	m_rushFrame(0)
{
}

MyLib::EnemyStateRush::~EnemyStateRush()
{
}

void MyLib::EnemyStateRush::OnInit(EnemyController* owner)
{
	auto animator = owner->GetAnimator().lock();

	m_pOwner = owner;

	m_pPlayerPos = owner->GetPlayerPos();

	animator->ChangeAnimation(kRushAnimName, kAnimSpeed, kAnimBlendFrame, true);

	m_rushFrame = kRushFrame;

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

	pRigidbody->SetVelocity(-moveDir * kRushSpeed);

	m_rushFrame = kRushFrame;
}

void MyLib::EnemyStateRush::OnUpdate()
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

	m_rushFrame--;

	if (m_rushFrame <= 0)
	{
		// 探すステートに遷移
		m_pStateMachine->ChangeState<EnemyStateSearch>();
		return;
	}

	if (toPlayer.Length() <= kAttackRange)
	{
		// 攻撃を行う
		m_pStateMachine->ChangeState<EnemyStateAttack>();
		return;
	}
}

void MyLib::EnemyStateRush::OnEnd()
{
	auto pRigidbody = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();
	pRigidbody->SetVelocity(Vector3::Zero());
}
