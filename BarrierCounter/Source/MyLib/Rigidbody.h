#pragma once
#include "../Geometry/Vector3.h"

namespace MyLib
{
	/// <summary>
	/// 物理クラス
	/// </summary>
	class Rigidbody
	{
	public:
		Rigidbody();

		/// <summary>
		/// 初期化(すべての値が0になる)
		/// </summary>
		void Init();

		const Vector3& GetPos() const { return m_pos; }
		const Vector3& GetDir() const { return m_dir; }
		const Vector3& GetVelocity() const { return m_velocity; }

		/// <summary>
		/// 座標をセットする
		/// </summary>
		/// <param name="set">セットする座標</param>
		void SetPos(const Vector3& set) { m_pos = set; }

		/// <summary>
		/// 速度をセットする
		/// 速度ベクトルから向きベクトルも計算する
		/// </summary>
		/// <param name="set">速度ベクトル</param>
		void SetVelocity(const Vector3& set);

	private:
		Vector3 m_pos;			// 座標
		Vector3 m_dir;			// 向き
		Vector3 m_velocity;		// 速度
	};

}



