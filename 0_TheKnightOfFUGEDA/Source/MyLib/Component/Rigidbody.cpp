#include "Rigidbody.h"
#include "Transform.h"
#include "../GameObject.h"
#include <cassert>

namespace
{
	constexpr float kDefaultMass = 1.0f;
}

MyLib::Rigidbody::Rigidbody() : 
	m_mass(kDefaultMass),
	m_bodyType(BodyType::Dynamic),
	m_isGravity(false),
	m_isApplyDirection(true)
{
}

MyLib::Rigidbody::~Rigidbody()
{
}

void MyLib::Rigidbody::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();
	// 親からTransformコンポーネントの参照を得る
	m_pTransform = pParent->GetComponent<MyLib::Transform>();
	// Transformが親にない場合assertする
	if (!m_pTransform.lock())
	{
		assert(false && "Rigidbody : Transformコンポーネントがありません");
	}
}

void MyLib::Rigidbody::Start()
{
}

void MyLib::Rigidbody::Update()
{
}

void MyLib::Rigidbody::End()
{
}
