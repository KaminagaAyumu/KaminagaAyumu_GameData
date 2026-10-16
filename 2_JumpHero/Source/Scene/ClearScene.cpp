#include <memory>
#include "ClearScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "../Game/GameManager.h"
#include "../Game/ScoreManager.h"
#include "../Utility/UI/UIManager.h"
#include "../Utility/UI/UIText.h"
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

	constexpr int kClearDispMargin = 50;
	constexpr int kScoreDispMargin = -50;

	constexpr int kClearBgNo = 1; // ゲームクリアの際の背景の番号
	constexpr int kStageMaxNum = 3; // ステージの合計数

	constexpr int kCursorMoveIndex = 1; // カーソルが動く値

	constexpr float kClearSEVolume = 30.0f; // クリア時のSEのボリューム

	const Size kNormalListSize = { 300,300 };
	const Position2 kNormalListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 180 };

	const Position2 kHighScoreTextPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 180 };
	constexpr int kHighScoreTextColor = 0xffaaaa;

	const Size kLastStageListSize = { 300,200 };
	const Position2 kLastStageListPos = { Game::kScreenWidth / 2, 500 };

}

ClearScene::ClearScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo) :
	SceneBase(controller),
	m_pGameManager(gameManager),
	m_updateFunc(&ClearScene::FadeInUpdate),
	m_drawFunc(&ClearScene::FadeDraw),
	m_fadeColor(0xffffff),
	m_resultScore(0)
{
	m_frameCount = kFadeInterval;

	m_stageNo = stageNo;

	m_gameScore = m_pGameManager->GetCurrentScore();

	m_pBg = std::make_shared<Bg>();
	m_pBg->Init();
	m_pBg->LoadNormalBg(kClearBgNo);

	m_pUIManager = std::make_shared<UIManager>();

	// ステージ番号をチェック
	// 最後のステージだった場合は次のステージへ遷移しないようにする
	if (m_stageNo >= kStageMaxNum)
	{
		m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kLastStageListSize, kLastStageListPos);
		auto list = m_pSelectList.lock();
		list->AddOption("ステージセレクトへ", [this]()
		{
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
		});
		list->AddOption("タイトルへ", [this]()
			{
					m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			});
	}
	else
	{
		m_pSelectList = m_pUIManager->CreateSelectList(Types::FontType::Small, kNormalListSize, kNormalListPos);
		auto list = m_pSelectList.lock();
		// 次のステージへ遷移するリストを追加
		list->AddOption("次のステージへ", [this]()
			{
				m_stageNo++; // 次のステージへ遷移するように加算
				m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_stageNo));
			});
		list->AddOption("ステージセレクトへ", [this]()
		{
			m_controller.ChangeScene(std::make_shared<SelectScene>(m_controller));
		});
		list->AddOption("タイトルへ", [this]()
			{
					m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			});
	}	

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
	Application::GetInstance().GetSoundManager()->LoadSoundClip("clearBGM", L"data/sound/BGM/clearBGM.mp3", SoundBus::BGM, 1.0f, true);
	Application::GetInstance().GetSoundManager()->CrossFadeBGM("clearBGM", kCrossFadeTime);

	// ステージクリアのSEをロード
	Application::GetInstance().GetSoundManager()->LoadSoundClip("clearSE", L"data/sound/SE/clearSE.mp3", SoundBus::SE, kClearSEVolume, false);

	// 現在のハイスコアを取得
	m_highScore = Application::GetInstance().GetScoreManager()->GetHighScore(m_stageNo);

	// ハイスコアを更新
	Application::GetInstance().GetScoreManager()->UpdateHighScore(m_stageNo, m_gameScore);

	// ハイスコアを更新した場合
	if (m_highScore < Application::GetInstance().GetScoreManager()->GetHighScore(m_stageNo))
	{
		// ハイスコア更新のテキストを生成する
		m_pHighScoreUpdateText = m_pUIManager->CreateText(Types::FontType::Midium, "ハイスコア更新！", kHighScoreTextPos);
		auto highScore = m_pHighScoreUpdateText.lock();
		highScore->SetColor(kHighScoreTextColor);
		highScore->SetBlinking();
	}

	m_pClearText = m_pUIManager->CreateText(Types::FontType::Header, "クリア！", { Game::kScreenWidth / 2, kClearDispMargin });
}

ClearScene::~ClearScene()
{
}

void ClearScene::Update(Input& input)
{
	(this->*m_updateFunc)(input);
}

void ClearScene::Draw()
{
	(this->*m_drawFunc)();
}

void ClearScene::FadeInUpdate(Input&)
{
	m_frameCount--;
	m_pBg->Update();
	if (m_frameCount <= 0)
	{
		Application::GetInstance().GetSoundManager()->Play("clearSE", 1.0f, true);
		// フェードイン完了
		m_updateFunc = &ClearScene::NormalUpdate;
		m_drawFunc = &ClearScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void ClearScene::NormalUpdate(Input& input)
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
		m_updateFunc = &ClearScene::FadeOutUpdate;
		m_drawFunc = &ClearScene::FadeDraw;
		Application::GetInstance().GetSoundManager()->Play("ok_se", 1.0f, true);
		return; // 念のため処理を抜ける
	}
}

void ClearScene::FadeOutUpdate(Input&)
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

void ClearScene::NormalDraw()
{
	m_pBg->Draw();
	m_pUIManager->Draw();
#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: NormalDraw", 0xffffff);
#endif
}

void ClearScene::FadeDraw()
{
	m_pBg->Draw();
	m_pUIManager->Draw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene: FadeDraw", 0xffffff);
#endif
}
