#include "../Geometry/Vector3.h"
#include "Collidable.h"
#include "SphereCollider.h"
#include "Physics.h"
#include "../Common/Game.h"
#include <cassert>

using namespace MyLib;

Collidable::Collidable(ObjectTag tag, ColliderBase::ColliderType type) :
	m_tag(tag)
{
	CreateCollider(type);
}

Collidable::~Collidable()
{
}

void Collidable::Init()
{
	auto& physics = Physics::GetInstance();
	physics.Entry(shared_from_this());
}

void Collidable::End()
{
	auto& physics = Physics::GetInstance();
	physics.Exit(shared_from_this());
}

bool Collidable::IsOutFromMap() const
{
	// 現在の座標がマップの範囲内から出たかどうかをチェック
	return (m_rigidbody.GetPos().x >= Game::kMapMaxX ||
		m_rigidbody.GetPos().x <= Game::kMapMinX ||
		m_rigidbody.GetPos().z >= Game::kMapMaxZ ||
		m_rigidbody.GetPos().z <= Game::kMapMinZ);
}

void Collidable::CreateCollider(ColliderBase::ColliderType type)
{
	if (m_pColliderData != nullptr)
	{
		assert(false && L"当たり判定データがすでに作られています");
	}
	else
	{
		switch (type)
		{
		case ColliderBase::ColliderType::Sphere:
			m_pColliderData = std::make_shared<SphereCollider>();
			break;
		case ColliderBase::ColliderType::Line:
			break;
		case ColliderBase::ColliderType::Capsule:
			break;
		default:
			break;
		}
	}
}
