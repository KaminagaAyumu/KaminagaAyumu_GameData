#include "UIFormatText.h"
#include "../StringFunction.h"
#include <algorithm>
#include "DxLib.h"

namespace
{
	constexpr int kDefaultColor = 0xffffff; // デフォルトの文字色(白)

	constexpr int kMinTypeFrame = 1;

	constexpr int kMaxFadeAlpha = 255;
}

UIFormatText::UIFormatText() :
	m_pos{},
	m_fontHandle(-1),
	m_color(kDefaultColor),
	m_frameCount(0),
	m_isAlive(true),
	m_isCenter(false),
	m_isFadeOut(false),
	m_visibleTextNum(-1), // 初期状態は-1
	m_typeFrame(0),
	m_fadeWaitFrame(0),
	m_fadeDuration(0),
	m_fadeAlpha(kMaxFadeAlpha),
	m_fadeCount(0),
	m_fadeState(FadeState::None)
{
}

UIFormatText::~UIFormatText()
{
}

void UIFormatText::Init(int handle, std::string text, const Position2& pos)
{
	m_fontHandle = handle;
	m_text = text;
	m_pos = pos;
	m_wText = StringFunction::WStringFromString(m_text); // テキストをワイド文字列にしたものを取得
}

void UIFormatText::Update()
{
	m_frameCount++;

	// プロバイダがない場合処理を行わない
	if (!m_provider)
	{
		UpdateTypewriter();
		return;
	}
	// プロバイダをセット
	CheckProvider();
	UpdateTypewriter();

	// フェードアウトする場合のみフェードの更新を行う
	if (m_isFadeOut)
	{
		UpdateFade();
	}

}

void UIFormatText::Draw() const
{
	const int nowTextNum = std::clamp(m_visibleTextNum, 0, static_cast<int>(m_wText.size()));
	std::wstring visibleText; // 描画するテキスト(現在見えているテキスト)
	int drawLength = 0; // 現在のテキストの大きさ
	if (m_visibleTextNum < 0)
	{
		visibleText = m_wText;
		drawLength = static_cast<int>(m_wText.size());
	}
	else
	{
		visibleText = std::wstring(m_wText.begin(), m_wText.begin() + nowTextNum);
		drawLength = nowTextNum;
	}

	// テキストの幅を取得
	const int width = GetDrawStringWidthToHandle(visibleText.c_str(), drawLength, m_fontHandle);
	const int fontSize = GetFontSizeToHandle(m_fontHandle);

	int x = static_cast<int>(m_pos.x);
	int y = static_cast<int>(m_pos.y);
	// 中央ぞろえの場合はx座標をテキストの中心にする
	if (m_isCenter)
	{
		x -= width / 2;
		y -= fontSize;
	}

	// 透明度が最大値でない場合フェードが進んでいるとする
	if (m_fadeAlpha < kMaxFadeAlpha)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
		DrawStringToHandle(x, y, visibleText.c_str(), m_color, m_fontHandle);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else // フェードが進んでいない場合通常の表示をする
	{
		DrawStringToHandle(x, y, visibleText.c_str(), m_color, m_fontHandle);
	}
}

bool UIFormatText::IsAlive() const
{
	return m_isAlive;
}

void UIFormatText::Close()
{
	m_isAlive = false;
}

void UIFormatText::SetProvider(std::function<std::string()> provider)
{
	// プロバイダをセット
	m_provider = std::move(provider);
	// セットと同時に実際のテキストの内容も変更
	CheckProvider();
}

void UIFormatText::SetCenter()
{
	m_isCenter = true;
}

void UIFormatText::ShowTypewriter(int typeFrame, bool isFadeOut)
{
	// 1文字を表示するフレームを取得
	// 最低でも1以上にする
	m_typeFrame = max(kMinTypeFrame, typeFrame);

	m_visibleTextNum = 0; // 表示されている文字数をセット

	m_frameCount = 0; // フレームカウンタをリセット

}

void UIFormatText::SetFadeOut(int fadeWaitFrame, int fadeDuration)
{
	m_isFadeOut = true;
	m_fadeWaitFrame = fadeWaitFrame;
	m_fadeDuration = fadeDuration;
}

void UIFormatText::CheckProvider()
{
	// プロバイダがセットされていない場合処理を抜ける
	if (!m_provider) return;
	
	// 現在のテキストの内容をプロバイダからチェック
	const std::string now = m_provider();
	
	// 現在のテキストのデータがプロバイダのテキストと一致していない場合
	if (now != m_text)
	{
		// テキストを変更
		m_text = now;
		m_wText = StringFunction::WStringFromString(m_text);
	}
}

void UIFormatText::UpdateTypewriter()
{
	// 文字を出すフレームが設定されていなければ以下の処理を行わない
	if (m_typeFrame <= 0)
	{
		return;
	}

	// 最大文字数を取得
	const int maxTextNum = static_cast<int>(m_wText.size());
	// 文字がすべて表示されたらそれ以上更新しない
	if (m_visibleTextNum >= maxTextNum)
	{
		return;
	}

	int addNum = m_frameCount / m_typeFrame;
	if (addNum > m_visibleTextNum)
	{
		m_visibleTextNum = min(addNum, maxTextNum);
	}

}

void UIFormatText::UpdateFade()
{
	// 最大文字数を取得
	const int maxTextNum = static_cast<int>(m_wText.size());

	if (m_visibleTextNum >= maxTextNum)
	{
		switch (m_fadeState)
		{
		case FadeState::None:
			m_fadeState = (m_fadeWaitFrame > 0) ? FadeState::Waiting : (m_fadeDuration > 0 ? FadeState::Fading : FadeState::Done);
			m_fadeCount = 0;
			break;
		case FadeState::Waiting:
			if (m_fadeCount++ >= m_fadeWaitFrame)
			{
				m_fadeState = (m_fadeDuration > 0) ? FadeState::Fading : FadeState::Done;
				m_fadeCount = 0;
			}
			break;
		case FadeState::Fading:
		{
			m_fadeCount++;
			const float t = (m_fadeDuration > 0) ? min(1.0f, static_cast<float>(m_fadeCount) / static_cast<float>(m_fadeDuration)) : 1.0f;
			m_fadeAlpha = static_cast<int>(kMaxFadeAlpha * (1.0f - t));
			if (m_fadeCount >= m_fadeDuration)
			{
				m_fadeAlpha = 0;
				m_fadeState = FadeState::Done;
				m_isAlive = false;
			}
			break;
		}
		case FadeState::Done:
			m_fadeAlpha = 0;
			break;
		}
	}
	else
	{
		m_fadeAlpha = kMaxFadeAlpha;
	}
}
