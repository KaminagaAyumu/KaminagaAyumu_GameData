#include "Renderer.h"
#include "../Common/Effect/EffectManager.h"
#include <cassert>

namespace
{
	// フォント名のパス
	const wchar_t* kMainFontName = L"HG明朝E";
	//const wchar_t* kMainFontName = L"チェックポイント★リベンジ";

	// 小サイズのフォントハンドルの設定
	constexpr int kSmallFontSize = 28;	// 文字の大きさ
	constexpr int kSmallFontEdgeSize = 1;	// 文字の縁取りの大きさ

	// 中サイズのフォントハンドルの設定
	constexpr int kMediumFontSize = 40;	// 文字の大きさ
	constexpr int kMediumFontEdgeSize = 2;	// 文字の縁取りの大きさ

	// 大サイズのフォントハンドルの設定
	constexpr int kLargeFontSize = 60;	// 文字の大きさ
	constexpr int kLargeFontEdgeSize = 2; 	// 文字の縁取りの大きさ

	// 見出しサイズのフォントハンドルの設定
	constexpr int kHeaderFontSize = 100;	// 文字の大きさ
	constexpr int kHeaderFontEdgeSize = 4; 	// 文字の縁取りの大きさ


	// デバッグ用に表示するポリゴンの細かさ
	constexpr int kDivNum = 8;

	// シャドウマップのサイズ
	constexpr int kShadowMapSize = 8192;
}

MyLib::Renderer::Renderer() : 
	m_shadowMapHandle(-1)
{

}

void MyLib::Renderer::LoadAllFontHandle()
{
	// フォントハンドル(小サイズ)を作成
	int handle = CreateFontToHandle(
		kMainFontName,
		kSmallFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kSmallFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(中サイズ)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kMediumFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kMediumFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(大サイズ)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kLargeFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kLargeFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);

	// フォントハンドル(見出しサイズ)を作成
	handle = CreateFontToHandle(
		kMainFontName,
		kHeaderFontSize,
		-1,
		DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
		-1,
		kHeaderFontEdgeSize,
		FALSE
	);
	// ハンドルを格納
	m_fontHandles.push_back(handle);
}

MyLib::Renderer::~Renderer()
{

}

MyLib::Renderer& MyLib::Renderer::GetInstance()
{
	static Renderer instance;
	return instance;
}

void MyLib::Renderer::Init()
{
	m_pDrawables.clear();

	m_uiLayerDatas.resize(static_cast<int>(MyLib::Drawable2D::UILayer::Max));

	LoadAllFontHandle();

	m_shadowMapHandle = MakeShadowMap(kShadowMapSize, kShadowMapSize);
}

void MyLib::Renderer::End()
{
	// フォントのハンドルを消去
	for (auto& fontHandle : m_fontHandles)
	{
		DeleteFontToHandle(fontHandle);
	}

	DeleteShadowMap(m_shadowMapHandle);
}

void MyLib::Renderer::Entry(std::shared_ptr<MyLib::Drawable> drawable)
{
	// 既に登録されているかを確認
	bool isFound = (std::find(m_pDrawables.begin(), m_pDrawables.end(), drawable) != m_pDrawables.end());

	if (isFound)
	{
		// 既に登録されていた場合アサート
		assert(false && "既に登録されている描画コンポーネントが登録されました");
	}
	else
	{
		// 登録する
		m_pDrawables.emplace_back(drawable);
	}

}

void MyLib::Renderer::Exit(std::shared_ptr<MyLib::Drawable> drawable)
{
	// 登録されている描画コンポーネントを確認
	auto count = std::erase_if(m_pDrawables, [drawable](std::shared_ptr<MyLib::Drawable> target)
		{
			// 対象の描画コンポーネントと一致したものを消去
			return target == drawable;
		});
}

void MyLib::Renderer::Draw()const
{
	///////////////////////
	// シャドウマップへの描画
	///////////////////////
	ShadowMap_DrawSetup(m_shadowMapHandle);
	for (auto& drawable : m_pDrawables)
	{
		if (drawable->IsCastShadow())
		{
			// 描画を行う
			drawable->Draw();
		}
	}
	ShadowMap_DrawEnd();
	///////////////////////
	// シャドウマップへの描画
	///////////////////////


	// 通常の描画
	// レイヤーの順番に描画を行う
	for (uint8_t layer = 0; layer < static_cast<uint8_t>(MyLib::DrawLayer::Max); ++layer)
	{
		for (auto& drawable : m_pDrawables)
		{
			// レイヤーが違う場合処理をしない
			if (static_cast<uint8_t>(drawable->GetDrawLayer()) != layer) continue;


			const bool isReceiveShadow = drawable->IsReceiveShadow();

			if (isReceiveShadow)
			{
				SetUseShadowMap(0, m_shadowMapHandle);
			}

			// 描画を行う
			drawable->Draw();

			if (isReceiveShadow)
			{
				SetUseShadowMap(0, -1);
			}
		}

		if (static_cast<MyLib::DrawLayer>(layer) == MyLib::DrawLayer::Effect)
		{
			EffectManager::GetInstance().Draw();
		}
	}

	//TestDrawShadowMap(m_shadowMapHandle, 0, 0, 320, 320);
}

void MyLib::Renderer::SetUILayerActive(MyLib::Drawable2D::UILayer layer, bool isActive)
{
	m_uiLayerDatas[static_cast<int>(layer)].isActive = isActive;
}

void MyLib::Renderer::SetUILayerAlpha(MyLib::Drawable2D::UILayer layer, int alpha)
{
	m_uiLayerDatas[static_cast<int>(layer)].alpha = alpha;
}

int MyLib::Renderer::GetFontHandle(FontType type)
{
	return m_fontHandles[static_cast<int>(type)];
}

void MyLib::Renderer::SetShadowMapLightDir(const Vector3& dir)
{
	SetShadowMapLightDirection(m_shadowMapHandle, dir);
}

void MyLib::Renderer::SetShadowMapArea(const Vector3& min, const Vector3& max)
{
	SetShadowMapDrawArea(m_shadowMapHandle, min, max);
}

void MyLib::Renderer::DebugDraw()
{
	for (const auto& item : m_lineInfos)
	{
		DxLib::DrawLine3D(item.start, item.end, item.color);
	}
	for (const auto& item : m_sphereInfos)
	{
		DxLib::DrawSphere3D(item.center, item.radius, kDivNum, item.color, item.color, false);
	}
	for (const auto& item : m_capsuleInfos)
	{
		DxLib::DrawCapsule3D(item.top, item.bottom, item.radius, kDivNum, item.color, item.color, false);
	}
	for (const auto& item : m_boxInfos)
	{
		DxLib::DrawCube3D(item.min, item.max, item.color, item.color, false);
	}
}

void MyLib::Renderer::DebugClear()
{
	m_lineInfos.clear();
	m_sphereInfos.clear();
	m_capsuleInfos.clear();
	m_boxInfos.clear();
}

void MyLib::Renderer::DrawLine(const Vector3& start, const Vector3& end, unsigned int color)
{
	LineInfo newInfo;
	newInfo.start = start;
	newInfo.end = end;
	newInfo.color = color;
	m_lineInfos.push_back(newInfo);
}

void MyLib::Renderer::DrawSphere(const Vector3& center, float radius, unsigned int color)
{
	SphereInfo newInfo;
	newInfo.center = center;
	newInfo.radius = radius;
	newInfo.color = color;
	m_sphereInfos.push_back(newInfo);
}

void MyLib::Renderer::DrawCapsule(const Vector3& top, const Vector3& bottom, float radius, unsigned int color)
{
	CapsuleInfo newInfo;
	newInfo.top = top;
	newInfo.bottom = bottom;
	newInfo.radius = radius;
	newInfo.color = color;
	m_capsuleInfos.push_back(newInfo);
}

void MyLib::Renderer::DrawBox(const Vector3& min, const Vector3& max, unsigned int color)
{
	BoxInfo newInfo;
	newInfo.min = min;
	newInfo.max = max;
	newInfo.color = color;
	m_boxInfos.push_back(newInfo);
}
