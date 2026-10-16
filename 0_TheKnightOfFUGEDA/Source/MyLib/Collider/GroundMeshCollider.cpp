#include "GroundMeshCollider.h"

namespace
{
	// 三角形の頂点の数
	constexpr int kTriangleVertexNum = 3;

	// デバッグ用の当たり判定の色
	constexpr unsigned int kDebugColor = 0xf47f00;
}

MyLib::GroundMeshCollider::GroundMeshCollider(ColliderBase::ObjectTag tag, std::vector<Vector3>&& vertices, std::vector<int>&& indices, bool isTrigger, const std::string& name) :
	ColliderBase(ColliderShape::GroundMesh, tag, name, isTrigger),
	m_vertices(std::move(vertices)),
	m_indices(std::move(indices))
{
	// バウンディングボックスを作成する
	CulculateBoundingBox();
}

BoundingBox MyLib::GroundMeshCollider::GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const
{
	return m_boundingBox;
}

void MyLib::GroundMeshCollider::DrawDebug(const Vector3& worldPos, const Quaternion& rotation) const
{
	for (size_t i = 0; i < m_indices.size(); i += kTriangleVertexNum)
	{

		Vector3 pos0 = m_vertices[m_indices[i]];
		Vector3 pos1 = m_vertices[m_indices[i + 1]];
		Vector3 pos2 = m_vertices[m_indices[i + 2]];


		DrawTriangle3D(pos0, pos1, pos2, kDebugColor, false);
	}
}

void MyLib::GroundMeshCollider::CulculateBoundingBox()
{
	if (m_vertices.empty()) return;

	Vector3 minPos = m_vertices[0];
	Vector3 maxPos = m_vertices[0];

	for (auto& pos : m_vertices)
	{
		minPos = Vector3::Min(minPos, pos);
		maxPos = Vector3::Max(maxPos, pos);
	}

	m_boundingBox = BoundingBox{ minPos, maxPos };
}
