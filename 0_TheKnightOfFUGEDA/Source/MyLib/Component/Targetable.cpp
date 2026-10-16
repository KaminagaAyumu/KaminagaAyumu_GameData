#include "Targetable.h"
#include "Transform.h"
#include "../GameObject.h"
#include <cassert>

MyLib::Targetable::Targetable()
{
}

MyLib::Targetable::~Targetable()
{
}

void MyLib::Targetable::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();
	// 親からTransformコンポーネントの参照を得る
	m_pTransform = pParent->GetComponent<MyLib::Transform>();
	// Transformが親にない場合assertする
	if (!m_pTransform.lock())
	{
		assert(false && "Targetable : Transformコンポーネントがありません");
	}

}

void MyLib::Targetable::Start()
{
}

void MyLib::Targetable::Update()
{
}

void MyLib::Targetable::End()
{
}

const Vector3 MyLib::Targetable::GetLockOnPoint() const
{
	if (std::shared_ptr<Transform> pTransform = m_pTransform.lock())
	{
		return pTransform->GetPos() + m_lockOnOffset;
	}

	return m_lockOnOffset;
}
