#include "SphereCollider.h"

using namespace MyLib;

SphereCollider::SphereCollider() : 
	ColliderBase(ColliderType::Sphere),
	m_radius(0.0f)
{
}

SphereCollider::~SphereCollider()
{
	// 処理なし
}
