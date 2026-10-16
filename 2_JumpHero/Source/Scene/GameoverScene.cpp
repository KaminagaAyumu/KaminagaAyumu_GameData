#include <memory>
#include "GameoverScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "../Game/GameManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UISelectList.h"
#include "../Utility/UI/UIFormatText.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "../Utility/Input.h"
#include "../Utility/Bg.h"
#include "../Utility/Game.h"
#include "../Utility/GameType.h"
#include "DxLib.h"


namespace
{
	constexpr int kFadeInterval = 60; // フェード処理を行う時間

	constexpr float kScoreAddRate = 0.15f; // スコア加算の割合
	constexpr float	kScoreThreshold = 0.9f; // スコア加算の閾値

	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値

	constexpr float kCrossFadeTime = 120.0f; // BGMをクロスフェードさせる際の時間

	constexpr int kCursorMoveIndex = 1; // カーソルが動く値

	constexpr int kHeaderDispMargin = 50;
	constexpr int kScoreDispMargin = -50;

	constexpr int kGameoverBgNo = 2; // ゲームオーバーの際の背景の番号
	constexpr int kStageMaxNum = 2; // ステージの合計数

	const Size kNormalListSize = { 300,300 };
	const Position2 kNormalListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 180 };
}

GameoverScene::GameoverScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo) :
	SceneBase(controller),
	m_pGameManager(gameManager),
	m_updateFunc(&GameoverScene::FadeInUpdate),
	m_drawFunc(&GameoverScene::FadeDraw),
	m_fadeColor(0xffffff),
	m_resultScore(0)
{
	m_frameCount = kFadeInterval;

	m_stageNo = stageNo;

	m_gameScore = m_pGameManager->GetCurrentScore();

	m_pBg = std::make_shared<Bg>();
	m_pBg->Init();
	m_pBg->LoadNormalBg(kGameoverBgNo);

	m_pUIManager = std::make_shared<UIManager>();

	// ステージ番号をチェック
	m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kNormalListSize, kNormalListPos);
	auto list = m_pSelectList.lock();
	// コンティニューするリストを追加
	list->AddOption("コンティニュー", [this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_stageNo));
		});
	// ステージセレクトへ遷移するリストを追加
	list->AddOption("ステージセレクトへ", [this]()
	{
		m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
	});
	// タイトルへ遷移するリストを追加
	list->AddOption("タイトルへ", [this]()
		{
			m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		});

	m_pScoreText = m_pUIManager->CreateFormatText(Types::FontType::Large, "", { Game::kScreenWidth / 2, Game::kScreenHeight / 2 + kScoreDispMargin });
	auto score = m_pScoreText.lock();
	score->SetProvider([this]()
		{
			return std::string("スコア:") + std::to_string(GetScore());
		});
	score->SetCenter();

	// カーソル選択SEをロード(念のため)
	Application::GetInstance().GetSoundManager()->LoadSoundClip("cursor_se", L"data/sound/SE/cursorSE.mp3", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("ok_se", L"data/sound/SE/okSE.mp3", SoundBus::SE, 1.0f, false);

	// ステージクリアのBGMをロード
	Application::GetInstance().GetSoundManager()->LoadSoundClip("gameoverBGM", L"data/sound/BGM/gameoverBGM.mp3", SoundBus::BGM, 1.0f, true);
	Application::GetInstance().GetSoundManager()->CrossFadeBGM("gameoverBGM", kCrossFadeTime);

	m_pClearText = m_pUIManager->CreateText(Types::FontType::Header, "ゲームオーバー...", { Game::kScreenWidth / 2, kHeaderDispMargin });
}

GameoverScene::~GameoverScene()
{
}

void GameoverScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void GameoverScene::Draw()
{
	(this->*m_drawFunc)();
}

void GameoverScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	m_pBg->Update();
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_updateFunc = &GameoverScene::NormalUpdate;
		m_drawFunc = &GameoverScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void GameoverScene::NormalUpdate(Input& input)
{
	m_pBg->Update();
	m_pUIManager->Update();

	// スコアの更新処理
	if (m_resultScore < m_gameScore) // リザルト表示スコアがゲームで獲得したスコアより小さい場合
	{
		int add = static_cast<int>((m_gameScore - m_resultScore) * kScoreAddRate); // ゲーム獲得スコアとの差分を計算
		if (add <= kScoreThreshold) // 加算の値が閾値を超えたら
		{
			m_resultScore = m_gameScore; // スコアを現在のスコアに合わせる
		}
		else
		{
			m_resultScore += add; // スコアを更新
		}
		if (m_resultScore > m_gameScore) // スコアが現在のスコアを超えた場合
		{
			m_resultScore = m_gameScore; // スコアを現在のスコアに合わせる
		}
	}

	if (m_resultScore != m_gameScore)
	{
		return;
	}

	if (input.IsTriggered("Up"))
	{
		Application::GetInstance().GetSoundManager()->Play("cursor_se", 1.0f, true);
		auto list = m_pSelectList.lock();
		list->MoveCursor(-kCursorMoveIndex);
	}
	if (input.IsTriggered("Down"))
	{
		Application::GetInstance().GetSoundManager()->Play("cursor_se", 1.0f, true);
		auto list = m_pSelectList.lock();
		list->MoveCursor(kCursorMoveIndex);
	}

	// 決定ボタンを押したとき
	if (input.IsTriggered("OK"))
	{
		// フェードイン完了
		m_fadeColor = 0x000000;
		m_updateFunc = &GameoverScene::FadeOutUpdate;
		m_drawFunc = &GameoverScene::FadeDraw;
		Application::GetInstance().GetSoundManager()->Play("ok_se", 1.0f, true);
		return; // 念のため処理を抜ける
	}
}

void GameoverScene::FadeOutUpdate(Input&)
{
	m_frameCount++;
	m_pBg->Update();
	if (m_frameCount >= kFadeInterval)
	{
		// フェードアウト完了
		auto list = m_pSelectList.lock();
		list->TriggerSelect();
		return; // 念のため処理を抜ける
	}
}

void GameoverScene::NormalDraw()
{
	m_pBg->Draw();
	m_pUIManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"GameoverScene: NormalDraw", 0xffffff);
#endif
}

void GameoverScene::FadeDraw()
{
	m_pBg->Draw();
	m_pUIManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"GameoverScene: FadeDraw", 0xffffff);
#endif
}