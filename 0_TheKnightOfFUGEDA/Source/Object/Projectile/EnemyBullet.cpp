#include "EnemyBullet.h"
#include "../../MyLib/Component/Transform.h"
#include "../../MyLib/Component/Rigidbody.h"
#include "../../MyLib/Component/Collision/Collidable.h"
#include "../../MyLib/Component/Draw/Drawable3D.h"
#include "../../MyLib/Component/EffectComponent.h"
#include "../../MyLib/Collider/SphereCollider.h"
#include "../../Utility/File/FileManager.h"
#include "../../Utility/File/File.h"
#include "../../Common/Sound/SoundManager.h"
#include "../../Common/Effect/EffectManager.h"
#include "../../Common/Effect/Effect.h"

namespace
{
	// 弾モデルのスケール
	const Vector3 kBulletScale = { 0.002f,0.002f,0.002f };

	constexpr float kSpeed = 0.15f;	// 弾のスピード

	constexpr float kJustGuardSpeed = 0.5f; // ジャストガード時の弾の速度

	constexpr float kRadius = 0.1f;	// 弾の半径
}

EnemyBullet::EnemyBullet() : 
	GameObject(MyLib::GameObject::Type::Projectile)
{
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Init(const Position3& pos, const Vector3& target, std::weak_ptr<MyLib::GameObject> shooter)
{
	m_pShooter = shooter;

	AddComponent<MyLib::Rigidbody>();							// Rigidbodyコンポーネントを追加
	AddComponent<MyLib::Collidable>();	// Collidableコンポーネントを追加
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加
	AddComponent<MyLib::EffectComponent>();						// エフェクトコンポーネントを追加

	// 座標とRigidbodyのデータを取得する
	std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock();
	std::shared_ptr<MyLib::Rigidbody> pRigidbody = GetComponent<MyLib::Rigidbody>().lock();
	// 3D描画コンポーネントのデータを取得する
	std::shared_ptr<MyLib::Drawable3D> pDrawable3D = GetComponent<MyLib::Drawable3D>().lock();

	std::shared_ptr<MyLib::Collidable> pCollidable = GetComponent<MyLib::Collidable>().lock();
	pCollidable->AddCollider(std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::EnemyBullet, kRadius, true));
	auto collider = std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::EnemyBullet, kRadius, true, "ReflectBullet");
	collider->SetEnable(false);
	m_pReflectCollider = collider;
	pCollidable->AddCollider(collider);

	pCollidable->SetOnCollideEnter([this](const MyLib::CollisionInfo& info)
		{
			if (info.otherCollider->GetShape() == MyLib::ColliderBase::ColliderShape::GroundMesh ||
				info.otherCollider->GetShape() == MyLib::ColliderBase::ColliderShape::WallMesh ||
				info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::Player)
			{
				Destroy();
			}

			if (info.otherCollider->GetName() == "EnemyBody")
			{
				if (m_pReflectCollider->IsEnable())
				{
					Destroy();
				}
			}

			if (info.otherCollider->GetName() == "PlayerGuard")
			{
				if (info.myCollider->GetName() == "ReflectBullet")
				{
					return;
				}
				std::shared_ptr<MyLib::Rigidbody> pRigidbody = GetComponent<MyLib::Rigidbody>().lock();
				std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock();

				MyLib::ColliderBase::HitContext context = info.otherCollider->GetHitContext();
				Vector3 velocity = pRigidbody->GetVelocity();

				if (context.isJustGuard)
				{
					if (std::shared_ptr<MyLib::GameObject> pShooter = m_pShooter.lock())
					{
						std::shared_ptr<MyLib::Transform> pShooterTransform = pShooter->GetComponent<MyLib::Transform>().lock();
						Vector3 dir = pShooterTransform->GetPos() - pTransform->GetPos();
						dir.Normalize();
						pRigidbody->SetVelocity(dir * kJustGuardSpeed);
						auto& soundManager = SoundManager::GetInstance();
						soundManager.Play("JustGuard", 1.0f, true);
					}
					else
					{
						auto& soundManager = SoundManager::GetInstance();
						soundManager.Play("Reflect", 1.0f, true);
						pRigidbody->SetVelocity(velocity - info.hitNormal * (2.0f * Vector3::Dot(velocity, info.hitNormal)));
					}
				}
				else
				{
					auto& soundManager = SoundManager::GetInstance();
					soundManager.Play("Reflect", 1.0f, true);
					pRigidbody->SetVelocity(velocity - info.hitNormal * (2.0f * Vector3::Dot(velocity, info.hitNormal)));
				}
				

				if (!m_pReflectCollider->IsEnable())
				{
					m_pReflectCollider->SetEnable(true);
				}
			}
		});

	pTransform->SetPos(pos);
	pTransform->SetScale(kBulletScale);

	Vector3 dir = target - pTransform->GetPos();
	//dir.y = 0.0f;
	dir.Normalize();

	// Rigidbodyにスピードを追加
	pRigidbody->SetVelocity(dir * kSpeed);

	FileManager& fileManager = FileManager::GetInstance();
	std::shared_ptr<File> bulletModelH = fileManager.GetModel(L"Data/File/Model/bullet.mv1", false);
	pDrawable3D->AddModel(Model::ModelSlot::Main, bulletModelH->GetHandle());

	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("Reflect", L"Data/File/Sound/SE/reflect.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("JustGuard", L"Data/File/Sound/SE/just_guard.mp3", SoundBus::SE, 1.0f, false);

	EffectManager::GetInstance().LoadEffect(L"bullet.efk");

	// コンポーネントの初期化処理を行う
	GameObject::Init();

	auto effectComponent = GetComponent<MyLib::EffectComponent>().lock();
	m_pBulletEffect = effectComponent->PlayEffect(L"bullet.efk");
}

void EnemyBullet::Update()
{
	// コンポーネントの更新処理を行う
	GameObject::Update();
}

void EnemyBullet::End()
{
	// コンポーネントの終了処理を行う
	GameObject::End();

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Reflect");
	soundManager.DeleteSoundClip("JustGuard");

	auto effect = m_pBulletEffect.lock();
	effect->StopEffect();

	EffectManager::GetInstance().DeleteEffect(L"bullet.efk");
}