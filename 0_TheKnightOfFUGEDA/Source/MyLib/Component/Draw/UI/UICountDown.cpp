#include "UICountDown.h"
#include "../../../GameObject.h"
#include "../../../Renderer.h"
#include "../../../../Common/Sound/SoundManager.h"
#include <string>
#include <cassert>

namespace
{
	constexpr unsigned int kDefaultTextColor = 0xffffff;	// 文字色の初期値
	constexpr int kDefaultFrameParCount = 60;	// 1カウントにかけるフレーム数の初期値
	constexpr int kDefaultStartHoldFrame = 60;	// "START!"を表示し続けるフレーム数の初期値

	constexpr int kMaxAlpha = 255;	// 透明度の最大値(不透明)
}

MyLib::UICountDown::UICountDown(DrawLayer layer) :
	Drawable2D(layer),
	m_phase(CountPhase::Idle),
	m_fontHandle(-1),
	m_textColor(kDefaultTextColor),
	m_currentCount(0),
	m_frameParCount(kDefaultFrameParCount),
	m_frameCount(0),
	m_startHoldFrame(kDefaultStartHoldFrame),
	m_isFinished(true),
	m_isAlive(true)
{
}

void MyLib::UICountDown::Init(std::weak_ptr<GameObject> parent)
{
	std::shared_ptr<GameObject> pParent = parent.lock();
	m_pTransform = pParent->GetComponent<Transform>();
	if (!m_pTransform.lock())
	{
		assert(false && "UITelop : Transformコンポーネントがありません");
	}
	Renderer::GetInstance().Entry(shared_from_this());

	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("Ready", L"Data/File/Sound/SE/ready.mp3", SoundBus::SE, 1.0f, false); // カウントダウン時
	soundManager.LoadSoundClip("Go", L"Data/File/Sound/SE/go.mp3", SoundBus::SE, 1.0f, false); // スタート時のSE

}

void MyLib::UICountDown::Start()
{
}

void MyLib::UICountDown::Update()
{
	m_uiUpdateFrameCount++;

	if (UpdateFade())
	{
		m_phase = CountPhase::Finished;
		m_isFinished = true;
		m_isAlive = !m_isAfterDelete;
		return;
	}
	if (UpdateAppearState())
	{
		m_phase = CountPhase::Finished;
		m_isFinished = true;
		m_isAlive = !m_isAfterDelete;
		return;
	}
	auto& soundManager = SoundManager::GetInstance();

	switch (m_phase)
	{
	case MyLib::UICountDown::CountPhase::Idle:
		break;
	case MyLib::UICountDown::CountPhase::Counting:
		m_frameCount++;
		if (m_frameCount >= m_frameParCount)
		{
			m_frameCount = 0;
			m_currentCount--;

			if (m_currentCount <= 0)
			{
				soundManager.Play("Go", 1.0f, false);

				m_phase = CountPhase::ShowStart;
			}
			else
			{
				soundManager.Play("Ready", 1.0f, false);
			}
		}
		break;
	case MyLib::UICountDown::CountPhase::ShowStart:
		m_frameCount++;
		if (m_frameCount >= m_startHoldFrame)
		{
			m_phase = CountPhase::Finished;
			m_isFinished = true;
		}

		break;
	case MyLib::UICountDown::CountPhase::Finished:
		break;
	default:
		break;
	}

}

void MyLib::UICountDown::End()
{
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.DeleteSoundClip("Ready"); // カウントダウン時
	soundManager.DeleteSoundClip("Go"); // スタート時のSE

	Renderer::GetInstance().Exit(shared_from_this());
}

void MyLib::UICountDown::Draw() const
{
	if (!IsActive()) return;
	if (m_phase == CountPhase::Idle || m_phase == CountPhase::Finished) return;

	Vector2Int pos = Vector2Int{ 0,0 };

	if (auto pTransform = m_pTransform.lock())
	{
		pos = pTransform->GetScreenPos();
	}

	const std::wstring text = (m_phase == CountPhase::ShowStart) ? L"START!" : std::to_wstring(m_currentCount);

	const int textW = GetDrawStringWidthToHandle(text.c_str(), static_cast<int>(text.size()), m_fontHandle);
	const int fontSize = GetFontSizeToHandle(m_fontHandle);

	const float fadeOpacity = GetFadeAlphaRate();
	const double scale = static_cast<double>(GetAppearRate());

	const int finalAlpha = static_cast<int>(kMaxAlpha * fadeOpacity);
	const bool needBlend = finalAlpha != kMaxAlpha;
	if (needBlend) SetDrawBlendMode(DX_BLENDMODE_ALPHA, finalAlpha);

	const int x = pos.x - static_cast<int>(textW * scale) / 2;
	const int y = pos.y - static_cast<int>(fontSize * scale) / 2;

	DrawExtendStringToHandle(x, y, scale, scale, text.c_str(), m_textColor, m_fontHandle);

	if (needBlend) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MyLib::UICountDown::StartCountDown(int startCount, int frameParCount, int startHoldFrame)
{
	m_currentCount = startCount;
	m_frameParCount = frameParCount;
	m_startHoldFrame = startHoldFrame;
	m_frameCount = 0;
	m_phase = CountPhase::Counting;
	m_isFinished = false;
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Play("Ready", 1.0f, false);
}
