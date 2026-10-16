#include "BoxCollider.h"
#include "../Renderer.h"

namespace
{
	// デバッグ用の当たり判定の色
	constexpr unsigned int kDebugColor = 0xff0000;
}

MyLib::BoxCollider::BoxCollider(ColliderBase::ObjectTag tag, const Vector3& size, bool isTrigger, const std::string& name) :
	ColliderBase(ColliderShape::Box, tag, name, isTrigger),
	m_size(size)
{
}

BoundingBox MyLib::BoxCollider::GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const
{
	Vector3 worldMin = worldPos + (rotation * (m_size / 2));
	Vector3 worldMax = worldPos + (rotation * (m_size / 2));

	Vector3 max = Vector3::Max(worldMin, worldMax) + m_size / 2;
	Vector3 min = Vector3::Min(worldMin, worldMax) - m_size / 2;

	return BoundingBox{min, max};
}

void MyLib::BoxCollider::DrawDebug(const Vector3& worldPos, const Quaternion& rotation) const
{
	Vector3 worldMin = worldPos - (rotation * (m_size / 2));
	Vector3 worldMax = worldPos + (rotation * (m_size / 2));

	Vector3 max = Vector3::Max(worldMin, worldMax) + (m_size / 2);
	Vector3 min = Vector3::Min(worldMin, worldMax) - (m_size / 2);

	Renderer::GetInstance().DrawBox(min, max, kDebugColor);
}
