#include "SkullEnemyController.h"
#include "../../../State/Enemy/EnemyStateIdle.h"
#include "../../../State/Enemy/EnemyStateFreeze.h"
#include "../../../State/Enemy/EnemyStateSearch.h"
#include "../../../State/Enemy/EnemyStateChase.h"
#include "../../../State/Enemy/EnemyStateDamage.h"
#include "../../../State/Enemy/EnemyStateStun.h"
#include "../../../State/Enemy/EnemyStateDead.h"
#include "../../../State/Enemy/EnemyStateBack.h"
#include "../../../State/Enemy/EnemyStateRush.h"
#include "../../Draw/Drawable3D.h"
#include "../../Animator.h"
#include "../../Rigidbody.h"
#include "../../EffectComponent.h"
#include "../../Collision/Collidable.h"
#include "../../../Collider/SphereCollider.h"
#include "../../../../Utility/File/FileManager.h"
#include "../../../../Utility/File/File.h"
#include "../../../../Common/Effect/EffectManager.h"
#include "../../../../Common/Sound/SoundManager.h"
#include <cassert>

namespace
{
	// モデルのサイズ
	const Vector3 kModelScale = { 0.01f, 0.01f, 0.01f };

	const Vector3 kFirstPos = { 0.0f, 1.0f, 5.0f };

	// モデルの表示オフセット
	const Vector3 kModelOffset = { 0.0f, -1.0f, 0.0f };
	
	// 見失いエフェクトの表示オフセット
	const Vector3 kLostEffectOffset = { 0.0f, 1.0f, 0.0f };

	// HP(仮)
	constexpr int kMaxHitPoint = 3;

	// 敵のボディ当たり判定
	constexpr float kBodyColRadius = 0.75f;

	// プレイヤーを検知する半径
	constexpr float kDetectRadius = 8.0f;

	// 攻撃の当たり判定
	constexpr float kAttackRadius = 0.5f;
	// 攻撃の当たり判定のオフセット
	const Vector3 kAttackColOffset = { 0.0f, 0.0f, -1.0f };

	// 攻撃を受けた際のアニメーション名
	constexpr const std::wstring_view kHitAnimName = L"Enemy|Hit";

	// スタン状態のアニメーション名
	constexpr const std::wstring_view kStunAnimName = L"Enemy|Dance";

	// アニメーションの速度(割合)
	constexpr float kAnimSpeed = 0.5f;
	// アニメーションのブレンドフレーム数
	constexpr float kAnimBlendFrame = 5.0f;

	constexpr float kKnockBackSpeed = 0.075f;
}

MyLib::SkullEnemyController::SkullEnemyController() :
	m_hp(kMaxHitPoint),
	m_isStunned(false)
{
}

MyLib::SkullEnemyController::~SkullEnemyController()
{
}

void MyLib::SkullEnemyController::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	m_pOwnerObj = parent;

	m_stateMachine.Init(this);

	// 最初の状態を待機状態にする
	m_stateMachine.ChangeState<MyLib::EnemyStateIdle>();

	// 親オブジェクトを取得
	std::shared_ptr<MyLib::GameObject> pParent = m_pOwnerObj.lock();

	std::shared_ptr<MyLib::Collidable> pCollidable = pParent->GetComponent<MyLib::Collidable>().lock();
	std::shared_ptr<MyLib::Transform> pTransform = pParent->GetComponent<MyLib::Transform>().lock();	// Transformコンポーネントを取得
	std::shared_ptr<MyLib::Drawable3D> pDrawable = pParent->GetComponent<MyLib::Drawable3D>().lock();	// Drawable3Dコンポーネントを取得
	std::shared_ptr<MyLib::Rigidbody> pRigidbody = pParent->GetComponent<MyLib::Rigidbody>().lock();	// Rigidbodyコンポーネントを取得

	FileManager& fileManager = FileManager::GetInstance();
	std::shared_ptr<File> modelFile = fileManager.GetModel(L"Data/File/Model/enemy_skull.mv1", false);

	pRigidbody->SetBodyType(MyLib::BodyType::Dynamic);

	// モデルをセットする
	pDrawable->AddModel(Model::ModelSlot::Main, modelFile->GetHandle());

	m_pModelOffset = std::make_shared<MyLib::Transform>();
	m_pModelOffset->SetPos(kModelOffset);

	pDrawable->SetModelOffset(Model::ModelSlot::Main, m_pModelOffset);

	m_pAnimator = pParent->GetComponent<MyLib::Animator>();	// Animatorコンポーネントの弱参照を取得


	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("Damage", L"Data/File/Sound/SE/slash.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Dead", L"Data/File/Sound/SE/enemy_dead.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Die", L"Data/File/Sound/SE/enemy_die.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Bite", L"Data/File/Sound/SE/enemy_bite.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Rush", L"Data/File/Sound/SE/enemy_rush.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("JustGuard", L"Data/File/Sound/SE/just_guard.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Detect", L"Data/File/Sound/SE/enemy_detect.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Lost", L"Data/File/Sound/SE/enemy_lost.mp3", SoundBus::SE, 1.0f, false);

	// 敵のボディの判定を作成
	auto bodyCol = std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Enemy, kBodyColRadius, false, "EnemyBody");

	bodyCol->SetContext([&]() {
		MyLib::ColliderBase::HitContext context;
		context.ownerObj = m_pOwnerObj;
		context.isStunned = m_stateMachine.IsCheckState<MyLib::EnemyStateStun>();
		return context;
	});

	pCollidable->AddCollider(bodyCol);
	pCollidable->AddCollider(std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Enemy, kDetectRadius, true, "EnemyDetect"));

	auto attackCol = std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Enemy, kAttackRadius, true, "EnemyAttack");
	attackCol->SetLocalOffset(kAttackColOffset);
	attackCol->SetEnable(false);
	m_pAttackCollider = attackCol;
	pCollidable->AddCollider(attackCol);
	
	//pCollidable->AddCollider(std::make_shared<MyLib::CapsuleCollider>(1.0f, 2.0f, Vector3::Zero(), false));
	pCollidable->SetOnCollide([&](const MyLib::CollisionInfo& info)
		{
			if (info.myCollider->GetName() == "EnemyDetect")
			{
				if (info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::Player && info.otherCollider->GetName() != "PlayerAttack")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>() &&
						!m_stateMachine.IsCheckState<MyLib::EnemyStateFreeze>())
					{
						if (m_stateMachine.IsCheckState<MyLib::EnemyStateSearch>())
						{
							m_stateMachine.ChangeState<MyLib::EnemyStateChase>();
							return;
						}
					}
				}
			}
		});

	pCollidable->SetOnCollideEnter([&](const MyLib::CollisionInfo& info)
		{
			if (info.myCollider->GetName() == "EnemyDetect")
			{
				if (info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::Player && info.otherCollider->GetName() != "PlayerAttack")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>() &&
						!m_stateMachine.IsCheckState<MyLib::EnemyStateFreeze>() &&
						!m_stateMachine.IsCheckState<MyLib::EnemyStateBack>() &&
						!m_stateMachine.IsCheckState<MyLib::EnemyStateRush>() && 
						!m_stateMachine.IsCheckState<MyLib::EnemyStateStun>())
					{
						m_stateMachine.ChangeState<MyLib::EnemyStateChase>();
						return;
					}
				}
			}
			if (info.myCollider->GetName() == "EnemyBody")
			{
				if (info.otherCollider->GetName() == "PlayerAttack")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>())
					{
						if (m_stateMachine.IsCheckState<MyLib::EnemyStateStun>())
						{
							m_isStunned = true;
							m_pAnimator.lock()->ChangeAnimation(kHitAnimName, kAnimSpeed, kAnimBlendFrame, false);
							m_hp--;

							auto pTransform = GetOwnerObj().lock()->GetComponent<MyLib::Transform>().lock();

							Vector3 knockBackDir = -pTransform->GetDir();
							knockBackDir.Normalize();

							auto pRigidbody = GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

							pRigidbody->SetVelocity(knockBackDir * kKnockBackSpeed);
							pRigidbody->SetIsApplyDirection(false);

							auto effectComponent = GetOwnerObj().lock()->GetComponent<MyLib::EffectComponent>().lock();

							effectComponent->PlayEffect(L"hit_2.efk");
						}
						else
						{
							m_stateMachine.ChangeStateReset<MyLib::EnemyStateDamage>();
							m_hp--;
						}
						soundManager.Play("Damage", 1.0f, true);
						return;
					}
				}

				if (info.otherCollider->GetName() == "PlayerGuard")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>() && !m_stateMachine.IsCheckState<MyLib::EnemyStateStun>())
					{
						auto context = info.otherCollider->GetHitContext();
						if (context.isJustGuard)
						{
						}
						else
						{
							m_stateMachine.ChangeStateReset<MyLib::EnemyStateDamage>();
							return;
						}
					}
				}
			}
			if (info.myCollider->GetName() == "EnemyAttack")
			{
				if (info.otherCollider->GetName() == "PlayerGuard")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>())
					{
						auto context = info.otherCollider->GetHitContext();
						if (context.isJustGuard)
						{
							SoundManager::GetInstance().Play("JustGuard", 1.0f, true);
							m_stateMachine.ChangeState<MyLib::EnemyStateStun>();
							return;
						}
					}
				}
			}
		});

	pCollidable->SetOnCollideExit([&](const MyLib::CollisionInfo& info)
		{
			if (info.myCollider->GetName() == "EnemyDetect")
			{
				if (info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::Player && info.otherCollider->GetName() != "PlayerAttack")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>()
						&& !m_stateMachine.IsCheckState<MyLib::EnemyStateBack>() 
						&& !m_stateMachine.IsCheckState<MyLib::EnemyStateRush>()
						&& !m_stateMachine.IsCheckState<MyLib::EnemyStateStun>())
					{
						if (ResetDetectNotify())
						{
							auto effectComponent = m_pOwnerObj.lock()->GetComponent<MyLib::EffectComponent>().lock();

							effectComponent->PlayEffect(L"lost.efk", kLostEffectOffset);

							SoundManager::GetInstance().Play("Lost", 1.0f, true);
						}

						m_stateMachine.ChangeState<MyLib::EnemyStateSearch>();
						return;
					}
				}
			}
		});

	// モデルのスケールを設定する
	pTransform->SetScale(kModelScale);

	// モデルの初期位置を設定する
	pTransform->SetPos(kFirstPos);

	// エフェクトを登録
	auto& effectManager = EffectManager::GetInstance();
	effectManager.LoadEffect(L"detect.efk");
	effectManager.LoadEffect(L"lost.efk");
	effectManager.LoadEffect(L"hit.efk");
	effectManager.LoadEffect(L"hit_2.efk");
	effectManager.LoadEffect(L"enemyDie.efk");
	effectManager.LoadEffect(L"enemyDead.efk");
	effectManager.LoadEffect(L"enemyBite.efk");
	effectManager.LoadEffect(L"enemyFocus.efk");

}

void MyLib::SkullEnemyController::Start()
{
}

void MyLib::SkullEnemyController::Update()
{
	if (m_hp <= 0)
	{
		m_stateMachine.ChangeState<MyLib::EnemyStateDead>();
	}

	if(m_stateMachine.IsCheckState<MyLib::EnemyStateStun>() && m_isStunned)
	{
		if(auto pAnimator = m_pAnimator.lock())
		{
			if(pAnimator->GetAnimEnd())
			{
				m_isStunned = false;
				pAnimator->ChangeAnimation(kStunAnimName, kAnimSpeed, kAnimBlendFrame, true);
				auto pRigidbody = GetOwnerObj().lock()->GetComponent<MyLib::Rigidbody>().lock();

				pRigidbody->SetVelocity(Vector3::Zero());
				pRigidbody->SetIsApplyDirection(true);
			}
		}
	}

	m_stateMachine.Update();
}

void MyLib::SkullEnemyController::End()
{
	m_stateMachine.End();

	// エフェクトを削除
	auto& effectManager = EffectManager::GetInstance();
	effectManager.DeleteEffect(L"detect.efk");
	effectManager.DeleteEffect(L"lost.efk");
	effectManager.DeleteEffect(L"hit.efk");
	effectManager.DeleteEffect(L"hit_2.efk");
	effectManager.DeleteEffect(L"enemyDie.efk");
	effectManager.DeleteEffect(L"enemyDead.efk");
	effectManager.DeleteEffect(L"enemyBite.efk");
	effectManager.DeleteEffect(L"enemyFocus.efk");

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Damage");
	soundManager.DeleteSoundClip("Dead");
	soundManager.DeleteSoundClip("Die");
	soundManager.DeleteSoundClip("Bite");
	soundManager.DeleteSoundClip("Rush");
	soundManager.DeleteSoundClip("JustGuard");
	soundManager.DeleteSoundClip("Detect");
}