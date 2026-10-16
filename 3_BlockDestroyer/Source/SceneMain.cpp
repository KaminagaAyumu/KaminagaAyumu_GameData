#include "SceneMain.h"
#include "DxLib.h"

namespace
{
	constexpr float kFirstCursorPosX = 400.0f; // 最初のカーソル位置
	constexpr float kFirstCursorPosY = 300.0f; // 最初のカーソル位置
	constexpr float kCursorMoveHeight = 100.0f; // カーソルの動く高さ

	constexpr int kPauseWidth = 960; // ポーズ画面に表示する背景の横幅
	constexpr int kPauseHeight = 540; // ポーズ画面に表示する背景の縦幅

	constexpr int kStartDispTime = 120; // スタートを表示する時間
	
	constexpr int kCountdownNumSizeX = 75; // スタートまでのカウントダウンの数字の大きさ
	constexpr int kCountdownNumSizeY = 95; // スタートまでのカウントダウンの数字の大きさ

	constexpr int kScorePosX = Game::kGameRightWidth; // スコアUIの表示位置X
	constexpr int kScorePosY = 40; // スコアUIの表示位置Y
	constexpr int kScoreWidth = 147; // スコアUIの幅
	constexpr int kScoreHeight = 45; // スコアUIの高さ

	constexpr int kMaxFadeAlpha = 255; // 画像の透明度(不透明)
}

SceneMain::SceneMain() :
	m_resultFontHandle(-1),
	m_defaultFontHandle(-1),
	m_bgGraphHandle(-1),
	m_playerGraphHandle(-1),
	m_blockGraphHandle(-1),
	m_startUIHandle(-1),
	m_scoreUIHandle(-1),
	m_clearUIHandle(-1),
	m_gameoverUIHandle(-1),
	m_pauseUIHandle(-1),
	m_blockSeHandle(-1),
	m_blockEffectGraphHandle(-1),
	m_countdownSeHandle(-1),
	m_startSeHandle(-1),
	m_pauseSeHandle(-1),
	m_retryUIHandle(-1),
	m_titleUIHandle(-1),
	m_restartUIHandle(-1),
	m_clearBgmHandle(-1),
	m_gameoverBgmHandle(-1),
	m_blurGraphHandle(-1),
	m_frameCount(0),
	m_countdown(0),
	m_prevCountdown(0),
	m_startDispCount(0),
	m_startFadeAlpha(0),
	m_clearScore(0),
	m_isStartSePlayed(false),
	m_isClearBgmPlayed(false),
	m_isGameoverBgmPlayed(false),
	m_isPause(false),
	m_isCardSelectPause(false),
	m_cursorPos(),
	m_player(nullptr),
	m_ballManager(nullptr),
	m_blockManager(nullptr),
	m_gManager(nullptr)
{
}

SceneMain::~SceneMain()
{

}

void SceneMain::Init()
{
	SceneBase::Init();
	m_resultFontHandle = CreateFontToHandle("HG明朝E", 100, -1, -1); // フォントの生成
	m_defaultFontHandle = CreateFontToHandle("HG明朝E", 45, -1, -1); // フォントの生成
	m_bgGraphHandle = LoadGraph("data/main_Bg.png");
	m_playerGraphHandle = LoadGraph("data/player.png");
	m_blockGraphHandle = LoadGraph("data/block.png");
	m_startUIHandle = LoadGraph("data/start_UI.png");
	m_scoreUIHandle = LoadGraph("data/score_UI.png");
	m_clearUIHandle = LoadGraph("data/gameClear_UI.png");
	m_gameoverUIHandle = LoadGraph("data/gameover_UI.png");
	m_pauseUIHandle = LoadGraph("data/pause_UI.png");
	m_retryUIHandle = LoadGraph("data/retry_UI.png");
	m_titleUIHandle = LoadGraph("data/title_UI.png");
	m_restartUIHandle = LoadGraph("data/restart_UI.png");
	m_blockSeHandle = LoadSoundMem("data/block_Se.mp3");
	m_blockEffectGraphHandle = LoadGraph("data/ice.png");
	m_countdownSeHandle = LoadSoundMem("data/countdownSe.mp3");
	m_startSeHandle = LoadSoundMem("data/startSe.mp3");
	m_pauseSeHandle = LoadSoundMem("data/pause_Se.mp3");
	m_clearBgmHandle = LoadSoundMem("data/clear_Bgm.mp3");
	m_gameoverBgmHandle = LoadSoundMem("data/gameover_Bgm.mp3");
	m_bgmHandle = LoadSoundMem("data/main_Bgm.mp3");
	m_blurGraphHandle = -1;
	m_frameCount = 180;
	m_countdown = 0;
	m_prevCountdown = 0;
	m_startDispCount = 0;
	m_startFadeAlpha = kMaxFadeAlpha;
	m_clearScore = 0;
	m_isStartSePlayed = false;
	m_isClearBgmPlayed = false;
	m_isGameoverBgmPlayed = false;
	m_isPause = false;
	m_isCardSelectPause = false;
	m_cursorPos = { kFirstCursorPosX,kFirstCursorPosY };
	m_nextScene = NextScene::None;
	m_ballManager = new BallManager;
	m_ballManager->Init();
	m_player = new Player();
	m_player->Init(m_playerGraphHandle);
	m_blockManager = new BlockManager();
	m_blockManager->BlockFirstCreate(8, 8, m_blockGraphHandle, m_blockSeHandle, m_blockEffectGraphHandle);
	m_gManager = new GameManager(m_ballManager, m_player, m_blockManager);
	m_gManager->Init();
	m_bg->Init(m_bgGraphHandle);
	m_sound->ChangeVolume(200, m_bgmHandle);
}
void SceneMain::End()
{
	SceneBase::End();
	m_ballManager->End();
	m_player->End();
	DeleteFontToHandle(m_resultFontHandle);
	DeleteFontToHandle(m_defaultFontHandle);
	DeleteGraph(m_bgGraphHandle);
	DeleteGraph(m_playerGraphHandle);
	DeleteGraph(m_blockGraphHandle);
	DeleteGraph(m_startUIHandle);
	DeleteGraph(m_scoreUIHandle);
	DeleteGraph(m_clearUIHandle);
	DeleteGraph(m_gameoverUIHandle);
	DeleteGraph(m_pauseUIHandle);
	DeleteGraph(m_retryUIHandle);
	DeleteGraph(m_titleUIHandle);
	DeleteGraph(m_restartUIHandle);
	DeleteGraph(m_blockEffectGraphHandle);
	DeleteSoundMem(m_blockSeHandle);
	DeleteSoundMem(m_countdownSeHandle);
	DeleteSoundMem(m_startSeHandle);
	DeleteSoundMem(m_pauseSeHandle);
	DeleteSoundMem(m_clearBgmHandle);
	DeleteSoundMem(m_gameoverBgmHandle);
	DeleteSoundMem(m_bgmHandle);

	m_gManager->End();
}
void SceneMain::Update()
{
	SceneBase::Update();


	// キーボードのスペースキー、パッドのスタートボタンが押された時の処理
	if (m_inputManager->GetKeyDown(KEY_INPUT_SPACE) || m_inputManager->GetPadDown(PAD_INPUT_8))
	{
		m_sound->ChangeVolume(200, m_pauseSeHandle);
		m_sound->Play(m_pauseSeHandle);
		m_isPause = !m_isPause;
	}
	if (m_isPause) // ポーズ中
	{
		PauseUpdate();
		return;
	}

	m_frameCount--;
	m_countdown = m_frameCount / 60;


	if (m_countdown != m_prevCountdown)
	{
		m_sound->ChangeVolume(200, m_countdownSeHandle);
		m_sound->Play(m_countdownSeHandle);
		m_prevCountdown = m_countdown;
	}
	if (m_frameCount > 0)
	{
		return;
	}
	else
	{
		m_frameCount = -1;
	}

	if (!m_isStartSePlayed)
	{
		m_sound->ChangeVolume(200, m_startSeHandle);
		m_sound->Play(m_startSeHandle);
		m_sound->PlayLoop(m_bgmHandle);
		m_isStartSePlayed = true;
	}

	if (m_startDispCount >= kStartDispTime)
	{
		m_startDispCount = kStartDispTime;
	}
	else
	{
		m_startDispCount++;
		m_startFadeAlpha = kMaxFadeAlpha - (kMaxFadeAlpha * m_startDispCount / kStartDispTime);
	}


	if (m_gManager->IsGameClear())
	{
		m_sound->Stop(m_bgmHandle);
		if (!m_isClearBgmPlayed)
		{
			m_sound->ChangeVolume(150, m_clearBgmHandle);
			m_sound->PlayLoop(m_clearBgmHandle);
			m_isClearBgmPlayed = true;
			//m_cursorPos.Y += kCursorMoveHeight;
		}
		ClearUpdate();
		return;
	}
	if (m_gManager->IsGameOver()) // ボールが存在しない場合
	{
		m_sound->Stop(m_bgmHandle);
		if (!m_isGameoverBgmPlayed)
		{
			m_sound->ChangeVolume(150, m_gameoverBgmHandle);
			m_sound->PlayLoop(m_gameoverBgmHandle);
			m_isGameoverBgmPlayed = true;
		}
		OverUpdate();
		return;
	}
	

	m_gManager->Update(*m_inputManager);
	m_player->PosCorrection();

	if (m_inputManager->GetPadDown(PAD_INPUT_7) || m_inputManager->GetKeyDown(KEY_INPUT_V))
	{
		m_isCardSelectPause = !m_isCardSelectPause;
	}
	if (m_isCardSelectPause)
	{
		return;
	}

	m_player->Update(*m_inputManager);
	m_blockManager->Update();
	m_ballManager->Update();

}
void SceneMain::Draw()
{
	SceneBase::Draw();
	m_bg->Draw();
	m_ballManager->Draw();
	m_player->Draw();
	m_blockManager->Draw();
	m_gManager->Draw();
	DrawLine(Game::kGameLeftWidth, Game::kGameUpHeight, Game::kGameLeftWidth, Game::kGameDownHeight, GetColor(255, 255, 255));
	DrawLine(Game::kGameRightWidth, Game::kGameUpHeight, Game::kGameRightWidth, Game::kGameDownHeight, GetColor(255, 255, 255));
	DrawLine(Game::kGameLeftWidth, m_player->GetPosY(), Game::kGameRightWidth, m_player->GetPosY(), GetColor(255, 0, 0));
	DrawGraph(kScorePosX, kScorePosY, m_scoreUIHandle, true);
	if (m_frameCount > 0)
	{
		if (m_frameCount < 180)
		{
			m_ui->DrawNumberInSize(m_countdown + 1, Game::kScreenWidth / 2 - kCountdownNumSizeX / 2, Game::kScreenHeight / 2 - kCountdownNumSizeY / 2, kCountdownNumSizeX,kCountdownNumSizeY);
		}
	}
	else
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_startFadeAlpha);
		int sizeX, sizeY;
		GetGraphSize(m_startUIHandle, &sizeX, &sizeY);
		DrawGraph(Game::kScreenWidth / 2 - sizeX / 2, Game::kScreenHeight / 2 - sizeY / 2, m_startUIHandle, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (m_isCardSelectPause)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(Game::kGameLeftWidth, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (m_isPause)
	{
		DrawBox((Game::kScreenWidth - kPauseWidth) / 2, (Game::kScreenHeight - kPauseHeight) / 2, (Game::kScreenWidth - kPauseWidth) / 2 + kPauseWidth, (Game::kScreenHeight - kPauseHeight) / 2 + kPauseHeight, GetColor(200, 200, 200), true);
		DrawBox(m_cursorPos.X, m_cursorPos.Y, m_cursorPos.X + 50, m_cursorPos.Y + 50, GetColor(255, 255, 255), true);
		int sizeX, sizeY;
		GetGraphSize(m_pauseUIHandle, &sizeX, &sizeY);
		DrawGraph(Game::kScreenWidth / 2 - sizeX / 2, Game::kScreenHeight / 2 - sizeY - 100, m_pauseUIHandle, true);
		int retrySizeX, retrySizeY;
		GetGraphSize(m_retryUIHandle, &retrySizeX, &retrySizeY);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY, m_restartUIHandle, true);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY + kCursorMoveHeight, m_retryUIHandle, true);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY + (kCursorMoveHeight * 2), m_titleUIHandle, true);
	}
	
	if (m_gManager->IsGameClear()) // ゲームクリアになったら
	{
		DrawBox((Game::kScreenWidth - kPauseWidth) / 2, (Game::kScreenHeight - kPauseHeight) / 2, (Game::kScreenWidth - kPauseWidth) / 2 + kPauseWidth, (Game::kScreenHeight - kPauseHeight) / 2 + kPauseHeight, GetColor(200, 200, 200), true);
		DrawBox(m_cursorPos.X, m_cursorPos.Y + kCursorMoveHeight, m_cursorPos.X + 50, m_cursorPos.Y + kCursorMoveHeight + 50, GetColor(255, 255, 255), true);
		int scoreFontWidth = GetDrawFormatStringWidthToHandle(m_defaultFontHandle, "スコア:%d", (int)m_gManager->GetScore());
		int sizeX, sizeY;
		GetGraphSize(m_clearUIHandle, &sizeX, &sizeY);
		DrawGraph(Game::kScreenWidth / 2 - sizeX / 2, Game::kScreenHeight / 2 - sizeY - 100, m_clearUIHandle, true);
		int titleSizeX, titleSizeY, retrySizeX, retrySizeY;
		GetGraphSize(m_titleUIHandle, &titleSizeX, &titleSizeY);
		GetGraphSize(m_retryUIHandle, &retrySizeX, &retrySizeY);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY, m_scoreUIHandle, true);
		m_ui->DrawNumber(m_clearScore, kFirstCursorPosX + retrySizeX / 2 + kScoreWidth, kFirstCursorPosY, 0.5f);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY + kCursorMoveHeight, m_retryUIHandle, true);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY + (kCursorMoveHeight * 2), m_titleUIHandle, true);
	}
	if (m_gManager->IsGameOver()) // ゲームオーバーになったら
	{
		DrawBox((Game::kScreenWidth - kPauseWidth) / 2, (Game::kScreenHeight - kPauseHeight) / 2, (Game::kScreenWidth - kPauseWidth) / 2 + kPauseWidth, (Game::kScreenHeight - kPauseHeight) / 2 + kPauseHeight, GetColor(200, 200, 200), true);
		DrawBox(m_cursorPos.X, m_cursorPos.Y, m_cursorPos.X + 50, m_cursorPos.Y + 50, GetColor(255, 255, 255), true);
		int sizeX, sizeY;
		GetGraphSize(m_gameoverUIHandle, &sizeX, &sizeY);
		DrawGraph(Game::kScreenWidth / 2 - sizeX / 2, Game::kScreenHeight / 2 - sizeY - 100, m_gameoverUIHandle, true);
		int titleSizeX, titleSizeY, retrySizeX, retrySizeY;
		GetGraphSize(m_titleUIHandle, &titleSizeX, &titleSizeY);
		GetGraphSize(m_retryUIHandle, &retrySizeX, &retrySizeY);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY, m_retryUIHandle, true);
		DrawGraph(kFirstCursorPosX + retrySizeX / 2, kFirstCursorPosY + 100, m_titleUIHandle, true);
	}

}

void SceneMain::PauseUpdate()
{
	// カーソルの移動
	if (m_inputManager->GetKeyDown(KEY_INPUT_UP) || m_inputManager->GetPadDown(PAD_INPUT_UP))
	{
		m_sound->Play(m_cursorSeHandle);
		m_cursorPos.Y -= kCursorMoveHeight;
		if (m_cursorPos.Y < kFirstCursorPosY)
		{
			m_cursorPos.Y = kFirstCursorPosY + (kCursorMoveHeight * 2);
		}
	}
	if (m_inputManager->GetKeyDown(KEY_INPUT_DOWN) || m_inputManager->GetPadDown(PAD_INPUT_DOWN))
	{
		m_sound->Play(m_cursorSeHandle);
		m_cursorPos.Y += kCursorMoveHeight;
		if (m_cursorPos.Y > kFirstCursorPosY + (kCursorMoveHeight * 2))
		{
			m_cursorPos.Y = kFirstCursorPosY;
		}
	}
	// 決定キーでシーン遷移
	if (m_inputManager->GetKeyDown(KEY_INPUT_RETURN) || m_inputManager->GetPadDown(PAD_INPUT_1))
	{
		if (m_cursorPos.Y == kFirstCursorPosY)
		{
			m_sound->Play(m_pauseSeHandle);
			m_isPause = false;
		}
		else if (m_cursorPos.Y == kFirstCursorPosY + kCursorMoveHeight) // リトライ
		{
			m_sound->Play(m_selectSeHandle);
			m_nextScene = NextScene::SceneMain; // リトライ
			SetNextScene(m_nextScene);
			m_isEnd = true;
		}
		else if (m_cursorPos.Y == kFirstCursorPosY + (kCursorMoveHeight * 2)) // タイトルへ
		{
			m_sound->Play(m_selectSeHandle);
			m_nextScene = NextScene::SceneTitle; // タイトルへ遷移
			SetNextScene(m_nextScene);
			m_isEnd = true;
		}
	}
	m_player->ResetSpeed();
}

void SceneMain::ClearUpdate()
{
	if (m_clearScore < m_gManager->GetScore())
	{
		m_clearScore += 1000;
		if (m_clearScore > m_gManager->GetScore())
		{
			m_clearScore = (int)m_gManager->GetScore();
		}
		return;
	}


	// カーソルの移動
	if (m_inputManager->GetKeyDown(KEY_INPUT_UP) || m_inputManager->GetPadDown(PAD_INPUT_UP))
	{
		m_sound->Play(m_cursorSeHandle);
		m_cursorPos.Y -= kCursorMoveHeight;
		if (m_cursorPos.Y < kFirstCursorPosY)
		{
			m_cursorPos.Y = kFirstCursorPosY + kCursorMoveHeight;
		}
	}
	if (m_inputManager->GetKeyDown(KEY_INPUT_DOWN) || m_inputManager->GetPadDown(PAD_INPUT_DOWN))
	{
		m_sound->Play(m_cursorSeHandle);
		m_cursorPos.Y += kCursorMoveHeight;
		if (m_cursorPos.Y > kFirstCursorPosY + kCursorMoveHeight)
		{
			m_cursorPos.Y = kFirstCursorPosY;
		}
	}
	// 決定キーでシーン遷移
	if (m_inputManager->GetKeyDown(KEY_INPUT_RETURN) || m_inputManager->GetPadDown(PAD_INPUT_1))
	{
		if (m_cursorPos.Y == kFirstCursorPosY) // リトライ
		{
			m_sound->Play(m_selectSeHandle);
			m_nextScene = NextScene::SceneMain; // リトライ
			SetNextScene(m_nextScene);
			m_isEnd = true;
			
		}
		else if (m_cursorPos.Y == kFirstCursorPosY + kCursorMoveHeight) // タイトルへ
		{
			m_sound->Play(m_selectSeHandle);
			m_nextScene = NextScene::SceneTitle; // タイトルへ遷移
			SetNextScene(m_nextScene);
			m_isEnd = true;
		}
	}
}

void SceneMain::OverUpdate()
{
	// カーソルの移動
	if (m_inputManager->GetKeyDown(KEY_INPUT_UP) || m_inputManager->GetPadDown(PAD_INPUT_UP))
	{
		m_sound->Play(m_cursorSeHandle);
		m_cursorPos.Y -= kCursorMoveHeight;
		if (m_cursorPos.Y < kFirstCursorPosY)
		{
			m_cursorPos.Y = kFirstCursorPosY + kCursorMoveHeight;
		}
	}
	if (m_inputManager->GetKeyDown(KEY_INPUT_DOWN) || m_inputManager->GetPadDown(PAD_INPUT_DOWN))
	{
		m_sound->Play(m_cursorSeHandle);
		m_cursorPos.Y += kCursorMoveHeight;
		if (m_cursorPos.Y > kFirstCursorPosY + kCursorMoveHeight)
		{
			m_cursorPos.Y = kFirstCursorPosY;
		}
	}
	// 決定キーでシーン遷移
	if (m_inputManager->GetKeyDown(KEY_INPUT_RETURN) || m_inputManager->GetPadDown(PAD_INPUT_1))
	{
		if (m_cursorPos.Y == kFirstCursorPosY) // リトライ
		{
			m_sound->Play(m_selectSeHandle);
			m_nextScene = NextScene::SceneMain; // リトライ
			SetNextScene(m_nextScene);
			m_isEnd = true;
		}
		else if (m_cursorPos.Y == kFirstCursorPosY + kCursorMoveHeight) // タイトルへ
		{
			m_sound->Play(m_selectSeHandle);
			m_nextScene = NextScene::SceneTitle; // タイトルへ遷移
			SetNextScene(m_nextScene);
			m_isEnd = true;
		}
	}
}
