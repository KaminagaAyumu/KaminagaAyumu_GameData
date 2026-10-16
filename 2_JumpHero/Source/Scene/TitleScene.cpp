#include <memory>
#include "TitleScene.h"
#include "../Utility/Input.h"
#include "SceneController.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "../Utility/Bg.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UIText.h"
#include "../Game/Effect/EffectManager.h"
#include "../Game/Player.h"
#include "../Utility/Application.h"
#include <string>
#include "DxLib.h"
#include "EffekseerForDxLib.h"

namespace
{
	constexpr int kRandBgNum = 1; // タイトルの背景描画方法をランダムに決めるときに使う値
	constexpr int kTitleSceneMapNo = -1; // タイトルのマップ番号

	constexpr int kFadeInterval = 60; // フェード処理を行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr int kStartTextMargin = 100; // スタート方法を示すテキストの表示位置調整

	const Size kTitleLogoSize = { 1280, 720 };
	const Position2 kTitleLogoPos = { 0, -150 };
}


TitleScene::TitleScene(SceneController& controller) : SceneBase(controller),
m_frameCount(kFadeInterval),
m_updateFunc(&TitleScene::FadeInUpdate),
m_drawFunc(&TitleScene::FadeDraw)
{
	m_titleImageHandle = LoadGraph(L"data/logo.png");
	m_bg = std::make_shared<Bg>();
	
	// 背景をランダムで決める(地下か地上かのどちらか)
	if (GetRand(kRandBgNum) == kRandBgNum)
	{
		m_bg->SetLayer();
	}

	m_bg->SetBgType(Types::BgType::ScrollX);
	m_bg->Init();
	m_pMap = std::make_shared<Map>(kTitleSceneMapNo, false);
	m_pCamera = std::make_shared<Camera>(m_pMap->GetMapSize());
	m_soundManager = Application::GetInstance().GetSoundManager();
	m_soundManager->LoadSoundClip("title", L"data/sound/BGM/titleBGM.mp3",SoundBus::BGM,1.0f,true);
	m_soundManager->LoadSoundClip("testSE", L"data/sound/SE/testSE.mp3", SoundBus::SE, 1.0f, false);
	m_soundManager->PlayBGM("title",0.0f);

	m_pUIManager = std::make_shared<UIManager>();
	auto text = m_pUIManager->CreateText(Types::FontType::Large, "STARTかAボタンでスタート", { Game::kScreenWidth / 2,
		Game::kScreenHeight / 2 + kStartTextMargin});
	auto pText = text.lock();
	pText->SetBlinking();

	m_pUIManager->CreateImage(Types::ImageType::TitleLogo, kTitleLogoSize, kTitleLogoPos);

	m_pEffectManager = std::make_shared<EffectManager>(controller.GetEffekseerResourceManager());

	m_pTitlePlayer = std::make_shared<Player>(m_pMap);
	m_pTitlePlayer->SetCamera(m_pCamera);
	m_pTitlePlayer->InitAuto();
	
	m_pCamera->SetTargetProvider([this]() {return m_pTitlePlayer->GetPos(); });
	m_pCamera->Init();
}

TitleScene::~TitleScene()
{
	DeleteGraph(m_titleImageHandle);
}

void TitleScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void TitleScene::Draw()
{
	(this->*m_drawFunc)();
}

void TitleScene::FadeInUpdate(Input& input)
{
	// カメラの更新
	m_pCamera->Update();

	m_pUIManager->Update();

	m_pTitlePlayer->Update(input);

	m_frameCount--;

	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &TitleScene::NormalUpdate;
		m_drawFunc = &TitleScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	// カメラの更新
	m_pCamera->Update();

	m_soundManager->Update();
	m_pUIManager->Update();
	m_pEffectManager->Update();

	m_bg->Update();
	m_pMap->Update();

	m_pTitlePlayer->Update(input);

	// STARTボタンもしくはAボタンが押されたら
	if (input.IsTriggered("OK"))
	{
		// フェードアウト後、ゲームシーンへ移行
		m_updateFunc = &TitleScene::FadeOutUpdate;
		m_drawFunc = &TitleScene::FadeDraw;
		m_frameCount = 0;
		m_soundManager->Play("testSE", 1.0f, false);
		return;
	}

#ifdef _DEBUG
	// エフェクトテスト用
	if (input.IsTriggered("Up"))
	{
		m_pEffectManager->CreateEffekseerEffect(Types::EffectType::Test, { Game::kScreenWidth / 2,Game::kScreenHeight / 2 }, false);
	}
#endif
}

void TitleScene::FadeOutUpdate(Input& input)
{
	m_frameCount++;
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
		return; // 念のため処理を抜ける
	}
}

void TitleScene::NormalDraw()
{
	m_bg->Draw(m_pCamera);
	m_pMap->Draw(m_pCamera);

	m_pTitlePlayer->Draw();

	m_pUIManager->Draw();

	m_pEffectManager->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"TitleScene: NormalDraw", 0xFFFFFF);
#endif
	

}

void TitleScene::FadeDraw()
{
	m_bg->Draw(m_pCamera);
	m_pMap->Draw(m_pCamera);

	m_pTitlePlayer->Draw();

	m_pUIManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG
	DrawString(0, 0, L"TitleScene: FadeDraw", 0xFFFFFF);
#endif
}
