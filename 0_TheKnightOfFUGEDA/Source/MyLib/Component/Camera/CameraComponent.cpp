#include "CameraComponent.h"
#include "../../GameObject.h"
#include "../Transform.h"
#include "../../../Utility/CameraManager.h"
#include <cassert>

MyLib::CameraComponent::CameraComponent() : 
	m_fov(0.0f),
	m_near(0.0f),
	m_far(0.0f),
	m_priority(0)
{
}

MyLib::CameraComponent::~CameraComponent()
{
}

void MyLib::CameraComponent::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	// 親となるゲームオブジェクトを取得する
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();
	// 親からTransformコンポーネントの参照を得る
	m_pTransform = pParent->GetComponent<MyLib::Transform>();
	// Transformが親にない場合assertする
	if (!m_pTransform.lock())
	{
		assert(false && "CameraComponent : Rigidbodyコンポーネントがありません");
	}

	CameraManager::GetInstance().Entry(shared_from_this());
}

void MyLib::CameraComponent::Start()
{
}

void MyLib::CameraComponent::Update()
{
}

void MyLib::CameraComponent::End()
{
	CameraManager::GetInstance().Exit(shared_from_this());
}

const Position3 MyLib::CameraComponent::GetPosition() const
{
	// Transformコンポーネントを取得する
	std::shared_ptr<MyLib::Transform> pTransform = m_pTransform.lock();

	// Transformコンポーネントの座標を取得する
	if (pTransform)
	{
		return pTransform->GetRotation() * pTransform->GetPos();
	}

	// 見つからなかった場合はアサートする
	assert(false && "CameraComponent : カメラの座標を取得できませんでした");

	// 見つからなかった場合はZeroVectorを返す
	return Vector3::Zero();
}
