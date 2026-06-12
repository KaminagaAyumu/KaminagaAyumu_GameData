#include "PauseScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SceneController.h"
#include "../Common/Game.h"
#include "../Common/Input.h"
#include "../Common/UI/UIManager.h"
#include "../Common/UI/UIImage.h"
#include "../Common/UI/UISelectList.h"
#include "../MyLib/Physics.h"
#include "../Effect/EffectManager.h"
#include "../Sound/SoundManager.h"
#include "DxLib.h"

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kPopInterval = 20; // 元のシーンに戻る際のフェード時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr float kCrossFadeBGMTime = 1.0f; // BGMをクロスフェードさせる時間(秒換算)

	// 選択リストのカーソルが動く値
	constexpr int kCursorMoveIndex = 1;

	// 選択肢を表示する座標
	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };
	// 選択肢のサイズ
	const Vector2Int kSelectListSize = { 500, 250 };
}

PauseScene::PauseScene(SceneController& controller) : 
	SceneBase(controller),
	m_update(&PauseScene::FadeInUpdate),
	m_draw(&PauseScene::FadeDraw),
	m_isBackScene(false)
{
	// フェードの設定
	m_frameCount = kPopInterval;

	// 遷移設定を短い時間にする
	m_transitionInterval = kPopInterval;

	// 物理更新を止める
	MyLib::Physics::GetInstance().StopUpdate();
	// エフェクトの更新を止める
	EffectManager::GetInstance().StopUpdate();

	// UI管理クラスを取得
	auto& uiManager = UIManager::GetInstance();
	uiManager.LoadGraphHandle(L"backGround.png");
	m_pBackGround = uiManager.CreateImage(kSelectListPos, L"backGround.png");
	auto backGround = m_pBackGround.lock();
	backGround->SetLayer(UIBase::Layer::PauseBackGround);
	m_pSelectList = uiManager.CreateSelectList(kSelectListPos, kSelectListSize, FontType::Midium);

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("OK", L"Data/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false); // 決定時のSE
	soundManager.LoadSoundClip("Cancel", L"Data/Sound/SE/cancel.mp3", SoundBus::SE, 1.0f, false); // キャンセル時のSE
	soundManager.LoadSoundClip("CursorMove", L"Data/Sound/SE/cursorMove.mp3", SoundBus::SE, 1.0f, false); // カーソルが動く時のSE

	auto selectList = m_pSelectList.lock();
	selectList->AddOption(L"ポーズ解除", [this]()
		{
			// ポーズしたシーンに戻る
			m_controller.PopScene();
		});
	selectList->AddOption(L"リトライ", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// ゲームシーンに移行
			m_controller.ResetScene(std::make_shared<GameScene>(m_controller));
		});
	selectList->AddOption(L"タイトルにもどる", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// タイトルシーンに移行
			m_controller.ResetScene(std::make_shared<TitleScene>(m_controller));
		});
	// レイヤーをセット
	selectList->SetLayer(UIBase::Layer::Pause);
	UIManager::GetInstance().SetLayerAlpha(UIBase::Layer::PauseBackGround, 128);

}

PauseScene::~PauseScene()
{
	auto selectList = m_pSelectList.lock();
	selectList->End();

	UIManager::GetInstance().DeleteGraphHandle(L"backGround.png");

	// 物理更新を行う
	MyLib::Physics::GetInstance().StartUpdate();
	// エフェクトの更新を行う
	EffectManager::GetInstance().StartUpdate();


	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを開放
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cancel");
	soundManager.DeleteSoundClip("CursorMove");
}

void PauseScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void PauseScene::Draw()
{
	(this->*m_draw)();
}

void PauseScene::FadeInUpdate(Input&)
{
	// フレームを減少
	m_frameCount--;

	// フレームが0以下になったら
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_update = &PauseScene::NormalUpdate;
		m_draw = &PauseScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void PauseScene::NormalUpdate(Input& input)
{
	// 再びポーズボタンが押された際は元のシーンに戻る
	if (input.IsTriggered("Pause"))
	{
		// キャンセル時のSEを再生
		SoundManager::GetInstance().Play("Cancel", 1.0f, true);
		// フェードアウト
		m_update = &PauseScene::FadeOutUpdate;
		m_draw = &PauseScene::FadeDraw;
		m_isBackScene = true;
		m_transitionInterval = kPopInterval;
		return; // 念のため処理を抜ける
	}

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
		// フェードアウト
		m_update = &PauseScene::FadeOutUpdate;
		m_draw = &PauseScene::FadeDraw;

		auto selectList = m_pSelectList.lock();
		// シーン遷移時間を設定する
		if (selectList->IsMatchedCursor(L"ポーズ解除"))
		{
			// キャンセル時のSEを再生
			SoundManager::GetInstance().Play("Cancel", 1.0f, true);
			// 遷移時間を短めの時間にする
			m_transitionInterval = kPopInterval;
		}
		else // ポーズ解除時以外は完全なシーン遷移を行う
		{
			// 決定時のSEを再生
			SoundManager::GetInstance().Play("OK", 1.0f, true);
			// 遷移時間をフェードをする時間にする
			m_transitionInterval = kFadeInterval;
		}
		return; // 念のため処理を抜ける
	}
}

void PauseScene::FadeOutUpdate(Input&)
{
	// フレーム数を加算
	m_frameCount++;

	// フェードアウト時間を超えたら
	if (m_frameCount >= m_transitionInterval)
	{
		// 元のシーンに戻るかどうか
		if (m_isBackScene)
		{
			// 元のシーンに戻ることが確定している場合
			// 直接ポーズしたシーンに戻る
			m_controller.PopScene();
		}
		else
		{
			// 元のシーンに戻ることが確定していない場合
			// 選択肢に応じた処理を行う
			auto selectList = m_pSelectList.lock();
			selectList->TriggerSelect();
		}
		return; // 念のため処理を抜ける
	}
}

void PauseScene::NormalDraw()
{
	// UIの描画
	UIManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"PauseScene", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FPS:%.1f", GetFPS());
#endif // _DEBUG
}

void PauseScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_transitionInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}