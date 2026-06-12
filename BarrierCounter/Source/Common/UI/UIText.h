#pragma once
#include "UIBase.h"
#include "../../Geometry/Vector2Int.h"
#include "../../Geometry/Vector2.h"
#include <string>

class UIText : public UIBase
{
public:
	UIText();
	virtual ~UIText();

	void Init()override;
	void End()override;
	void Update()override;
	void Draw()const override;

	bool IsAlive()const override;

	/// <summary>
	/// フォントのハンドルをセットする
	/// </summary>
	/// <param name="handle">フォントのハンドル</param>
	void SetHandle(int handle) { m_handle = handle; }

	/// <summary>
	/// テキストの内容をセットする
	/// </summary>
	/// <param name="text">表示したいテキスト</param>
	void SetText(std::wstring text) { m_text = text; }

	/// <summary>
	/// テキストの色をセットする
	/// </summary>
	/// <param name="color"></param>
	void SetTextColor(unsigned int color) { m_textColor = color; }

	/// <summary>
	/// 表示する座標をセット
	/// </summary>
	/// <param name="pos">2D座標</param>
	void SetPos(const Vector2Int& pos) { m_pos = pos; }

	/// <summary>
	/// 文字の寄せかた
	/// </summary>
	enum class AlignmentType
	{
		Left,
		Right,
		Center
	};

	/// <summary>
	/// 文字の寄せかたを設定
	/// </summary>
	/// <param name="type"></param>
	void SetAlignment(AlignmentType type);

private:
	// フォントのハンドル
	int m_handle;
	// 影を含めたテキストを画像にする際のハンドル
	int m_graphHandle;
	// 文字の色
	unsigned int m_textColor;
	// 表示する文字
	std::wstring m_text;
	// 存在フラグ
	bool m_isAlive;
	// 表示座標(文字の中心)
	Vector2Int m_pos;
	// 文字の寄せかたを取得
	AlignmentType m_alignmentType;

private:

	// 画像ハンドルにテキストを描画
	void DrawGraphHandle();

};

