#pragma once
#include "ColliderBase.h"
#include <vector>

namespace MyLib
{
	class WallMeshCollider : public ColliderBase
	{
	public:
		/// <summary>
		/// コンストラクタ
		/// 生成された頂点データを受け取る
		/// </summary>
		/// <param name="vertices">頂点データ</param>
		/// <param name="indices">頂点インデックス</param>
		WallMeshCollider(ColliderBase::ObjectTag tag, std::vector<Vector3>&& vertices, std::vector<int>&& indices, bool isTrigger, const std::string& name = "");
		virtual ~WallMeshCollider() = default;

		BoundingBox GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const override;

		virtual void DrawDebug(const Vector3& worldPos, const Quaternion& rotation)const override;

		const std::vector<Vector3>& GetVertices() const { return m_vertices; }
		const std::vector<int>& GetIndices() const { return m_indices; }

	private:
		/// <summary>
		/// バウンディングボックスを作成する(このコライダーが生成されたときに呼ぶ)
		/// </summary>
		void CulculateBoundingBox();

	private:
		// 頂点データ
		std::vector<Vector3> m_vertices;
		// 頂点インデックス
		std::vector<int> m_indices;
		// バウンディングボックス(毎フレーム呼ぶと重いため予め持っておく)
		BoundingBox m_boundingBox;
	};

}

