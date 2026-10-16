#include <memory>
#include "SoundControlScene.h"
#include "SceneController.h"
#include "GameScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UISelectList.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	const Size kSelectListSize = { 400, 700 };
	const Position2 kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };

}

SoundControlScene::SoundControlScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&SoundControlScene::AppearUpdate),
	m_drawFunc(&SoundControlScene::IntervalDraw),
	m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;
	m_soundManager = Application::GetInstance().GetSoundManager();
	m_soundManager->LoadSoundClip("cursor_se", L"data/sound/cursorSE.mp3", SoundBus::SE, 1.0f, false);
	m_soundManager->LoadSoundClip("ok_se", L"data/sound/okSE.mp3", SoundBus::SE, 1.0f, false);

	m_pUIManager = std::make_unique<UIManager>();

	m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kSelectListSize, kSelectListPos);
	auto list = m_pSelectList.lock();
	list->AddOption("Master", [this]()
		{
		});
	list->AddOption("BGM", [this]()
		{
		});
	list->AddOption("SE", [this]()
		{
		});
	list->AddOption("閉じる", [this]()
		{
			// 開いたシーンに戻る
			m_controller.PopScene();
		});
}

SoundControlScene::~SoundControlScene()
{
}

void SoundControlScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void SoundControlScene::Draw()
{
	(this->*m_drawFunc)();
}

void SoundControlScene::AppearUpdate(Input& input)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &SoundControlScene::NormalUpdate;
		m_drawFunc = &SoundControlScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void SoundControlScene::NormalUpdate(Input& input)
{
	m_pUIManager->Update();
	if (input.IsTriggered("Down"))
	{
		m_soundManager->Play("cursor_se", 1.0f, true);

		auto list = m_pSelectList.lock();
		list->MoveCursor(1);
	}
	if (input.IsTriggered("Up"))
	{
		m_soundManager->Play("cursor_se", 1.0f, true);

		auto list = m_pSelectList.lock();
		list->MoveCursor(-1);
	}

	if (input.IsTriggered("Left"))
	{

	}

	if (input.IsTriggered("Right"))
	{

	}

	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &SoundControlScene::DisAppearUpdate;
		m_drawFunc = &SoundControlScene::IntervalDraw;
		m_soundManager->Play("ok_se", 1.0f, true);
		return; // 念のため処理を抜ける
	}

}

void SoundControlScene::DisAppearUpdate(Input& input)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		auto list = m_pSelectList.lock();
		list->TriggerSelect();
		//m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_selectIndex));
		return; // 念のため処理を抜ける
	}
}

void SoundControlScene::NormalDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x116611, TRUE);

	m_pUIManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"SoundControlScene: NormalDraw", 0xffffff);
#endif
}

void SoundControlScene::IntervalDraw()
{

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"SoundControlScene: IntervalDraw", 0xffffff);
#endif
}