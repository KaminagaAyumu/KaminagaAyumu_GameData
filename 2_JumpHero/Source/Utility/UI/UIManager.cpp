#include <string>
#include "UIManager.h"
#include "UITextWindow.h"
#include "UIText.h"
#include "UIFormatText.h"
#include "UISelectList.h"
#include "UIGauge.h"
#include "UIImage.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	//----------------
	// フォントデータ関連
	//----------------
	const wchar_t* kMainFontName = L"クラフト明朝"; // メインで使うフォント名

	// 小サイズのフォントハンドルの設定
	constexpr int kSmallFontSize		= 28;	// 文字の大きさ
	constexpr int kSmallFontEdgeSize	= 1;	// 文字の縁取りの大きさ

	// 中サイズのフォントハンドルの設定
	constexpr int kMediumFontSize		= 40;	// 文字の大きさ
	constexpr int kMediumFontEdgeSize	= 2;	// 文字の縁取りの大きさ

	// 大サイズのフォントハンドルの設定
	constexpr int kLargeFontSize		= 60;	// 文字の大きさ
	constexpr int kLargeFontEdgeSize	= 2; 	// 文字の縁取りの大きさ
	
	// 見出しサイズのフォントハンドルの設定
	constexpr int kHeaderFontSize		= 100;	// 文字の大きさ
	constexpr int kHeaderFontEdgeSize	= 4; 	// 文字の縁取りの大きさ

	//----------------
	// 画像データ関連
	//----------------

	constexpr int kButtonAnimNum = 4;
	constexpr int kButtonAnimFrame = 4;

	// 画像ファイル名の配列
	const std::wstring kImageFileName[] =
	{
		L"data/img/title_logo.png", // タイトルロゴ
		L"data/img/tutorial_icon.png", // タイトルロゴ
		L"data/img/stage1_icon.png", // ステージ1のアイコン
		L"data/img/stage2_icon.png", // ステージ2のアイコン
		L"data/img/stage3_icon.png", // ステージ3のアイコン
		L"data/img/title_icon.png", // タイトルのアイコン
		L"data/img/exit_icon.png", // ゲーム終了のアイコン
		L"data/img/player_icon.png", // プレイヤーのアイコン
		L"data/img/change_to_coin_icon.png", // 敵をコインに変えるアイテムのアイコン
		L"data/img/upgrade_medal_icon.png", // 強化メダルのアイコン
		L"data/img/button_a.png", // Aボタンのアイコン
	};

	// 画像枚数が違うときにエラーを出せるようにする
	static_assert(static_cast<int>(Types::ImageType::ImageNum) == _countof(kImageFileName), "画像枚数の定義が間違っています");
}



UIManager::UIManager()
{
	// UIコンテナを初期化
	m_pUIElements.clear();
	m_imageHandles.clear();

	// フォントデータを読み込んで設定
	LoadFonts();

	// 画像データを読み込んで設定
	LoadImages();

	m_windowGraphHandle = LoadGraph(L"data/img/window_back.png");
	m_windowFrameHandle = LoadGraph(L"data/img/window_frame.png");

	m_gaugeFrameHandle = LoadGraph(L"data/img/gauge_frame.png");
	m_gaugeFillHandle = LoadGraph(L"data/img/gauge_fill.png");

	m_gaugeYellowFrameHandle = LoadGraph(L"data/img/yellow_gauge_frame.png");
	m_gaugeYellowFillHandle = LoadGraph(L"data/img/yellow_gauge_fill.png");

	m_selectListFrameHandle = LoadGraph(L"data/img/select_list_frame.png");
	m_selectListFillHandle = LoadGraph(L"data/img/select_list_fill.png");
}

UIManager::~UIManager()
{
	// フォントのハンドルを開放
	for (auto& handle : m_fontHandles)
	{
		DeleteFontToHandle(handle);
	}

	// imageのハンドルを解放
	for (auto& image : m_imageHandles)
	{
		DeleteGraph(image);
	}

	DeleteGraph(m_windowGraphHandle);
	DeleteGraph(m_windowFrameHandle);
	
	DeleteGraph(m_gaugeFrameHandle);
	DeleteGraph(m_gaugeFillHandle);

	DeleteGraph(m_gaugeYellowFrameHandle);
	DeleteGraph(m_gaugeYellowFillHandle);

	DeleteGraph(m_selectListFrameHandle);
	DeleteGraph(m_selectListFillHandle);
}

void UIManager::Init()
{
}

void UIManager::Update()
{
	if (m_pUIElements.empty()) return;

	for (auto& element : m_pUIElements)
	{
		element->Update();
	}

	m_pUIElements.remove_if(
		[](const std::shared_ptr<UIBase>& element) 
		{
			return !element->IsAlive();
		}
	);


}

void UIManager::Draw()
{
	for (auto& element : m_pUIElements)
	{
		element->Draw();
	}
}

std::weak_ptr<UITextWindow> UIManager::CreateTextWindow(const std::string& text, const Size& size, const Position2& pos, Types::FontType fontType)
{
	auto ptr = std::make_shared<UITextWindow>();
	ptr->Init(text, size, pos, m_fontHandles[static_cast<int>(fontType)], m_windowFrameHandle,m_windowGraphHandle);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UITextWindow> UIManager::CreateTextWindowPaged(const std::string& id, const std::vector<TextData>& pages, const Size& size, const Position2& pos, float appearDuration, Types::FontType fontType)
{
	auto ptr = std::make_shared<UITextWindow>();
	ptr->Init("", size, pos, m_fontHandles[static_cast<int>(fontType)], m_windowFrameHandle, m_windowGraphHandle);
	ptr->SetPages(id, pages);
	ptr->ShowPaging(appearDuration);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIText> UIManager::CreateText(Types::FontType fontType, const std::string& text, const Position2& pos)
{
	auto ptr = std::make_shared<UIText>();
	ptr->Init(m_fontHandles[static_cast<int>(fontType)], text, pos);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIFormatText> UIManager::CreateFormatText(Types::FontType fontType, const std::string& text, const Position2& pos)
{
	auto ptr = std::make_shared<UIFormatText>();
	ptr->Init(m_fontHandles[static_cast<int>(fontType)], text, pos);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UISelectList> UIManager::CreateSelectList(Types::FontType fontType, const Size& size, const Position2& pos)
{
	auto ptr = std::make_shared<UISelectList>();
	ptr->Init(m_fontHandles[static_cast<int>(fontType)], size, pos);
	ptr->SetHandle(m_selectListFrameHandle, m_selectListFillHandle);
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIGauge> UIManager::CreateGauge(const Size& size, const Position2& pos, int gaugeNo)
{
	auto ptr = std::make_shared<UIGauge>();
	ptr->Init(size, pos);
	if (gaugeNo == 0)
	{
		ptr->SetHandle(m_gaugeFrameHandle, m_gaugeFillHandle);
	}
	else
	{
		ptr->SetHandle(m_gaugeYellowFrameHandle, m_gaugeYellowFillHandle);
	}
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIImage> UIManager::CreateImage(Types::ImageType type, const Size& size, const Position2& pos)
{
	auto ptr = std::make_shared<UIImage>();
	ptr->Init(m_imageHandles[static_cast<int>(type)], size, pos);
	// アニメーションがある画像の場合
	if (type == Types::ImageType::AButtonIcon)
	{
		// アニメーションを設定
		ptr->SetAnimation(kButtonAnimNum, kButtonAnimFrame, true);
	}
	m_pUIElements.push_back(ptr);
	return ptr;
}

void UIManager::LoadFonts()
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

void UIManager::LoadImages()
{
	for (int i = 0; i < static_cast<int>(Types::ImageType::ImageNum); i++)
	{
		int graphHandle = LoadGraph(kImageFileName[i].c_str());
		assert(graphHandle != -1 && "UI画像の読み込みに失敗しました");
		m_imageHandles.push_back(graphHandle);
	}
}
