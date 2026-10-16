#include "BulletEnemyController.h"
#include "../../../State/Enemy/EnemyStateIdle.h"
#include "../../../State/Enemy/EnemyStateFreeze.h"
#include "../../../State/Enemy/EnemyStateSearch.h"
#include "../../../State/Enemy/EnemyStateDetect.h"
#include "../../../State/Enemy/EnemyStateDamage.h"
#include "../../../State/Enemy/EnemyStateDead.h"
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
	constexpr float kDetectRadius = 15.0f;
}

MyLib::BulletEnemyController::BulletEnemyController() : 
	m_hp(kMaxHitPoint)
{
}

MyLib::BulletEnemyController::~BulletEnemyController()
{
}

void MyLib::BulletEnemyController::Init(std::weak_ptr<MyLib::GameObject> parent)
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
	std::shared_ptr<File> modelFile = fileManager.GetModel(L"Data/File/Model/enemy_bullet.mv1", false);

	pRigidbody->SetBodyType(MyLib::BodyType::Static);

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
	soundManager.LoadSoundClip("Shoot", L"Data/File/Sound/SE/enemy_shoot.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Detect", L"Data/File/Sound/SE/enemy_detect.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Lost", L"Data/File/Sound/SE/enemy_lost.mp3", SoundBus::SE, 1.0f, false);

	pCollidable->AddCollider(std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Enemy, kBodyColRadius, false, "EnemyBody"));
	pCollidable->AddCollider(std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Enemy, kDetectRadius, true, "EnemyDetect"));
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
							m_stateMachine.ChangeState<MyLib::EnemyStateDetect>();
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
						!m_stateMachine.IsCheckState<MyLib::EnemyStateFreeze>())
					{
						m_stateMachine.ChangeState<MyLib::EnemyStateDetect>();
						return;
					}
				}
			}
			if (info.myCollider->GetName() == "EnemyBody")
			{
				if (info.otherCollider->GetName() == "ReflectBullet")
				{
					m_hp = 0;
					return;
				}
				if (info.otherCollider->GetName() == "PlayerAttack")
				{
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>())
					{
						m_stateMachine.ChangeStateReset<MyLib::EnemyStateDamage>();
						m_hp--;
						soundManager.Play("Damage", 1.0f, true);
						return;
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
					if (!m_stateMachine.IsCheckState<MyLib::EnemyStateDead>())
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

	
}

void MyLib::BulletEnemyController::Start()
{
}

void MyLib::BulletEnemyController::Update()
{
	if (m_hp <= 0)
	{
		m_stateMachine.ChangeState<MyLib::EnemyStateDead>();
	}
	m_stateMachine.Update();

	if (m_stateMachine.IsCheckState<MyLib::EnemyStateDead>())
	{
		if (auto pOwner = m_pOwnerObj.lock())
		{
			if (std::shared_ptr<MyLib::Collidable> pCollidable = pOwner->GetComponent<MyLib::Collidable>().lock())
			{
				pCollidable->SetEnable(false);
			}
		}
	}
}

void MyLib::BulletEnemyController::End()
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

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Damage");
	soundManager.DeleteSoundClip("Dead");
	soundManager.DeleteSoundClip("Die");
	soundManager.DeleteSoundClip("Shoot");
	soundManager.DeleteSoundClip("Detect");
	soundManager.DeleteSoundClip("Lost");
}

