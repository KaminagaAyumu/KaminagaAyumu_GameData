#include "UIText.h"
#include "../StringFunction.h"
#include "DxLib.h"

namespace
{
	constexpr int kDefaultColor = 0xffffff; // デフォルトの文字色(白)
	constexpr int kMaxAlpha = 255;
	constexpr float kBlinkCycle = 60.0f;

	constexpr int kAliveTime = 600; // テキストが表示されている時間
}

UIText::UIText() : 
	m_pos{},
	m_fontHandle(-1),
	m_color(kDefaultColor),
	m_frameCount(0),
	m_alpha(kMaxAlpha),
	m_isAlive(true),
	m_isBlinking(false)
{
}

UIText::~UIText()
{
}

void UIText::Init(int handle, std::string text, const Position2& pos)
{
	m_fontHandle = handle;
	m_text = text;
	m_pos = pos;
}

void UIText::Update()
{
	m_frameCount++;
}

void UIText::Draw() const
{
	// テキストの内容をワイド文字列に変換
	auto text = StringFunction::WStringFromString(m_text);
	// テキストの幅を取得
	int width = GetDrawStringWidthToHandle(text.c_str(), static_cast<int>(m_text.length()), m_fontHandle);
	// テキストを中央揃えにするためにX座標を調整
	int adjustedX = static_cast<int>(m_pos.x) - width / 2;
	

	// テキストを描画
	if (m_isBlinking) // 点滅する場合
	{
		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
		auto sinRate = 1.0f - sinf(m_frameCount / (kBlinkCycle) * DX_PI_F * 2);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxAlpha * sinRate));
		DrawStringToHandle(adjustedX, static_cast<int>(m_pos.y), text.c_str(), m_color, m_fontHandle);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else // 点滅しない場合は通常の表示
	{
		DrawStringToHandle(adjustedX, static_cast<int>(m_pos.y), text.c_str(), m_color, m_fontHandle);
	}
}

bool UIText::IsAlive() const
{
	return m_isAlive;
}

void UIText::SetColor(int color)
{
	m_color = color;
}

void UIText::SetBlinking()
{
	m_isBlinking = true;
}
