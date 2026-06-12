#include "UIManager.h"
#include "UIImage.h"
#include "UIText.h"
#include "UISelectList.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// フォント名のパス
	const wchar_t* kMainFontName = L"チェックポイント★リベンジ";

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

	// レイヤーの初期ブレンド状態
	constexpr int kDefaultLayerAlpha = 255;

	// 共通で使用するパスの文字列
	const std::wstring kCommonPathName = L"Data/UI/";

	// リソースが存在していることを示す
	constexpr int kResourceExist = 1;
}

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

UIManager& UIManager::GetInstance()
{
	static UIManager instance;
	return instance;
}

void UIManager::Init()
{
	// 画像リソースのコンテナをクリア
	m_uiResources.clear();

	// UIのリストをクリア
	m_pUIElements.clear();

	// フォントのハンドルをセット
	LoadAllFontHandle();

	// レイヤーの状態を初期化
	InitLayerData();
}

void UIManager::End()
{
	// UIリストをリセット
	ResetUIElements();

	// ロードされているすべてのUI画像リソースをメモリから解放する
	for (auto& resource : m_uiResources)
	{
		DeleteGraph(resource.second.handle);
	}
	// UI画像コンテナをクリア
	m_uiResources.clear();

	// フォントのハンドルを消去
	for (auto& fontHandle : m_fontHandles)
	{
		DeleteFontToHandle(fontHandle);
	}
}

void UIManager::Update()
{
	// UIの更新
	for (auto& element : m_pUIElements)
	{
		element->Update();
		// UIが存在しない時
		if (!element->IsAlive())
		{
			// 終了処理を行う
			element->End();
		}
	}

	// UIの中で存在しないものを探す
	m_pUIElements.remove_if([](std::shared_ptr<UIBase> element) 
		{
			// UIが存在しない判定の時に削除
			return !element->IsAlive();
		});
}

void UIManager::Draw()const
{
	for (int layer = 0; layer < static_cast<int>(UIBase::Layer::Max); ++layer)
	{
		// レイヤーが非アクティブ状態なら処理をしない
		if (!m_layers[layer].isActive) continue;

		// UIの描画
		for (auto& element : m_pUIElements)
		{
			// レイヤーが違う場合処理をしない
			if (static_cast<int>(element->GetLayer()) != layer) continue;

			// アルファブレンドを行うかどうか
			bool isAlphaBlend = m_layers[layer].alpha != kDefaultLayerAlpha;

			// アルファ値がデフォルトのものでない場合アルファブレンドを行う
			if (isAlphaBlend)
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_layers[layer].alpha);
			}

			// UIが表示状態のときのみ描画を行う
			if (element->IsActive())
			{
				element->Draw();
			}

			// ブレンドを行っている場合設定を元に戻す
			if (isAlphaBlend)
			{
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}

		}
	}
}

void UIManager::DrawLayer(UIBase::Layer start, UIBase::Layer end)
{
	for (int layer = static_cast<int>(start); layer < static_cast<int>(end); ++layer)
	{
		// レイヤーが非アクティブ状態なら処理をしない
		if (!m_layers[layer].isActive) continue;

		// UIの描画
		for (auto& element : m_pUIElements)
		{
			// レイヤーが違う場合処理をしない
			if (static_cast<int>(element->GetLayer()) != layer) continue;

			// アルファブレンドを行うかどうか
			bool isAlphaBlend = m_layers[layer].alpha != kDefaultLayerAlpha;

			// アルファ値がデフォルトのものでない場合アルファブレンドを行う
			if (isAlphaBlend)
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_layers[layer].alpha);
			}

			// UIが表示状態のときのみ描画を行う
			if (element->IsActive())
			{
				element->Draw();
			}

			// ブレンドを行っている場合設定を元に戻す
			if (isAlphaBlend)
			{
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}

		}
	}

}

void UIManager::ResetUIElements()
{
	// 残っているUIすべての終了処理を行う
	for (auto& element : m_pUIElements)
	{
		element->End();
	}
	// UIリストをクリア
	m_pUIElements.clear();
}

void UIManager::SetLayerActive(UIBase::Layer layer, bool isActive)
{
	m_layers[static_cast<int>(layer)].isActive = isActive;
}

void UIManager::SetLayerAlpha(UIBase::Layer layer, int alpha)
{
	m_layers[static_cast<int>(layer)].alpha = alpha;
}

void UIManager::LoadGraphHandle(const std::wstring& path)
{
	// 画像のパスがすでにロード済みの場合
	if (m_uiResources.contains(path))
	{
		// 画像の参照カウンタを増加
		m_uiResources[path].refCounter++;
	}
	else
	{
		// リソースの参照カウンタを1にする
		m_uiResources[path].refCounter = kResourceExist;
		// パスに対応した画像ハンドルを取得
		m_uiResources[path].handle = LoadGraph((kCommonPathName + path).c_str());
		// パスが見つからなかったらアサート
		assert(m_uiResources[path].handle >= 0 && "UI画像のパスが存在しません");
	}
}

int UIManager::DeleteGraphHandle(const std::wstring& path)
{
	// 指定されたパスの要素を返す
	auto it = m_uiResources.find(path);
	assert(it != m_uiResources.end() && "UI削除時にパスからハンドルの取得に失敗しました");

	// 参照カウンタを減らして、最後の参照だったら
	if (--it->second.refCounter <= 0)
	{
		// リソースのハンドルを開放
		DeleteGraph(it->second.handle);
		// コンテナからも削除
		m_uiResources.erase(it);
		return 0; // 削除成功
	}
	else
	{
		return kResourceExist; // 削除しないとする
	}

}

int UIManager::GetGraphHandle(const std::wstring& path)
{
	// 指定されたパスの要素を返す
	auto resource = m_uiResources.find(path);
	assert(resource != m_uiResources.end() && "ハンドル取得時にパスからハンドルの取得に失敗しました");
	// リソースのハンドルを取得
	return resource->second.handle;
}

std::weak_ptr<UIImage> UIManager::CreateImage(const Vector2Int& pos, const std::wstring& path, const std::wstring& framePath)
{
	auto ptr = std::make_shared<UIImage>();
	ptr->Init(); // 初期化処理
	ptr->SetPos(pos); // 座標をセット
	// 指定されたパスの要素を返す
	auto resource = m_uiResources.find(path);
	assert(resource != m_uiResources.end() && "UIImage生成時にパスからハンドルの取得に失敗しました");
	// 画像ハンドルをセット
	ptr->SetHandle(resource->second.handle);
	// 枠画像ハンドルがある場合
	if (!framePath.empty())
	{
		// 指定されたパスの要素を返す
		auto frameResource = m_uiResources.find(framePath);
		assert(frameResource != m_uiResources.end() && "UIImageの枠生成時にパスからハンドルの取得に失敗しました");
		// 枠画像のハンドルをセット
		ptr->SetFrameHandle(frameResource->second.handle);
	}
	// UIリストに追加
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIImage> UIManager::CreateImageNoPath(const Vector2Int& pos, const std::wstring& name, int handle, const std::wstring& framePath)
{
	auto ptr = std::make_shared<UIImage>();
	ptr->Init(); // 初期化処理
	ptr->SetPos(pos); // 座標をセット
	// 画像ハンドルをセット
	ptr->SetHandle(handle);
	// 枠画像ハンドルがある場合
	if (!framePath.empty())
	{
		// 指定されたパスの要素を返す
		auto frameResource = m_uiResources.find(framePath);
		assert(frameResource != m_uiResources.end() && "UIImageの枠生成時にパスからハンドルの取得に失敗しました");
		// 枠画像のハンドルをセット
		ptr->SetFrameHandle(frameResource->second.handle);
	}
	// UIリストに追加
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UIText> UIManager::CreateText(const Vector2Int& pos, const std::wstring& text, FontType type)
{
	auto ptr = std::make_shared<UIText>();
	ptr->Init(); // 初期化処理
	ptr->SetPos(pos); // 座標をセット
	ptr->SetHandle(m_fontHandles[static_cast<int>(type)]); // フォントのハンドルをセット
	ptr->SetText(text); // テキストの内容をセット

	// UIリストに追加
	m_pUIElements.push_back(ptr);
	return ptr;
}

std::weak_ptr<UISelectList> UIManager::CreateSelectList(const Vector2Int& pos, const Vector2Int& size, FontType type, const std::wstring& backGroundPath)
{
	auto ptr = std::make_shared<UISelectList>();
	ptr->Init(); // 初期化処理
	ptr->SetPos(pos); // 座標をセット
	ptr->SetSize(size); // リスト全体のサイズをセット
	ptr->SetFontHandle(m_fontHandles[static_cast<int>(type)]); // フォントのハンドルをセット

	// 背景画像ハンドルがある場合
	if (!backGroundPath.empty())
	{
		// 指定されたパスの要素を返す
		auto frameResource = m_uiResources.find(backGroundPath);
		assert(frameResource != m_uiResources.end() && "UISelectListの背景画像生成時にパスからハンドルの取得に失敗しました");
		// 背景画像のハンドルをセット
		ptr->SetBackGroundHandle(frameResource->second.handle);
	}

	// UIリストに追加
	m_pUIElements.push_back(ptr);
	return ptr;
}

void UIManager::LoadAllFontHandle()
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

void UIManager::InitLayerData()
{
	// すべてのレイヤーのデータを確保
	m_layers.resize(static_cast<int>(UIBase::Layer::Max));

	// レイヤーの状態を初期化
	for (int i = 0; i < static_cast<int>(UIBase::Layer::Max); i++)
	{
		m_layers[i].isActive = true;
		m_layers[i].alpha = kDefaultLayerAlpha;
	}
}
