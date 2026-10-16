#include "SelectScene.h"

SelectScene::SelectScene():
	m_selectTutorialHandle(-1),
	m_selectStartHandle(-1),
	m_tutorialGraphHandle(-1),
	m_isTutorial(false)
{
}

SelectScene::~SelectScene()
{
}

void SelectScene::Init()
{
	SceneBase::Init();
	m_selectTutorialHandle = LoadGraph("data/select_tutorial.png");
	m_selectStartHandle = LoadGraph("data/select_gamestart.png");
	m_tutorialGraphHandle = LoadGraph("data/tutorial.png");
	m_isTutorial = false;
	m_cursorPos.X = 380.0f;
	m_cursorPos.Y = 240.0f;
	m_nextScene = NextScene::SceneMain;
	m_bgmHandle = LoadSoundMem("data/select_Bgm.mp3");
	m_sound->Init(m_cursorSeHandle);
	m_sound->ChangeVolume(200, m_bgmHandle);
	m_sound->PlayLoop(m_bgmHandle);
}

void SelectScene::End()
{
	DeleteSoundMem(m_bgmHandle);
	DeleteGraph(m_selectTutorialHandle);
	DeleteGraph(m_selectStartHandle);
	DeleteGraph(m_tutorialGraphHandle);
	SceneBase::End();
}

void SelectScene::Update()
{
	SceneBase::Update();
	if (m_inputManager->GetPadDown(PAD_INPUT_UP) || m_inputManager->GetKeyDown(KEY_INPUT_UP))
	{
		m_sound->Play(m_cursorSeHandle);
		if (m_cursorPos.Y > 240)
		{
			m_cursorPos.Y -= 240.0f;
		}
		else
		{
			m_cursorPos.Y = 480.0f;
		}
	}
	if (m_inputManager->GetPadDown(PAD_INPUT_DOWN) || m_inputManager->GetKeyDown(KEY_INPUT_DOWN))
	{
		m_sound->Play(m_cursorSeHandle);
		if (m_cursorPos.Y < 480)
		{
			m_cursorPos.Y += 240.0f;
		}
		else
		{
			m_cursorPos.Y = 240.0f;
		}
	}
	// EnterキーまたはパッドのAボタンが押されたら次のシーンへ遷移
	if (m_inputManager->GetKeyDown(KEY_INPUT_RETURN) || m_inputManager->GetPadDown(PAD_INPUT_1))
	{
		m_sound->Play(m_selectSeHandle);
		if (m_isTutorial)
		{
			m_isTutorial = false;
			return;
		}
		if (m_cursorPos.Y == 240)
		{
			m_isTutorial = true;
		}
		else
		{
			SetNextScene(m_nextScene);
			m_isEnd = true;
		}
	}
}

void SelectScene::Draw()
{
	SceneBase::Draw();
	DrawBox(m_cursorPos.X, m_cursorPos.Y, m_cursorPos.X + 40, m_cursorPos.Y + 40, GetColor(255, 0, 0), true);
	DrawGraph(450, 240, m_selectTutorialHandle, true);
	DrawGraph(450, 240 * 2, m_selectStartHandle, true);
	if (m_isTutorial)
	{
		DrawGraph(0, 0, m_tutorialGraphHandle, true);
	}
}
