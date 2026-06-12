#pragma once
#include "UIBase.h"
#include "../../Geometry/Vector2Int.h"
#include "../../Geometry/Vector2.h"
#include <vector>
#include <string>
#include <functional>


class UISelectList : public UIBase
{
public:
	UISelectList();
	virtual ~UISelectList();

	void Init()override;
	void End()override;
	void Update()override;
	void Draw()const override;

	bool IsAlive()const override;

	/// <summary>
	/// 選択肢の内容
	/// </summary>
	struct OptionItem
	{
		std::wstring text; // 表示するテキストの内容
		std::function<void()> onSelect; // 決定されたときに行う処理
	};

	/// <summary>
	/// フォントのハンドルをセットする
	/// </summary>
	/// <param name="handle">フォントのハンドル</param>
	void SetFontHandle(int handle) { m_fontHandle = handle; }

	/// <summary>
	/// 背景にする画像ハンドルをセットする
	/// </summary>
	/// <param name="handle">画像ハンドル</param>
	void SetBackGroundHandle(int handle) { m_bgHandle = handle; }

	/// <summary>
	/// 表示する座標をセット
	/// </summary>
	/// <param name="pos">2D座標</param>
	void SetPos(const Vector2Int& pos) { m_pos = pos; }

	/// <summary>
	/// リスト全体のサイズをセット
	/// </summary>
	/// <param name="size">リスト全体のサイズ</param>
	void SetSize(const Vector2Int& size);

	/// <summary>
	/// ダイアログとして表示する際の設定をする
	/// </summary>
	/// <param name="title">ダイアログの見出し</param>
	void SetDialog(const std::wstring title);

	/// <summary>
	/// はいといいえを表示するダイアログを作成する
	/// </summary>
	/// <param name="title">ダイアログの見出し</param>
	/// <param name="yesFunc">はいが選択された際の処理</param>
	/// <param name="noFunc">いいえが選択された際の処理</param>
	void SetYesNoDialog(const std::wstring title, std::function<void()> yesFunc, std::function<void()> noFunc);

	/// <summary>
	/// 選択肢の内容を追加する
	/// </summary>
	/// <param name="text">表示するテキスト</param>
	/// <param name="onSelect">選択された際に呼ばれる内容</param>
	void AddOption(const std::wstring text, std::function<void()> onSelect);

	/// <summary>
	/// カーソルを移動させる
	/// </summary>
	/// <param name="dir">移動量</param>
	void MoveCursor(int dir);

	/// <summary>
	/// 指定したテキストの場所にカーソルがあるかどうか
	/// </summary>
	/// <param name="text">表示しているテキスト</param>
	/// <returns>true : 選択中 false : 選択していない</returns>
	bool IsMatchedCursor(const std::wstring text);

	/// <summary>
	/// 選択した際の処理
	/// </summary>
	void TriggerSelect();

	void StartAppearCenter(int appearFrame);

	/// <summary>
	/// 文字の寄せかた
	/// </summary>
	enum class AlignmentType
	{
		Left,
		Right,
		Center
	};

private:
	// フォントのハンドル
	int m_fontHandle;
	// 背景の画像ハンドル
	int m_bgHandle;
	// 表示座標(リスト全体の中心)
	Vector2Int m_pos;
	// 表示サイズ(リスト全体)
	Vector2Int m_size;
	// 表示サイズ(出現、消える際に使う)
	Vector2Int m_targetSize;
	// 選択肢の内容をまとめるvector
	std::vector<OptionItem> m_items;

	// ダイアログ状態の際に表示する見出しテキスト
	std::wstring m_dialogTitle;

	// テキストが動く際のフレームカウンタ
	int m_textMoveFrameCount;

	// 現在の選択位置
	int m_cursor;

	// 存在状態
	bool m_isAlive;

	// 文字の寄せかた
	AlignmentType m_alignmentType;

	// 描画処理用の関数ポインタを定義
	using DrawFunc_t = void(UISelectList::*)() const;
	DrawFunc_t m_draw;

private:
	/// <summary>
	/// 通常の描画処理
	/// </summary>
	void NormalDraw() const;

	/// <summary>
	/// ダイアログ状態の描画処理
	/// </summary>
	void DialogDraw() const;

};

