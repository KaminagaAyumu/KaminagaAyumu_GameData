#include "GameoverScene.h"
#include "SceneController.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "../Common/UI/UIManager.h"
#include "../Common/UI/UIText.h"
#include "../Common/UI/UISelectList.h"
#include "../Common/Input.h"
#include "../Common/Game.h"
#include "../Common/GameManager.h"
#include "../Sound/SoundManager.h"
#include "DxLib.h"

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0xffffff; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0x000000; // フェードアウトの色
	constexpr float kCrossFadeBGMTime = 1.0f; // BGMをクロスフェードさせる時間(秒換算)

	// ゲームオーバーの文字を表示する位置
	const Vector2Int kGameoverTextPos = { Game::kScreenWidth / 2, 200 };

	// 選択肢を表示する座標
	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 130 };
	// 選択肢のサイズ
	const Vector2Int kSelectListSize = { 400, 200 };
	// 選択リストが表示される際のフレーム
	constexpr int kSelectListAppearFrame = 10;

	// 選択リストのカーソルが動く値
	constexpr int kCursorMoveIndex = 1;

}

GameoverScene::GameoverScene(SceneController& controller) :
	SceneBase(controller),
	m_update(&GameoverScene::FadeInUpdate),
	m_draw(&GameoverScene::FadeDraw)
{
	// フェードの設定
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("GameBGM", L"Data/Sound/BGM/gameSceneBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームシーンBGM
	soundManager.LoadSoundClip("GameoverBGM", L"Data/Sound/BGM/gameoverBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームオーバーBGM
	soundManager.LoadSoundClip("OK", L"Data/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false); // 決定時のSE
	soundManager.LoadSoundClip("Cancel", L"Data/Sound/SE/cancel.mp3", SoundBus::SE, 1.0f, false); // キャンセル時のSE
	soundManager.LoadSoundClip("CursorMove", L"Data/Sound/SE/cursorMove.mp3", SoundBus::SE, 1.0f, false); // カーソルが動く時のSE


	// UIマネージャーのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	auto gameoverText = uiManager.CreateText(kGameoverTextPos, L"ゲームオーバー!", FontType::Header);
	auto pGameoverText = gameoverText.lock();
	pGameoverText->SetTextColor(0xfe4444);

	m_pSelectList = uiManager.CreateSelectList(kSelectListPos, kSelectListSize, FontType::Midium);
	auto pSelectList = m_pSelectList.lock();
	pSelectList->AddOption(L"リトライ", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// ゲームシーンに移行
			m_controller.ResetScene(std::make_shared<GameScene>(m_controller));
		});
	pSelectList->AddOption(L"タイトルにもどる", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// タイトルシーンに移行
			m_controller.ResetScene(std::make_shared<TitleScene>(m_controller));
		});
	// 最初は非表示にしておく
	pSelectList->SetActive(false);
}

GameoverScene::~GameoverScene()
{
	// 登録したサウンドを開放
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("GameoverBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cancel");
	soundManager.DeleteSoundClip("CursorMove");
}

void GameoverScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void GameoverScene::Draw()
{
	(this->*m_draw)();
}

void GameoverScene::FadeInUpdate(Input&)
{
	// フレームを減少
	m_frameCount--;

	// フレームが0以下になったら
	if (m_frameCount <= 0)
	{
		// 選択肢を表示する処理を行う
		auto pSelectList = m_pSelectList.lock();
		pSelectList->SetActive(true);
		pSelectList->StartAppearCenter(kSelectListAppearFrame);

		// フェードイン完了
		m_update = &GameoverScene::NormalUpdate;
		m_draw = &GameoverScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void GameoverScene::NormalUpdate(Input& input)
{

	// 選択肢のカーソルを移動
	if (input.IsTriggered("Up"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("CursorMove", 1.0f, true);
		auto selectList = m_pSelectList.lock();
		selectList->MoveCursor(-kCursorMoveIndex);
	}
	if (input.IsTriggered("Down"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("CursorMove", 1.0f, true);
		auto selectList = m_pSelectList.lock();
		selectList->MoveCursor(kCursorMoveIndex);
	}

	if (input.IsTriggered("OK"))
	{
		// BGMを変更
		SoundManager::GetInstance().CrossFadeBGM("GameBGM", kCrossFadeBGMTime);
		// 決定時のSEを再生
		SoundManager::GetInstance().Play("OK", 1.0f, false);
		m_update = &GameoverScene::FadeOutUpdate;
		m_draw = &GameoverScene::FadeDraw;
		m_fadeColor = kFadeOutColor;

		return; // 念のため処理を抜ける
	}
}

void GameoverScene::FadeOutUpdate(Input&)
{
	// フレーム数を加算
	m_frameCount++;

	// フェードアウト時間を超えたら
	if (m_frameCount >= kFadeInterval)
	{
		// ステージの進行状況をリセットする
		auto pGameManager = m_controller.GetGameManager().lock();
		pGameManager->Init();

		// 選択リストを取得する
		auto selectList = m_pSelectList.lock();
		// 選択されている内容を実行する
		selectList->TriggerSelect();

		return; // 念のため処理を抜ける
	}
}

void GameoverScene::NormalDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x111155, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// UIの描画
	UIManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"GameoverScene", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FPS:%.1f", GetFPS());
#endif // _DEBUG
}

void GameoverScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}