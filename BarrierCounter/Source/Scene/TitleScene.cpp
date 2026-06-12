#include "TitleScene.h"
#include "GameScene.h"
#include "SceneController.h"
#include "../Actor/Model.h"
#include "../Common/Input.h"
#include "../Common/Application.h"
#include "../Common/Game.h"
#include "../Common/Camera/Camera.h"
#include "../Common/UI/UIManager.h"
#include "../Common/UI/UIText.h"
#include "../Common/UI/UIImage.h"
#include "../Common/UI/UISelectList.h"
#include "../Sound/SoundManager.h"
#include "../Geometry/Vector2Int.h"

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0x000000; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0x000000; // フェードアウトの色
	constexpr float kCrossFadeBGMTime = 1.5f; // BGMをクロスフェードさせる時間(秒換算)

	// 選択リストのカーソルが動く値
	constexpr int kCursorMoveIndex = 1;

	// ダイアログが動くフレーム
	constexpr int kDialogMoveFrame = 10;

	// タイトルの文字を表示する座標
	const Vector2Int kTitleImagePos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 120 };

	// タイトル画像のサイズ
	const Vector2 kTitleImageScale = { 1.0f,1.0f };

	// タイトルの説明を表示する座標
	const Vector2Int kTitleDescriptionTextPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 150 };

	// 選択肢を表示する座標
	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 130 };
	// 選択肢のサイズ
	const Vector2Int kSelectListSize = { 400, 200 };

	// スカイボックスの大きさ
	const Vector3 kSkyboxScale = { 75.0f,75.0f,75.0f };

	// スカイボックスの回転スピード
	constexpr float kSkyboxRotSpeed = 0.003f;

	// タイトルの説明を点滅させるフレーム数
	constexpr int kDescriptionTextBlinkFrame = 60;
}

TitleScene::TitleScene(SceneController& controller) : 
	SceneBase(controller),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	// フェードの設定
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;

	// スカイボックスの向きを初期状態にする
	m_skyboxDir = Vector3::Zero();

	// スカイボックスのハンドルを取得
	m_skyboxHandle = MV1LoadModel(L"Data/Model/skybox.mv1");

	// スカイボックスのモデルを作成
	m_pSkyboxModel = std::make_shared<Model>();
	m_pSkyboxModel->SetModelHandle(MV1DuplicateModel(m_skyboxHandle));
	m_pSkyboxModel->SetScale(kSkyboxScale);
	m_pSkyboxModel->SetDir(m_skyboxDir);
	m_pSkyboxModel->Init();

	// カメラを生成、初期化
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init();

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("TitleBGM", L"Data/Sound/BGM/titleBGM.mp3", SoundBus::BGM, 1.0f, true); // タイトルBGM
	//soundManager.LoadSoundClip("TitleBGM8Bit", L"Data/Sound/BGM/titleBGM_8bit.mp3", SoundBus::BGM, 1.0f, true); // タイトルBGM
	//soundManager.LoadSoundClip("TitleBGMVocal", L"Data/Sound/BGM/titleBGM_vocal.mp3", SoundBus::BGM, 1.0f, true); // タイトルBGM
	//soundManager.LoadSoundClip("TitleBGMInst", L"Data/Sound/BGM/titleBGM_inst.mp3", SoundBus::BGM, 1.0f, true); // タイトルBGM
	soundManager.LoadSoundClip("GameBGM", L"Data/Sound/BGM/gameSceneBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームシーンBGM
	soundManager.LoadSoundClip("OK", L"Data/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false); // 決定時のSE
	soundManager.LoadSoundClip("OpenDialog", L"Data/Sound/SE/openDialog.mp3", SoundBus::SE, 1.0f, false); // ダイアログを開く時のSE
	soundManager.LoadSoundClip("Cancel", L"Data/Sound/SE/cancel.mp3", SoundBus::SE, 1.0f, false); // キャンセル時のSE
	soundManager.LoadSoundClip("CursorMove", L"Data/Sound/SE/cursorMove.mp3", SoundBus::SE, 1.0f, false); // カーソルが動く時のSE

	// タイトルのBGMを再生
	soundManager.PlayBGM("TitleBGM", 0.0f);
	// UIマネージャーのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	// タイトルのテキストを描画
	//auto text = uiManager.CreateText(kTitleDescriptionTextPos, L"Aボタンでゲームスタート", FontType::Large);
	//auto pText = text.lock();
	//pText->StartBlinking(kDescriptionTextBlinkFrame); // 点滅するように設定
	//pText->SetTextColor(0x99ee99);

	// タイトルの画像をロード
	uiManager.LoadGraphHandle(L"title.png");
	// タイトルUIを生成
	auto titleImage = uiManager.CreateImage(kTitleImagePos, L"title.png");
	auto pTitleImage = titleImage.lock();
	pTitleImage->SetScale(kTitleImageScale);

	m_pSelectList = uiManager.CreateSelectList(kSelectListPos, kSelectListSize, FontType::Midium);
	auto selectList = m_pSelectList.lock();
	selectList->AddOption(L"ゲームスタート", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// ゲームシーンに移行
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		});
	selectList->AddOption(L"ゲームをおわる", []()
		{
			
		});

	m_pDialog = uiManager.CreateSelectList(Vector2Int{Game::kScreenWidth / 2, Game::kScreenHeight / 2}, kSelectListSize, FontType::Small);
	auto dialog = m_pDialog.lock();
	dialog->SetYesNoDialog(L"ゲームを終了しますか？", 
		// はいが選択された際の処理
		[this]()
		{
			// ゲーム終了処理を行う
			Application::GetInstance().RequestGameEnd();
		},
		// いいえが選択された際の処理
		[]()
		{
			
		});

	dialog->SetActive(false);

	// 初期状態はタイトルの選択状態
	m_mode = UIMode::TitleSelect;
	// 初期状態は何もなし
	m_fadeEndState = FadeEndState::None;
}

TitleScene::~TitleScene()
{
	// スカイボックスのモデルを消去
	MV1DeleteModel(m_skyboxHandle);
	m_pSkyboxModel->End();

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを開放
	soundManager.DeleteSoundClip("TitleBGM");
	//soundManager.DeleteSoundClip("TitleBGM8Bit");
	//soundManager.DeleteSoundClip("TitleBGMVocal");
	//soundManager.DeleteSoundClip("TitleBGMInst");
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("OpenDialog");
	soundManager.DeleteSoundClip("Cancel");
	soundManager.DeleteSoundClip("CursorMove");

	// タイトル画像ハンドルを開放
	UIManager::GetInstance().DeleteGraphHandle(L"title.png");
}

void TitleScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void TitleScene::Draw()
{
	(this->*m_draw)();
}

void TitleScene::FadeInUpdate(Input&)
{
	// フレームを減少
	m_frameCount--;

	// フレームが0以下になったら
	if (m_frameCount <= 0)
	{
		// フェードイン完了
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	// フレーム数を加算
	m_frameCount++;

	m_skyboxDir.x = cosf(m_frameCount * kSkyboxRotSpeed);
	m_skyboxDir.y = 0.0f;
	m_skyboxDir.z = sinf(m_frameCount * kSkyboxRotSpeed);
	m_skyboxDir.Normalize();

	// スカイボックスの更新
	m_pSkyboxModel->Update();

	// スカイボックスの向きの更新
	m_pSkyboxModel->SetDir(m_skyboxDir);

	switch (m_mode)
	{
	case UIMode::TitleSelect:
		UpdateTitleSelect(input);
		break;
	case UIMode::YesNoDialog:
		UpdateYesNoDialog(input);
		break;
	default:
		break;
	}
}

void TitleScene::FadeOutUpdate(Input&)
{
	// フレーム数を加算
	m_frameCount++;

	// フェードアウト時間を超えたら
	if (m_frameCount >= kFadeInterval)
	{
		if (m_fadeEndState == FadeEndState::GameStart)
		{
			auto selectList = m_pSelectList.lock();
			selectList->TriggerSelect();
		}
		else
		{
			auto dialog = m_pDialog.lock();
			dialog->TriggerSelect();
		}
		return; // 念のため処理を抜ける
	}
}

void TitleScene::NormalDraw()
{
	// スカイボックスの描画
	m_pSkyboxModel->Draw();

	// UIの描画
	UIManager::GetInstance().Draw();

	//DrawLine(Game::kScreenWidth / 2, 0, Game::kScreenWidth / 2, Game::kScreenHeight, 0xff0000);
#ifdef _DEBUG
	DrawString(0, 0, L"TitleScene", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FPS:%.1f", GetFPS());
#endif // _DEBUG
}

void TitleScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::UpdateTitleSelect(Input& input)
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
		auto selectList = m_pSelectList.lock();
		// カーソルがゲームスタートになっている場合
		if (selectList->IsMatchedCursor(L"ゲームスタート"))
		{
			m_fadeEndState = FadeEndState::GameStart;

			// フレーム数を初期化
			m_frameCount = 0;
			// BGMを変更
			SoundManager::GetInstance().CrossFadeBGM("GameBGM", kCrossFadeBGMTime);
			// 決定時のSEを再生
			SoundManager::GetInstance().Play("OK", 1.0f, false);
			m_update = &TitleScene::FadeOutUpdate;
			m_draw = &TitleScene::FadeDraw;
			m_fadeColor = kFadeOutColor;
		}
		else
		{
			// ダイアログを開く時のSEを再生
			SoundManager::GetInstance().Play("OpenDialog", 1.0f, true);
			m_mode = UIMode::YesNoDialog;
			auto dialog = m_pDialog.lock();
			dialog->SetActive(true);
			dialog->StartAppearCenter(kDialogMoveFrame);
		}
		return; // 念のため処理を抜ける
	}

}

void TitleScene::UpdateYesNoDialog(Input& input)
{
	// 選択肢のカーソルを移動
	if (input.IsTriggered("Left"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("CursorMove", 1.0f, true);
		auto dialog = m_pDialog.lock();
		dialog->MoveCursor(-kCursorMoveIndex);
	}
	if (input.IsTriggered("Right"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("CursorMove", 1.0f, true);
		auto dialog = m_pDialog.lock();
		dialog->MoveCursor(kCursorMoveIndex);
	}

	if (input.IsTriggered("OK"))
	{
		auto dialog = m_pDialog.lock();
		// カーソルがゲームスタートになっている場合
		if (dialog->IsMatchedCursor(L"はい"))
		{
			m_fadeEndState = FadeEndState::GameEnd;

			// フレーム数を初期化
			m_frameCount = 0;
			// BGMを変更
			//SoundManager::GetInstance().CrossFadeBGM("GameBGM", kCrossFadeBGMTime);
			// 決定時のSEを再生
			SoundManager::GetInstance().Play("OK", 1.0f, false);
			m_update = &TitleScene::FadeOutUpdate;
			m_draw = &TitleScene::FadeDraw;
			m_fadeColor = kFadeOutColor;
		}
		else
		{
			// キャンセル時のSEを再生
			SoundManager::GetInstance().Play("Cancel", 1.0f, true);
			m_mode = UIMode::TitleSelect;
			auto dialog = m_pDialog.lock();
			dialog->StartCloseCenter(kDialogMoveFrame, false);
		}
		return; // 念のため処理を抜ける
	}
}
