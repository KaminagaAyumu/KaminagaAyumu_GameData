#include "ItemController.h"
#include "../../GameObject.h"
#include "../Collision/Collidable.h"
#include "../Draw/Drawable3D.h"
#include "../EffectComponent.h"
#include "../../Collider/SphereCollider.h"
#include "../../../Utility/File/FileManager.h"
#include "../../../Utility/File/File.h"
#include "../../../Common/Effect/EffectManager.h"
#include "../../../Common/Sound/SoundManager.h"

namespace
{
	// 当たり判定の半径
	constexpr float kColRadius = 1.0f;

	// 回転する速度
	constexpr float kRotSpeed = 0.05f;

	// 上下動する速度
	constexpr float kWaveSpeed = 0.05f;

	// 上下動する範囲
	constexpr float kWaveRange = 0.1f;

	// 獲得時に上昇する速度
	constexpr float kUpSpeed = 0.05f;

	// 獲得時の回転する速度
	constexpr float kGetRotSpeed = 0.8f;

	// 獲得してから消えるまでのフレーム数
	constexpr int kDestroyFrame = 30;

	// 初期位置
	const Vector3 kFirstPos = { 0.0f, 1.0f, 10.0f };

	// モデルのサイズ
	const Vector3 kModelScale = { 0.005f, 0.005f, 0.005f };
}

MyLib::ItemController::ItemController() : 
	m_angle(0.0f),
	m_wave(0.0f),
	m_isDestroy(false),
	m_destroyFrame(0)
{
}

MyLib::ItemController::~ItemController()
{
}

void MyLib::ItemController::Init(std::weak_ptr<MyLib::GameObject> parent)
{

	// 親オブジェクトを取得
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();

	m_pGameObj = parent;

	m_pTransform = pParent->GetComponent<MyLib::Transform>();	// Transformコンポーネントを取得
	m_pDrawable = pParent->GetComponent<MyLib::Drawable3D>();	// Drawable3Dコンポーネントを取得
	std::shared_ptr<MyLib::Collidable> pCollidable = pParent->GetComponent<MyLib::Collidable>().lock();	// Collidableコンポーネントを取得
	std::shared_ptr<MyLib::Transform> pTransform = m_pTransform.lock();									// Transformコンポーネントを取得
	std::shared_ptr<MyLib::Drawable3D> pDrawable = m_pDrawable.lock();									// Drawable3Dコンポーネントを取得
	std::shared_ptr<MyLib::Rigidbody> pRigidbody = pParent->GetComponent<MyLib::Rigidbody>().lock();	// Rigidbodyコンポーネントを取得

	// ファイルマネージャーを取得
	FileManager& fileManager = FileManager::GetInstance();
	// アイテムのモデルリソースを取得
	std::shared_ptr<File> modelFile = fileManager.GetModel(L"Data/File/Model/coin.mv1", false);

	// Rigidbodyを動かない状態にする
	pRigidbody->SetBodyType(MyLib::BodyType::Static);

	// モデルをセットする
	pDrawable->AddModel(Model::ModelSlot::Main, modelFile->GetHandle());

	m_pModelOffset = std::make_shared<MyLib::Transform>();
	m_pModelOffset->SetPos(Vector3::Zero());

	pDrawable->SetModelOffset(Model::ModelSlot::Main, m_pModelOffset);

	// サウンドを登録
	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("Coin", L"Data/File/Sound/SE/coin_get.mp3", SoundBus::SE, 1.0f, false);

	pCollidable->AddCollider(std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Item, kColRadius, true));

	pCollidable->SetOnCollideEnter([&, pParent](const MyLib::CollisionInfo& info)
		{
			if (info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::Player && info.otherCollider->GetName() != "PlayerAttack")
			{
				// コイン取得時のエフェクトを再生
				auto effectComponent = pParent->GetComponent<MyLib::EffectComponent>().lock();
				effectComponent->PlayEffect(L"coin.efk");
				// コイン取得時のSEを再生
				SoundManager::GetInstance().Play("Coin", 1.0f, true);
				// コインオブジェクトを消去する
				//pParent->Destroy();
				m_isDestroy = true;
			}
		});

	//pTransform->SetPos(kFirstPos);

	pTransform->SetScale(kModelScale);

	// エフェクトを登録
	auto& effectManager = EffectManager::GetInstance();
	effectManager.LoadEffect(L"coin.efk");
}

void MyLib::ItemController::Start()
{
}

void MyLib::ItemController::Update()
{
	

	if (m_isDestroy)
	{
		m_destroyFrame++;

		m_angle += kGetRotSpeed;

		std::shared_ptr<MyLib::Transform> pTransform = m_pTransform.lock();

		pTransform->SetRotation(Quaternion::AngleAxis(m_angle, Vector3::Up()));

		Vector3 pos = pTransform->GetPos();
		pos.y += kUpSpeed;
		pTransform->SetPos(pos);

		if (m_destroyFrame > kDestroyFrame)
		{
			m_pGameObj.lock()->Destroy();
		}
	}
	else
	{
		m_angle += kRotSpeed;

		m_wave += kWaveSpeed;

		std::shared_ptr<MyLib::Transform> pTransform = m_pTransform.lock();

		pTransform->SetRotation(Quaternion::AngleAxis(m_angle, Vector3::Up()));

		Vector3 pos = pTransform->GetPos();
		pos.y = 1.0f + std::sinf(m_wave) * kWaveRange;
		pTransform->SetPos(pos);
	}

}

void MyLib::ItemController::End()
{
	auto& effectManager = EffectManager::GetInstance();
	effectManager.DeleteEffect(L"coin.efk");

	// サウンドを登録
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Coin");
}
