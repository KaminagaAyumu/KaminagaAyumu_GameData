#pragma once
#include "Component.h"
#include "../../Geometry/Vector2Int.h"
#include "../../Geometry/Vector3.h"
#include "../../Geometry/Matrix4x4.h"
#include "../../Geometry/Quaternion.h"

namespace MyLib
{
	// プロトタイプ宣言
	class GameObject;

	/// <summary>
	/// 座標コンポーネント
	/// </summary>
	class Transform : public Component
	{
	public:

		Transform();
		virtual ~Transform();

		const Position3& GetPos() { return m_pos; }
		const Vector3 GetDir() { return (m_rotation * -Vector3::Forward()).Normalized(); }
		const Vector3& GetScale() { return m_scale; }
		const Quaternion& GetRotation() { return m_rotation; }
		const Matrix4x4 GetWorldMatrix();
		/// <summary>
		/// 拡大、移動、回転の順で計算を行う行列を返す
		/// </summary>
		/// <returns></returns>
		const Matrix4x4 GetPivotMatrix();

		/// <summary>
		/// 座標をセットする
		/// </summary>
		/// <param name="set">セットする座標</param>
		void SetPos(const Position3& set) { m_pos = set; }

		/// <summary>
		/// スケールをセットする
		/// </summary>
		/// <param name="set">セットするスケール</param>
		void SetScale(const Vector3& set) { m_scale = set; }

		/// <summary>
		/// 回転をセットする
		/// </summary>
		/// <param name="set">セットする回転</param>
		void SetRotation(const Quaternion& set) { m_rotation = set; m_rotation.Normalize(); }

		void SetScreenPos(const Vector2Int& set) { m_screenPos = set; }
		const Vector2Int& GetScreenPos() const { return m_screenPos; }

		virtual void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void End() override;

	private:
		Position3 m_pos;		// 座標
		Vector3 m_scale;		// スケール
		Quaternion m_rotation;	// 回転
		Vector2Int m_screenPos;	// スクリーン座標
	};
}



