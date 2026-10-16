#pragma once

namespace MyLib
{

	class ColliderBase abstract
	{
	public:

		/// <summary>
		/// 当たり判定タイプ
		/// </summary>
		enum class ColliderType
		{
			Sphere,
			Line,
			Capsule
		};

		/// <summary>
		/// コンストラクタ
		/// 当たり判定タイプを設定
		/// </summary>
		/// <param name="type"></param>
		ColliderBase(ColliderType type) : m_type(type){}
		virtual ~ColliderBase() = default;

	private:
		ColliderType m_type;

	};
}



