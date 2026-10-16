#include "Drawable2D.h"

MyLib::Drawable2D::Drawable2D(DrawLayer layer) : 
	Drawable(layer),
	m_uiLayer(UILayer::Normal),
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

void MyLib::Drawable2D::StartFadeIn(int fadeFrame)
{
	m_fadeState = FadeState::FadeIn;
	m_fadeFrame = fadeFrame;
	m_frameCount = 0;
}

void MyLib::Drawable2D::StartFadeOut(int fadeFrame, bool isAfterDelete)
{
	m_fadeState = FadeState::FadeOut;
	m_fadeFrame = fadeFrame;
	m_frameCount = 0;
	m_isAfterDelete = isAfterDelete;
}

bool MyLib::Drawable2D::IsFade() const
{
	// 通常の状態でないならフェード状態
	return m_fadeState != FadeState::Normal;
}

bool MyLib::Drawable2D::IsMoving() const
{
	// 開いているか閉じている途中なら動いている状態
	return m_uiState == UIState::AppearCenter || m_uiState == UIState::CloseCenter;
}

float MyLib::Drawable2D::GetBlinkAlphaRate() const
{
	if (m_uiState != UIState::Blinking || m_blinkFrame <= 0)
	{
		return 1.0f;
	}

	const float t = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_blinkFrame);

	const float wave = sinf(t * DX_TWO_PI_F);
	return 0.5f + wave * 0.5f;
}

void MyLib::Drawable2D::StartBlinking(int blinkFrame)
{
	m_uiState = UIState::Blinking;
	m_blinkFrame = blinkFrame;
	m_uiUpdateFrameCount = 0;
}

void MyLib::Drawable2D::StopBlinking()
{
	if (m_uiState == UIState::Blinking)
	{
		m_uiState = UIState::Normal;
	}
}

void MyLib::Drawable2D::StartBlinkOut(int blinkFrame, bool isAfterDelete)
{
	m_uiState = UIState::BlinkOut;
	m_blinkFrame = blinkFrame;
	m_uiUpdateFrameCount = 0;
}

void MyLib::Drawable2D::StartAppearCenter(int appearFrame)
{
	m_uiState = UIState::AppearCenter;
	m_appearFrame = appearFrame;
	m_uiUpdateFrameCount = 0;
}

void MyLib::Drawable2D::StartCloseCenter(int closeFrame, bool isAfterDelete)
{
	m_uiState = UIState::CloseCenter;
	m_appearFrame = closeFrame;
	m_uiUpdateFrameCount = 0;
	m_isAfterDelete = isAfterDelete;
}

bool MyLib::Drawable2D::UpdateFade()
{
	if (!IsFade()) return false;

	m_frameCount++;
	if (m_frameCount >= m_fadeFrame)
	{
		if (m_fadeState == FadeState::FadeOut)
		{
			m_isActive = false;
			return true;
		}
		m_fadeState = FadeState::Normal;
	}


	return false;
}

float MyLib::Drawable2D::GetFadeAlphaRate() const
{
	if (!IsFade() || m_fadeFrame <= 0) return 1.0f;
	const float rate = static_cast<float>(m_frameCount) / static_cast<float>(m_fadeFrame);
	
	return (m_fadeState == FadeState::FadeIn) ? rate : (1.0f - rate);
}

bool MyLib::Drawable2D::UpdateAppearState()
{
	if (m_uiState != UIState::AppearCenter && m_uiState != UIState::CloseCenter)
	{
		return false;
	}
	if (m_appearFrame <= 0)
	{
		m_uiState = UIState::Normal;
		return false;
	}

	const float rate = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_appearFrame);
	if (rate < 1.0f) return false;

	const bool wasClosing = (m_uiState == UIState::CloseCenter);
	m_uiState = UIState::Normal;

	if (wasClosing)
	{
		m_isActive = false;
		return true;
	}
	return false;
}

float MyLib::Drawable2D::GetAppearRate() const
{
	if (m_uiState != UIState::AppearCenter && m_uiState != UIState::CloseCenter)
	{
		return 1.0f;
	}

	if (m_appearFrame <= 0) return 1.0f;

	const float rate = std::min(static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_appearFrame), 1.0f);
	return (m_uiState == UIState::AppearCenter) ? rate : (1.0f - rate);
}
