#include "Skybox.h"
#include "../MyLib/Component/Transform.h"
#include "../MyLib/Component/Draw/Drawable3D.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/File.h"
#include <memory>

namespace
{
	// モデルのスケール
	const Vector3 kSkyboxScale = { 1.5f,1.5f,1.5f };

	// スカイボックスの回転速度
	constexpr float kRotSpeed = 0.001f;
}

Skybox::Skybox() :
	GameObject(MyLib::GameObject::Type::BackGround),
	m_angle(0.0f)
{
}

Skybox::~Skybox()
{
}

void Skybox::Init(Type type)
{
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加

	// Transformコンポーネントを取得
	std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock();
	// Drawable3Dコンポーネントを取得
	std::shared_ptr<MyLib::Drawable3D> pDrawable3D = GetComponent<MyLib::Drawable3D>().lock();

	pTransform->SetScale(kSkyboxScale);

	// 影を落とすフラグをfalseにする
	pDrawable3D->SetCastShadow(false);

	FileManager& fileManager = FileManager::GetInstance();
	
	std::shared_ptr<File> skyboxModelH;

	switch (type)
	{
	case Skybox::Type::Morning:
		skyboxModelH = fileManager.GetModel(L"Data/File/Model/skybox_morning.mv1", false);
		break;
	case Skybox::Type::Noon:
		skyboxModelH = fileManager.GetModel(L"Data/File/Model/skybox_noon.mv1", false);
		break;
	case Skybox::Type::Evening:
		skyboxModelH = fileManager.GetModel(L"Data/File/Model/skybox_evening.mv1", false);
		break;
	case Skybox::Type::Night:
		skyboxModelH = fileManager.GetModel(L"Data/File/Model/skybox_night.mv1", false);
		break;
	default:
		break;
	}
	
	pDrawable3D->AddModel(Model::ModelSlot::Main, skyboxModelH->GetHandle());

	GameObject::Init();
}

void Skybox::Update()
{
	m_angle += kRotSpeed;

	// Transformコンポーネントを取得
	std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock();

	if (pTransform)
	{
		pTransform->SetRotation(Quaternion::AngleAxis(m_angle, Vector3::Up()));
	}

	GameObject::Update();
}

void Skybox::End()
{
	GameObject::End();
}