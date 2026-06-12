#pragma once
#include <string>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include "../../Geometry/Vector2Int.h"
#include "UIBase.h"

class UIImage;
class UIText;
class UISelectList;

/// <summary>
/// 使用するフォントのサイズ
/// </summary>
enum class FontType : int
{
	Small, // 小サイズ
	Midium, // 中サイズ
	Large, // 大きいサイズ
	Header, // 見出しサイズ
	Default // 通常サイズ
};

/// <summary>
/// UI管理用クラス
/// 注意:更新処理はアプリケーションが行っているが、
/// 描画処理はシーンごとに呼ばなければならない
/// </summary>
class UIManager
{
public:
	virtual ~UIManager(); // デストラクタ

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static UIManager& GetInstance();

	void Init();
	void End();
	void Update();
	void Draw()const;
	/// <summary>
	/// レイヤーの描画する範囲を指定して描画する
	/// </summary>
	/// <param name="start">開始レイヤー</param>
	/// <param name="end">終了レイヤー</param>
	void DrawLayer(UIBase::Layer start, UIBase::Layer end);

	/// <summary>
	/// UIの要素をリセットする
	/// </summary>
	void ResetUIElements();

	/// <summary>
	/// レイヤーごとのアクティブ状態をセットする
	/// </summary>
	/// <param name="layer">指定レイヤー</param>
	/// <param name="isActive">アクティブ状態 true : アクティブ false : 非アクティブ</param>
	void SetLayerActive(UIBase::Layer layer, bool isActive);

	/// <summary>
	/// レイヤーごとのアルファ値をセットする
	/// </summary>
	/// <param name="layer">指定レイヤー</param>
	/// <param name="alpha">アルファ値</param>
	void SetLayerAlpha(UIBase::Layer layer, int alpha);

	/// <summary>
	/// 画像情報をロード
	/// </summary>
	/// <param name="path">ハンドルのパス</param>
	void LoadGraphHandle(const std::wstring& path);

	/// <summary>
	/// 画像情報をメモリから解放
	/// </summary>
	/// <param name="path">ハンドルのパス</param>
	/// <returns>0:成功 1:まだ参照先があるのでリソースの削除はしない -1:削除に失敗</returns>
	int DeleteGraphHandle(const std::wstring& path);

	/// <summary>
	/// 画像ハンドルをパスから取得する
	/// </summary>
	/// <param name="path">ハンドルのパス</param>
	/// <returns>画像のハンドル</returns>
	int GetGraphHandle(const std::wstring& path);

	/// <summary>
	/// Imageを生成
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="path">画像ハンドルのパス</param>
	/// <returns>生成したUI</returns>
	std::weak_ptr<UIImage> CreateImage(const Vector2Int& pos, const std::wstring& path, const std::wstring& framePath = L"");

	/// <summary>
	/// Imageをハンドルを指定したうえで作成
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="name">画像の名前</param>
	/// <param name="handle">画像ハンドル</param>
	/// <returns>生成したUI</returns>
	std::weak_ptr<UIImage> CreateImageNoPath(const Vector2Int& pos, const std::wstring& name, int handle, const std::wstring& framePath = L"");

	/// <summary>
	/// テキストを生成
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="text">テキストの内容</param>
	/// <param name="type">フォントのタイプ</param>
	/// <returns>生成したテキスト</returns>
	std::weak_ptr<UIText> CreateText(const Vector2Int& pos, const std::wstring& text, FontType type);

	/// <summary>
	/// 選択できるリストを生成
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="size">リスト全体のサイズ</param>
	/// <param name="type">フォントのタイプ</param>
	/// <param name="backGroundPath">背景画像のパス</param>
	/// <returns>生成した選択リスト</returns>
	std::weak_ptr<UISelectList> CreateSelectList(const Vector2Int& pos, const Vector2Int& size, FontType type, const std::wstring& backGroundPath = L"");


private:
	UIManager(); // コンストラクタをprivateで宣言
	UIManager(const UIManager&) = delete; // コピーコンストラクタを作れないようにする
	void operator=(const UIManager&) = delete; // 代入演算子も使えないようにする

	/// <summary>
	/// 画像のリソース情報
	/// </summary>
	struct UIResourceInfo
	{
		int refCounter; // 参照カウンタ
		int handle; // 画像ハンドル
	};

	/// <summary>
	/// レイヤーごとの状態
	/// </summary>
	struct LayerData
	{
		bool isActive;	// 表示状態
		int alpha;		// アルファブレンド値
	};

	// 画像のリソースを管理するmap(文字列は画像のパス)
	std::map<std::wstring, UIResourceInfo> m_uiResources;

	// UIを管理するlist
	std::list<std::shared_ptr<UIBase>> m_pUIElements;

	// フォントのハンドルを管理するvector
	std::vector<int> m_fontHandles;

	// レイヤーの状態を管理するvector
	std::vector<LayerData> m_layers;

private:
	// すべてのフォントハンドルをロード
	void LoadAllFontHandle();

	/// <summary>
	/// レイヤーの状態を初期化
	/// </summary>
	void InitLayerData();

};

