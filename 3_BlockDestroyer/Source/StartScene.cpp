#include "StartScene.h"

namespace
{
	int kStartFadeTime = 3;
}

StartScene::StartScene() :
	m_castleBgHandle(-1),
	m_startHandle(-1),
	m_StartAlphaCount(0),
	m_startFadeAlpha(0)
{
}

StartScene::~StartScene()
{
}

void StartScene::Init()
{
	SceneBase::Init();
	m_nextScene = NextScene::SceneSelect;
	m_castleBgHandle = LoadGraph("data/titleScene_Bg.png");
	m_startHandle = LoadGraph("data/startUI.png");
	m_StartAlphaCount = kStartFadeTime;
	m_startFadeAlpha = 0;
	m_bgmHandle = LoadSoundMem("data/zikken2.mp3");
	m_sound->Init(m_selectSeHandle);
	m_sound->ChangeVolume(200, m_bgmHandle);
	m_sound->PlayLoop(m_bgmHandle);
}

void StartScene::End()
{
	DeleteGraph(m_castleBgHandle);
	DeleteGraph(m_startHandle);
	DeleteSoundMem(m_bgmHandle);
	SceneBase::End();
}

void StartScene::Update()
{
	SceneBase::Update();

#ifdef _DEBUG

	if (m_bgmHandle == -1)
	{
		printfDx("bgmHandle:%d\n", m_bgmHandle);
	}

#endif
	
	m_startFadeAlpha += m_StartAlphaCount;
	if (m_startFadeAlpha >= 255)
	{
		m_startFadeAlpha = 255;
		m_StartAlphaCount = -kStartFadeTime;
	}
	if (m_startFadeAlpha <= 0)
	{
		m_startFadeAlpha = 0;
		m_StartAlphaCount = kStartFadeTime;
	}
	

	// EnterキーまたはパッドのAボタンが押されたら次のシーンへ遷移
	if (m_inputManager->GetKeyDown(KEY_INPUT_RETURN) || m_inputManager->GetPadDown(PAD_INPUT_1))
	{
		SetNextScene(m_nextScene);
		m_sound->Play();
		m_isEnd = true;
	}
}

void StartScene::Draw()
{
	SceneBase::Draw();
	
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 1.0f, 0, m_castleBgHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_startFadeAlpha);

	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + 200, 0.5f, 0, m_startHandle, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

