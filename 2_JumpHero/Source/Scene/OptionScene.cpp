#include <memory>
#include "OptionScene.h"
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
	const Position2 kSelectListPos = { 300, 350 };

}

OptionScene::OptionScene(SceneController& controller) :
	SceneBase(controller),
	m_updateFunc(&OptionScene::AppearUpdate),
	m_drawFunc(&OptionScene::IntervalDraw),
	m_fadeColor(0x000000)
{
	m_frameCount = kFadeInterval;
	m_soundManager = Application::GetInstance().GetSoundManager();

	m_pUIManager = std::make_unique<UIManager>();

	m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kSelectListSize, kSelectListPos);
	auto list = m_pSelectList.lock();
	list->AddOption("サウンド調整", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, 0));
		});
	list->AddOption("ステージ1", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, 1));
		});
	list->AddOption("ゲームを終わる", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, 2));
		});
}

OptionScene::~OptionScene()
{
}

void OptionScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void OptionScene::Draw()
{
	(this->*m_drawFunc)();
}

void OptionScene::AppearUpdate(Input& input)
{
	m_frameCount--;
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &OptionScene::NormalUpdate;
		m_drawFunc = &OptionScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void OptionScene::NormalUpdate(Input& input)
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

	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &OptionScene::DisAppearUpdate;
		m_drawFunc = &OptionScene::IntervalDraw;
		m_soundManager->Play("ok_se", 1.0f, true);
		return; // 念のため処理を抜ける
	}

}

void OptionScene::DisAppearUpdate(Input& input)
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

void OptionScene::NormalDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x116611, TRUE);

	m_pUIManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"OptionScene: NormalDraw", 0xffffff);
#endif
}

void OptionScene::IntervalDraw()
{

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"OptionScene: IntervalDraw", 0xffffff);
#endif
}
