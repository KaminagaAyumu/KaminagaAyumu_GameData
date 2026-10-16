#pragma once
#include <memory>
#include <list>
#include <vector>
#include "../Geometry.h"
#include "../GameType.h"
#include "../../Game/TextManager.h"

class Map;
class UIBase;
class UITextWindow;
class UIText;
class UIFormatText;
class UISelectList;
class UIGauge;
class UIImage;

/// <summary>
/// UI関連の管理用クラス
/// </summary>
class UIManager
{
public:
	UIManager();
	virtual ~UIManager();

	void Init();
	void Update();
	void Draw();

	/// <summary>
	/// テキストウィンドウを作成する関数
	/// </summary>
	/// <param name="text"></param>
	/// <param name="size"></param>
	/// <param name="pos"></param>
	/// <param name="fontType">フォントのタイプ</param>
	/// <returns></returns>
	std::weak_ptr<UITextWindow> CreateTextWindow(const std::string& text, const Size& size, const Position2& pos, Types::FontType fontType);

	/// <summary>
	/// ページ付きのテキストウィンドウを作成する関数
	/// </summary>
	/// <param name="id"></param>
	/// <param name="pages"></param>
	/// <param name="size"></param>
	/// <param name="pos"></param>
	/// <param name="appearDuration"></param>
	/// <param name="fontType">フォントのタイプ</param>
	/// <returns></returns>
	std::weak_ptr<UITextWindow> CreateTextWindowPaged(const std::string& id, const std::vector<TextData>& pages, const Size& size, const Position2& pos, float appearDuration, Types::FontType fontType);

	/// <summary>
	/// テキストを作成する関数
	/// </summary>
	/// <param name="fontType">フォントのタイプ</param>
	/// <param name="text">テキストの内容</param>
	/// <param name="pos">表示する中心座標</param>
	/// <returns></returns>
	std::weak_ptr<UIText> CreateText(Types::FontType fontType, const std::string& text, const Position2& pos);

	/// <summary>
	/// フォーマットを指定できるテキストを作成する関数
	/// </summary>
	/// <param name="fontType">フォントのタイプ</param>
	/// <param name="text">テキストの内容</param>
	/// <param name="pos">表示する中心座標</param>
	/// <returns></returns>
	std::weak_ptr<UIFormatText> CreateFormatText(Types::FontType fontType, const std::string& text, const Position2& pos);

	/// <summary>
	/// 選択できるリストを作成する関数
	/// </summary>
	/// <param name="fontType">フォントのタイプ</param>
	/// <param name="size">リスト全体のサイズ</param>
	/// <param name="pos">表示する中心座標</param>
	/// <returns></returns>
	std::weak_ptr<UISelectList> CreateSelectList(Types::FontType fontType, const Size& size, const Position2& pos);

	/// <summary>
	/// ゲージを作成する関数
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<UIGauge> CreateGauge(const Size& size, const Position2& pos, int gaugeNo);

	/// <summary>
	/// 画像を作成する関数
	/// </summary>
	/// <param name="type"></param>
	/// <param name="size"></param>
	/// <param name="pos"></param>
	/// <returns></returns>
	std::weak_ptr<UIImage> CreateImage(Types::ImageType type, const Size& size, const Position2& pos);

private:
	std::vector<int> m_fontHandles; // フォントハンドルを格納する配列
	std::vector<int> m_imageHandles; // イメージ画像データを格納する配列
	
	int m_windowGraphHandle; // ウィンドウの画像ハンドル
	int m_windowFrameHandle; // ウィンドウの枠ハンドル

	int m_gaugeFrameHandle; // ゲージの枠ハンドル
	int m_gaugeFillHandle; // ゲージの中身ハンドル

	int m_gaugeYellowFrameHandle; // 黄色ゲージの枠ハンドル
	int m_gaugeYellowFillHandle; // 黄色ゲージの中身ハンドル

	int m_selectListFrameHandle; // セレクトリストの枠ハンドル
	int m_selectListFillHandle; // セレクトリストの画像ハンドル

	std::list<std::shared_ptr<UIBase>> m_pUIElements; // UIの要素を格納するコンテナ

	/// <summary>
	/// フォントをロードする関数(初期化時に呼ぶ)
	/// </summary>
	void LoadFonts();

	/// <summary>
	/// 画像データをロードする(初期化時に呼ぶ)
	/// </summary>
	void LoadImages();

};

