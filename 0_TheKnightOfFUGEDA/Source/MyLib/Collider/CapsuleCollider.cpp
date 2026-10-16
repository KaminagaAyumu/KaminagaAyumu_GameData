#include "CapsuleCollider.h"
#include "../Renderer.h"

namespace
{
	// デバッグ用の当たり判定の色
	constexpr unsigned int kDebugColor = 0xff0000;
}

MyLib::CapsuleCollider::CapsuleCollider(ColliderBase::ObjectTag tag, float radius, float height, const Vector3& center, bool isTrigger, const std::string& name) :
	ColliderBase(ColliderShape::Capsule, tag, name, isTrigger),
	m_radius(radius),
	m_top(center + Vector3(0.0f,height / 2.0f, 0.0f)),
	m_bottom(center - Vector3(0.0f,height / 2.0f, 0.0f))
{
}

BoundingBox MyLib::CapsuleCollider::GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const
{
	// ワールド座標基準の現在位置を取得(回転を含む)
	Vector3 worldTop = worldPos + (rotation * m_top);
	Vector3 worldBottom = worldPos + (rotation * m_bottom);

	// カプセルの範囲を取得
	Vector3 maxPos = Vector3::Max(worldTop, worldBottom) + Vector3{ m_radius, m_radius,m_radius };
	Vector3 minPos = Vector3::Min(worldTop, worldBottom) - Vector3{ m_radius, m_radius,m_radius };

	// サイズの範囲分バウンディングボックスを制作する
	return BoundingBox(minPos, maxPos);
}

void MyLib::CapsuleCollider::DrawDebug(const Vector3& worldPos, const Quaternion& rotation) const
{
	Vector3 worldTop = worldPos + (rotation * m_top);
	Vector3 worldBottom = worldPos + (rotation * m_bottom);
	// TODO:カプセルを描画する関数を追加する
	Renderer::GetInstance().DrawCapsule(worldTop, worldBottom, m_radius, kDebugColor);
}
