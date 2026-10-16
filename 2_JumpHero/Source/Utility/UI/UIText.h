#pragma once
#include "UIBase.h"
#include <string>
#include "../Geometry.h"

/// <summary>
/// テキストのみを表示するクラス
/// </summary>
class UIText : public UIBase
{
public:
	UIText();
	virtual ~UIText();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="handle">テキストのハンドル</param>
	/// <param name="text">表示するテキストの内容</param>
	/// <param name="pos">表示する座標</param>
	void Init(int handle, std::string text, const Position2& pos);
	void Update()override;
	void Draw()const override;
	bool IsAlive()const override;

	/// <summary>
	/// 文字の色を変更する
	/// </summary>
	/// <param name="color"></param>
	void SetColor(int color);

	/// <summary>
	/// テキストを点滅させて表示するように設定する
	/// </summary>
	void SetBlinking();

private:
	Position2 m_pos; // 表示座標
	std::string m_text; // 表示するテキスト
	int m_fontHandle; // フォントのハンドル
	int m_color; // 文字の色
	int m_frameCount; // 表示してからのフレーム数
	int m_alpha; // 文字の透明度
	bool m_isAlive; // 存在フラグ
	bool m_isBlinking; // 点滅するかどうか

};

