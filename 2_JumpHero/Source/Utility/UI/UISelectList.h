#pragma once
#include "UIBase.h"
#include "../Geometry.h"
#include <vector>
#include <string>
#include <functional>

class UISelectList : public UIBase
{
public:
	/// <summary>
	/// 選択肢の内容
	/// </summary>
	struct OptionItem
	{
		std::string text; // 表示するテキストの内容
		std::function<void()> onSelect; // 決定されたときに行う処理
	};

	UISelectList();
	virtual ~UISelectList();

	/// <summary>
	/// リスト全体の初期化処理
	/// </summary>
	/// <param name="fontHandle">フォントのハンドル</param>
	/// <param name="size">リスト全体の大きさ</param>
	/// <param name="pos">リスト全体からの中心座標</param>
	void Init(int fontHandle, const Size& size, const Position2& pos);

	// リスト全体に直接関係のない変数を変更できる処理を追加したい(オプションごとに変わるようにしたい)

	/// <summary>
	/// 選択肢を追加する
	/// </summary>
	/// <param name="text">選択肢の名前</param>
	/// <param name="onSelect">選ばれた際の処理</param>
	void AddOption(const std::string& text, std::function<void()> onSelect);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()const override;

	/// <summary>
	/// 存在しているかどうかを返す
	/// </summary>
	/// <returns>true : 存在している false : 存在しない</returns>
	bool IsAlive()const override;

	/// <summary>
	/// 画像のハンドルを取得する
	/// </summary>
	/// <param name="frameHandle"></param>
	/// <param name="fillHandle"></param>
	void SetHandle(int frameHandle, int fillHandle);

	/// <summary>
	/// 上下方向にカーソルを動かす(値の数分動かす)
	/// </summary>
	/// <param name="dir">どの方向に動かすか(正の数で下に移動,負の数で上に移動)</param>
	void MoveCursor(int dir);

	/// <summary>
	/// 選択された時の処理
	/// </summary>
	void TriggerSelect();

	/// <summary>
	/// 現在のカーソルの位置を返す
	/// </summary>
	/// <returns>カーソルの位置</returns>
	int GetCursor() const { return m_cursor; }

	/// <summary>
	/// ダイアログモードでセレクトリストを使用する
	/// </summary>
	/// <param name="text">見出しの内容</param>
	void SetDialogMode(const std::string& text);

private:
	Position2 m_pos; // 表示座標(リスト全体の中心)
	Size m_size; // リスト全体のサイズ
	std::vector<OptionItem> m_items; // 選択肢の内容をまとめる
	int m_fontHandle; // フォントのハンドル
	int m_color; // 文字の色
	int m_itemSpacing; // 縦余白

	int m_frameCount; // 表示してからのフレーム数

	int m_cursor; // カーソルがどこにいるか

	int m_frameHandle; // 枠画像のハンドル
	int m_fillHandle; // 背景画像のハンドル

	/// <summary>
	/// 文字の描画オフセットを判別するための列挙体
	/// </summary>
	enum class OffsetPolicy
	{
		Left, // 左詰め
		Center, // 中央ぞろえ
	};

	int m_paddingX; // 左右の余白を設定する際に使う
	OffsetPolicy m_policy; // 描画オフセットを判別するために使う


	std::string m_dialogTitle; // ダイアログの見出しのテキスト

	bool m_isDialogMode; // ダイアログとして使うかどうか
};

