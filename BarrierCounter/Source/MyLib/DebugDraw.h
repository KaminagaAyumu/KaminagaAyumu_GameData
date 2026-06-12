#pragma once
#include <string>

namespace MyLib
{

	/// <summary>
	/// デバッグ用の描画を行うクラス
	/// </summary>
	class DebugDraw
	{
	public:
		// デストラクタ
		// 処理なし
		virtual ~DebugDraw() = default;

		/// <summary>
		/// インスタンスを取得する
		/// </summary>
		/// <returns></returns>
		static DebugDraw& GetInstance();

		void Clear();
		void Draw();

		void GetPrintData(const std::string str);

		void DrawLine(const Vector3& start, const Vector3& end, unsigned int color);
		void DrawSphere(const Vector3& center, float radius, unsigned int color);

	private:

		// 線の情報
		struct LineInfo
		{
			Vector3 start;
			Vector3 end;
			unsigned int color;
		};

		// 球の情報
		struct SphereInfo
		{
			Vector3 center;
			float radius;
			unsigned int color;
		};
		
		std::list<LineInfo> m_lineInfos;
		std::list<SphereInfo> m_sphereInfos;

	private:
		DebugDraw(); // コンストラクタをprivateで宣言
		DebugDraw(const DebugDraw&) = delete; // コピーコンストラクタを作れないようにする
		void operator=(const DebugDraw&) = delete; // 代入演算子も使えないようにする
	};

}



