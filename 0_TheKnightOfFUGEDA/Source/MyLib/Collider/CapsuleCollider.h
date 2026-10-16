#pragma once
#include "ColliderBase.h"

namespace MyLib
{
	/// <summary>
	/// カプセル状の当たり判定
	/// </summary>
	class CapsuleCollider : public ColliderBase
	{
	public:
		CapsuleCollider(ColliderBase::ObjectTag tag, float radius, float height, const Vector3& center, bool isTrigger, const std::string& name = "");
		virtual ~CapsuleCollider() = default;

		// 情報のゲッター、セッター
		float GetRadius() const { return m_radius; }
		void SetRadius(float radius) { m_radius = radius; }
		const Vector3& GetTop() const { return m_top; }
		void SetTop(const Vector3& top) { m_top = top; }
		const Vector3& GetBottom() const { return m_bottom; }
		void SetBottom(const Vector3& bottom) { m_bottom = bottom; }

		const Vector3 GetLine() const { return m_top - m_bottom; }

		virtual BoundingBox GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const override;

		/// <summary>
		/// デバッグ用の表示を行う
		/// </summary>
		/// <param name="worldPos"></param>
		virtual void DrawDebug(const Vector3& worldPos, const Quaternion& rotation)const override;

	private:
		// 半径
		float m_radius;

		// 球の中心座標
		Vector3 m_top;		// 上側
		Vector3 m_bottom;	// 下側
	};

}



