#include "UIBase.h"

UIBase::UIBase() : 
	m_layer(Layer::Game),
	m_fadeState(FadeState::Normal),
	m_uiState(UIState::Normal),
	m_fadeFrame(0),
	m_frameCount(0),
	m_blinkFrame(0),
	m_appearFrame(0),
	m_uiUpdateFrameCount(0),
	m_isActive(true),
	m_isAfterDelete(true)
{
}

void UIBase::StartFadeIn(int fadeFrame)
{
	m_fadeState = FadeState::FadeIn;
	m_fadeFrame = fadeFrame;
	m_frameCount = 0;
}

void UIBase::StartFadeOut(int fadeFrame, bool isAfterDelete)
{
	m_fadeState = FadeState::FadeOut;
	m_fadeFrame = fadeFrame;
	m_frameCount = 0;
	m_isAfterDelete = isAfterDelete;
}

bool UIBase::IsFade() const
{
	// 通常の状態でないならフェード状態
	return m_fadeState != FadeState::Normal;
}

bool UIBase::IsMoving() const
{
	// 開いているか閉じている途中なら動いている状態
	return m_uiState == UIState::AppearCenter || m_uiState == UIState::CloseCenter;
}

void UIBase::StartBlinking(int blinkFrame)
{
	m_uiState = UIState::Blinking;
	m_blinkFrame = blinkFrame;
	m_uiUpdateFrameCount = 0;
}

void UIBase::StartAppearCenter(int appearFrame)
{
	m_uiState = UIState::AppearCenter;
	m_appearFrame = appearFrame;
	m_uiUpdateFrameCount = 0;
}

void UIBase::StartCloseCenter(int closeFrame, bool isAfterDelete)
{
	m_uiState = UIState::CloseCenter;
	m_appearFrame = closeFrame;
	m_uiUpdateFrameCount = 0;
	m_isAfterDelete = isAfterDelete;
}
