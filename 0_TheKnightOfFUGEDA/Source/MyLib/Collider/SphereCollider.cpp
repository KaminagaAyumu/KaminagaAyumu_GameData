#include "SphereCollider.h"
#include "../Renderer.h"

namespace
{
	// デバッグ用の当たり判定の色
	constexpr unsigned int kDebugColor = 0xff0000;
}

MyLib::SphereCollider::SphereCollider(ColliderBase::ObjectTag tag, float radius, bool isTrigger, const std::string& name) :
	ColliderBase(ColliderShape::Sphere, tag, name, isTrigger),
	m_radius(radius)
{
}

BoundingBox MyLib::SphereCollider::GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const
{
	// 球の範囲を取得
	Vector3 extent = { m_radius, m_radius, m_radius };

	// サイズの範囲分バウンディングボックスを制作する
	return BoundingBox(worldPos - extent, worldPos + extent);
}

void MyLib::SphereCollider::DrawDebug(const Vector3& worldPos, const Quaternion& rotation) const
{
	Renderer::GetInstance().DrawSphere(worldPos, m_radius, kDebugColor);
}
