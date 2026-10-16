#pragma once
#include <memory>
#include <list>
#include <vector>
#include "Component/Draw/Drawable.h"
#include "Component/Draw/Drawable2D.h"

namespace MyLib
{
	/// <summary>
	/// 描画クラス
	/// </summary>
	class Renderer
	{
	public:

		enum class FontType : uint8_t
		{
			Small, // 小サイズ
			Midium, // 中サイズ
			Large, // 大きいサイズ
			Header, // 見出しサイズ
			Default // 通常サイズ
		};

	public:
		virtual ~Renderer();

		/// <summary>
		/// インスタンスを取得する
		/// </summary>
		/// <returns>描画クラスのインスタンス</returns>
		static Renderer& GetInstance();

		void Init();
		void End();

		/// <summary>
		/// 描画コンポーネントを登録
		/// </summary>
		/// <param name="drawable">描画コンポーネント</param>
		void Entry(std::shared_ptr<MyLib::Drawable> drawable);

		/// <summary>
		/// 描画コンポーネントを解除
		/// </summary>
		/// <param name="drawable">描画コンポーネント</param>
		void Exit(std::shared_ptr<MyLib::Drawable> drawable);

		/// <summary>
		/// 描画を行う
		/// </summary>
		void Draw() const;

		void SetUILayerActive(MyLib::Drawable2D::UILayer layer, bool isActive);
		void SetUILayerAlpha(MyLib::Drawable2D::UILayer layer, int alpha);

		bool IsUILayerActive(MyLib::Drawable2D::UILayer layer) const { return  m_uiLayerDatas[static_cast<int>(layer)].isActive; }
		int GetUILayerAlpha(MyLib::Drawable2D::UILayer layer) const { return m_uiLayerDatas[static_cast<int>(layer)].alpha; }

		int GetFontHandle(FontType type);

		void SetShadowMapLightDir(const Vector3& dir);

		void SetShadowMapArea(const Vector3& min, const Vector3& max);

		void DebugDraw();

		void DebugClear();

		void DrawLine(const Vector3& start, const Vector3& end, unsigned int color);
		void DrawSphere(const Vector3& center, float radius, unsigned int color);
		void DrawCapsule(const Vector3& top, const Vector3& bottom, float radius, unsigned int color);
		void DrawBox(const Vector3& min, const Vector3& max, unsigned int color);

	private:

		// 描画コンポーネントリスト
		std::list<std::shared_ptr<MyLib::Drawable>> m_pDrawables;

		// UIレイヤーの透明度の最大値(不透明)
		static constexpr int kMaxUILayerAlpha = 255;

		struct UILayerData
		{
			bool isActive = true;
			int alpha = kMaxUILayerAlpha;
		};

		std::vector<UILayerData> m_uiLayerDatas;

		// フォントのハンドルを管理するvector
		std::vector<int> m_fontHandles;

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

		struct CapsuleInfo
		{
			Vector3 top;
			Vector3 bottom;
			float radius;
			unsigned int color;
		};

		struct BoxInfo
		{
			Vector3 min;
			Vector3 max;
			unsigned int color;
		};

		std::list<LineInfo> m_lineInfos;
		std::list<SphereInfo> m_sphereInfos;
		std::list<CapsuleInfo> m_capsuleInfos;
		std::list<BoxInfo> m_boxInfos;

		// シャドウマップのリソースハンドル
		int m_shadowMapHandle;

	private:
		/// <summary>
		/// コンストラクタ
		/// シングルトンクラスのためprivateで宣言する
		/// ※宣言の際にcppファイルの一番上に配置しています
		/// </summary>
		Renderer();
		Renderer(const Renderer&) = delete;			// コピーコンストラクタを作れないようにする
		void operator=(const Renderer&) = delete;	// 代入演算子を使えないようにする

		// すべてのフォントハンドルをロード
		void LoadAllFontHandle();
	};

}

