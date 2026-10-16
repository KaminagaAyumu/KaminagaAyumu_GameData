#include "EnemyStateDetect.h"
#include "EnemyStateShot.h"
#include "../../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include "../../../MyLib/Component/Transform.h"
#include "../../../MyLib/Component/Animator.h"
#include "../../../MyLib/Component/EffectComponent.h"
#include "../../../MyLib/GameObject.h"
#include "../../../MyLib/Renderer.h"
#include "../../../Common/Sound/SoundManager.h"
#include <string>
#include <cassert>

namespace
{
	// アニメーション名
	constexpr const std::wstring_view kDetectAnimName = L"Enemy|Detect";

	// 攻撃するまでのフレーム数
	constexpr int kAttackFrame = 120;

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	// エフェクトが出るオフセット
	const Vector3 kEffectOffset = { 0.0f,1.0f,0.0f };
}

MyLib::EnemyStateDetect::EnemyStateDetect() : 
	m_attackFrame(kAttackFrame)
{
}

MyLib::EnemyStateDetect::~EnemyStateDetect()
{
}

void MyLib::EnemyStateDetect::OnInit(EnemyController* owner)
{
	auto animator = owner->GetAnimator().lock();

	m_pOwner = owner;

	m_pPlayerPos = owner->GetPlayerPos();

	animator->ChangeAnimation(kDetectAnimName, kAnimSpeed, kAnimBlendFrame, true);

	m_attackFrame = kAttackFrame;


	if (owner->TryNotifyDetect())
	{
		// プレイヤー発見時のエフェクトを生成
		auto effectComponent = owner->GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

		effectComponent->PlayEffect(L"detect.efk", kEffectOffset);

		SoundManager::GetInstance().Play("Detect", 1.0f, true);
	}
}

void MyLib::EnemyStateDetect::OnUpdate()
{

	m_attackFrame--;

	std::shared_ptr<MyLib::Transform> pTransform = m_pOwner->GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();

	if (std::shared_ptr<MyLib::Transform> pPlayerPos = m_pPlayerPos.lock())
	{
		Vector3 playerPos = pPlayerPos->GetPos();
		Vector3 pos = pTransform->GetPos();
		
		Vector3 toPlayer = pos - playerPos;

		Quaternion dir = Quaternion::LookRotation(toPlayer, Vector3::Up());
		//dir.DrawLocalAxis(pos, dir, 100.0f);
		pTransform->SetRotation(dir);
	}
	else
	{
		assert(false && "EnemyStateDetect : プレイヤーのTransformの取得に失敗しました");
	}

	if (m_attackFrame < 0)
	{
		m_pStateMachine->ChangeState<MyLib::EnemyStateShot>();
		return;
	}
}

void MyLib::EnemyStateDetect::OnEnd()
{
}
