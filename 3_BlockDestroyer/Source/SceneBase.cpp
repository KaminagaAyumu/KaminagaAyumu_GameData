#include "SceneBase.h"
#include "Game.h"
#include "DxLib.h"

SceneBase::SceneBase() :
	m_bgHandle(-1),
	m_bgmHandle(-1),
	m_selectSeHandle(-1),
	m_cursorSeHandle(-1),
	m_bg(nullptr),
	m_nextScene(NextScene::None),
	m_isEnd(false),
	m_sound(nullptr),
	m_ui(nullptr),
	m_inputManager(nullptr)
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	m_bgHandle = LoadGraph("data/bg.png");
	m_selectSeHandle = LoadSoundMem("data/select_Se.mp3");
	m_cursorSeHandle = LoadSoundMem("data/cursor_Se.mp3");
	m_isEnd = false;
	m_bg = new Bg;
	m_sound = new Sound;
	m_ui = new UI;
	m_inputManager = &InputManager::Instance();
}

void SceneBase::End()
{
	DeleteGraph(m_bgHandle);
	DeleteSoundMem(m_selectSeHandle);
	DeleteSoundMem(m_cursorSeHandle);
}

void SceneBase::Update()
{
	m_inputManager->Update();
}

void SceneBase::Draw()
{
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 1.0f, 0, m_bgHandle, false);
}

