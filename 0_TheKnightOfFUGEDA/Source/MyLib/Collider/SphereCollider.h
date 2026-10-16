#pragma once
#include "ColliderBase.h"

namespace MyLib
{
	/// <summary>
	/// 球状の当たり判定
	/// </summary>
	class SphereCollider : public ColliderBase
	{
	public:
		SphereCollider(ColliderBase::ObjectTag tag, float radius, bool isTrigger, const std::string& name = "");
		virtual ~SphereCollider() = default;
		// 半径のゲッター、セッター
		float GetRadius() const { return m_radius; }
		void SetRadius(float radius) { m_radius = radius; }

		virtual BoundingBox GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const override;

		/// <summary>
		/// デバッグ用の表示を行う
		/// </summary>
		/// <param name="worldPos"></param>
		virtual void DrawDebug(const Vector3& worldPos, const Quaternion& rotation)const override;

	private:
		// 半径
		float m_radius;
	};
}



