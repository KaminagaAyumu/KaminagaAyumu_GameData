#include "UITelop.h"
#include "../../../GameObject.h"
#include "../../../Renderer.h"
#include "../../../../Utility/Game.h"
#include <cassert>

namespace
{
	// 帯の初期設定
	constexpr unsigned int kDefaultBandColor = 0x3300aa;	// 帯の色
	constexpr int kDefaultBandHeight = 80;	// 帯の高さ

	// 演出の初期設定
	constexpr int kDefaultSlideFrame = 20;	// スライドイン・スライドアウトにかけるフレーム数
	constexpr int kDefaultOvershootFrame = 8;	// 行き過ぎた位置から戻るまでのフレーム数
	constexpr int kDefaultHoldFrame = 90;	// 中央で止まっているフレーム数
	constexpr float kDefaultOvershootAmount = 30.0f;	// 中央から行き過ぎる量

	constexpr unsigned int kTextColor = 0xffffff;	// 文字色
}

MyLib::UITelop::UITelop(DrawLayer layer) :
	Drawable2D(layer),
	m_fontHandle(-1),
	m_bandColor(kDefaultBandColor),
	m_bandHeight(kDefaultBandHeight),
	m_text(L""),
	m_phase(Phase::Idle),
	m_phaseFrameCount(0),
	m_slideFrame(kDefaultSlideFrame),
	m_overshootFrame(kDefaultOvershootFrame),
	m_holdFrame(kDefaultHoldFrame),
	m_overshootAmount(kDefaultOvershootAmount),
	m_textOffsetX(0.0f),
	m_isAlive(true)
{
}

void MyLib::UITelop::Init(std::weak_ptr<GameObject> parent)
{
	std::shared_ptr<GameObject> pParent = parent.lock();
	m_pTransform = pParent->GetComponent<Transform>();
	if (!m_pTransform.lock())
	{
		assert(false && "UITelop : Transformコンポーネントがありません");
	}
	Renderer::GetInstance().Entry(shared_from_this());
}

void MyLib::UITelop::Start()
{
}

void MyLib::UITelop::Update()
{
	switch (m_phase)
	{
	case MyLib::UITelop::Phase::Idle:
		break;
	case MyLib::UITelop::Phase::SlideIn:
		UpdateSlideIn();
		break;
	case MyLib::UITelop::Phase::Overshoot:
		UpdateOvershoot();
		break;
	case MyLib::UITelop::Phase::Hold:
		UpdateHold();
		break;
	case MyLib::UITelop::Phase::SlideOut:
		UpdateSlideOut();
		break;
	case MyLib::UITelop::Phase::Finished:
		break;
	default:
		break;
	}
}

void MyLib::UITelop::End()
{
	Renderer::GetInstance().Exit(shared_from_this());
}

void MyLib::UITelop::Draw() const
{
	if (!IsActive()) return;
	if (m_phase == Phase::Idle || m_phase == Phase::Finished) return;

	Vector2Int pos = Vector2Int{ 0,0 };

	if (auto pTransform = m_pTransform.lock())
	{
		pos = pTransform->GetScreenPos();
	}
	const int halfBandH = m_bandHeight / 2;

	DrawBox(0, pos.y - halfBandH, Game::kScreenWidth, pos.y + halfBandH, m_bandColor, true);

	const int textW = GetDrawStringWidthToHandle(m_text.c_str(), static_cast<int>(m_text.size()), m_fontHandle);
	const int fontSize = GetFontSizeToHandle(m_fontHandle);
	const int x = pos.x - textW / 2 + static_cast<int>(m_textOffsetX);
	const int y = pos.y - fontSize / 2;

	DrawStringToHandle(x, y, m_text.c_str(), kTextColor, m_fontHandle);
}

void MyLib::UITelop::ShowMessage(const std::wstring& text)
{
	ShowMessage(text, kDefaultSlideFrame, kDefaultOvershootFrame, kDefaultHoldFrame);
}

void MyLib::UITelop::ShowMessage(const std::wstring& text, int slideFrame, int overshootFrame, int holdFrame)
{
	m_text = text;
	m_slideFrame = slideFrame;
	m_overshootFrame = overshootFrame;
	m_holdFrame = holdFrame;

	m_phase = Phase::SlideIn;
	m_phaseFrameCount = 0;
}

void MyLib::UITelop::UpdateSlideIn()
{
	m_phaseFrameCount++;
	const float rate = std::min(static_cast<float>(m_phaseFrameCount) / static_cast<float>(m_slideFrame), 1.0f);

	const float startX = static_cast<float>(Game::kScreenWidth);
	const float overshootX = -m_overshootAmount;
	m_textOffsetX = std::lerp(startX, overshootX, rate);

	if (rate >= 1.0f)
	{
		m_phase = Phase::Overshoot;
		m_phaseFrameCount = 0;
	}

}

void MyLib::UITelop::UpdateOvershoot()
{
	m_phaseFrameCount++;
	const float rate = std::min(static_cast<float>(m_phaseFrameCount) / static_cast<float>(m_overshootFrame), 1.0f);

	m_textOffsetX = std::lerp(-m_overshootAmount, 0.0f, rate);

	if (rate >= 1.0f)
	{
		m_textOffsetX = 0.0f;
		m_phase = Phase::Hold;
		m_phaseFrameCount = 0;
	}
}

void MyLib::UITelop::UpdateHold()
{
	m_phaseFrameCount++;

	if (m_phaseFrameCount >= m_holdFrame)
	{
		m_phase = Phase::SlideOut;
		m_phaseFrameCount = 0;
	}
}

void MyLib::UITelop::UpdateSlideOut()
{
	m_phaseFrameCount++;
	const float rate = std::min(static_cast<float>(m_phaseFrameCount) / static_cast<float>(m_slideFrame), 1.0f);

	const float endX = -static_cast<float>(Game::kScreenWidth);
	m_textOffsetX = std::lerp(0.0f, endX, rate);

	if (rate >= 1.0f)
	{
		m_phase = Phase::Finished;
	}
}
