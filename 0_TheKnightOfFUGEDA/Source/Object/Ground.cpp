#include "Ground.h"
#include "../MyLib/Component/Rigidbody.h"
#include "../MyLib/Component/Collision/Collidable.h"
#include "../MyLib/Component/Draw/Drawable3D.h"
#include "../MyLib/Collider/SphereCollider.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/File.h"
#include <memory>

namespace
{
	// モデルのスケール
	const Vector3 kModelScale = { 0.01f,0.01f,0.01f };

	// ファイルを読み込む際のパスの最大サイズ(文字数)
	constexpr size_t kFilePathMax = 256;

	//const Vector3 kPos = { 0.0f, 980.0f * 0.01f, 2205.0f * 0.01f };
}

Ground::Ground() :
	GameObject(MyLib::GameObject::Type::BackGround)
{
}

Ground::~Ground()
{
}

void Ground::Init(int stageNo)
{
	AddComponent<MyLib::Rigidbody>();							// Rigidbodyコンポーネントを追加
	AddComponent<MyLib::Collidable>();							// Collidableコンポーネントを追加
	AddComponent<MyLib::Drawable3D>(MyLib::DrawLayer::Opaque);	// 3D描画コンポーネントを追加

	auto rigid = GetComponent<MyLib::Rigidbody>().lock();
	rigid->SetBodyType(MyLib::BodyType::Static);

	std::shared_ptr<MyLib::Transform> pTransform = GetComponent<MyLib::Transform>().lock();

	std::shared_ptr<MyLib::Drawable3D> pDrawable = GetComponent<MyLib::Drawable3D>().lock();

	// 影を落とされるフラグをtrueにする
	pDrawable->SetReceiveShadow(true);


	FileManager& fileManager = FileManager::GetInstance();
	
	wchar_t groundFilePath[kFilePathMax];
	std::swprintf(groundFilePath, kFilePathMax, L"Data/File/Model/ground_%d.mv1", stageNo);
	std::shared_ptr<File> GroundModelH = fileManager.GetModel(groundFilePath, false);

	pDrawable->AddModel(Model::ModelSlot::Main, GroundModelH->GetHandle());
	m_pModelOffset = std::make_shared<MyLib::Transform>();
	m_pModelOffset->SetScale(kModelScale);
	m_pModelOffset->SetRotation(Quaternion::SetRotation(Vector3::Zero(), Vector3::Right()));
	//m_pModelOffset->SetPos(kPos);

	pDrawable->SetModelOffset(Model::ModelSlot::Main, m_pModelOffset);

	//pDrawable->SetEnable(Model::ModelSlot::Main, false);

	GameObject::Init();
}

void Ground::Update()
{
	GameObject::Update();
}

void Ground::End()
{
	GameObject::End();
}