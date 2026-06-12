#include "ResultScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SceneController.h"
#include "../Actor/Model.h"
#include "../Common/Game.h"
#include "../Common/Input.h"
#include "../Common/GameManager.h"
#include "../Common/Camera/Camera.h"
#include "../Common/UI/UIManager.h"
#include "../Common/UI/UIText.h"
#include "../Common/UI/UIImage.h"
#include "../Common/UI/UISelectList.h"
#include "../Sound/SoundManager.h"
#include "DxLib.h"
#include <algorithm>

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

	// 背景がループするスピード(フレーム数)
	constexpr int kBackGroundScrollFrame = 1;

	// 選択リストが表示される際のフレーム
	constexpr int kSelectListAppearFrame = 10;

	// 背景を描画する座標
	const Vector2Int kBackGroundPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };

	// 選択肢を表示する座標
	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 130 };
	// 選択肢のサイズ
	const Vector2Int kSelectListSize = { 450, 200 };

	// スコアを加算する割合
	constexpr float kScoreAddRate = 0.15f;

	// 加算割合の閾値
	constexpr float kScoreThreshold = 0.9f;

	// クリアタイムを表示する際に待つ時間
	constexpr int kTimeResultWaitFrame = 30;

	// スコアボーナスを表示する際に待つ時間
	constexpr int kBonusScoreWaitFrame = 30;

	// リザルトのテキストを表示する位置
	const Vector2Int kResultTextPos = { Game::kScreenWidth / 2 , 100 };
	// クリア時間を表示する位置
	const Vector2Int kTimeTextPos = { Game::kScreenWidth / 2 , 250 };
	// ボーナススコアを表示する位置
	const Vector2Int kBonusScoreTextPos = { Game::kScreenWidth / 2 + 30, 350 };
	// スコアを表示する位置
	const Vector2Int kScoreTextPos = { Game::kScreenWidth / 2 , 400 };

	// プレイヤーのモデルを表示する座標
	const Vector3 kPlayerModelPos = { 500.0f, 0.0f, 0.0f };

	// UIをフェードインさせる際のフレーム数
	constexpr int kFadeInFrame = 10;

	// クリア時にデフォルトでもらえるスコア
	constexpr int kDefaultScore = 1000;

	// このシーンで使用するカメラの設定関連
	// 視野角(度数)
	constexpr float kFovDegree = 60.0f;
	// 半円の角度(視野角をラジアンに変換する際に使う)
	constexpr float kHalfCircleDegree = 180.0f;
	// Near,far
	constexpr float kCameraNear = 8.0f;
	constexpr float kCameraFar = 14400.0f;
	// カメラの座標とターゲット
	const Vector3 kCameraPos = { 0.0f,300.0f,-800.0f };
	const Vector3 kCameraTarget = { 0.0f,300.0f,0.0f };
}

ResultScene::ResultScene(SceneController& controller) : 
	SceneBase(controller),
	m_update(&ResultScene::FadeInUpdate),
	m_draw(&ResultScene::FadeDraw),
	m_resultScore(0),
	m_timeBonusScore(0),
	m_resultTime(0),
	m_waitFrame(0),
	m_isTimeResultEnd(false),
	m_isPlayerReactionEnd(false)
{
	// フェードの設定
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;

	// 視野角の設定
	float fovRadian = kFovDegree * (DX_PI_F / kHalfCircleDegree);
	SetupCamera_Perspective(fovRadian);
	SetCameraNearFar(kCameraNear, kCameraFar);
	SetCameraPositionAndTarget_UpVecY(kCameraPos.ToDxLibVector(), kCameraTarget.ToDxLibVector());

	// プレイヤーのモデルをロード
	m_playerModelHandle = MV1LoadModel(L"Data/Model/player.mv1");
	// モデルクラスを取得
	m_pPlayerModel = std::make_shared<Model>();
	// プレイヤーのモデルハンドルをモデルクラスにセット
	m_pPlayerModel->SetModelHandle(MV1DuplicateModel(m_playerModelHandle));
	m_pPlayerModel->Init();
	m_pPlayerModel->SetAnimation(3, 0.5f, 0, true);
	m_pPlayerModel->SetPos(kPlayerModelPos);
	// モデルの向きをカメラ目線にする
	m_pPlayerModel->SetDir((kCameraPos - kPlayerModelPos).Normalized());

	// ゲームマネージャーを取得
	auto pGameManager = m_controller.GetGameManager().lock();
	// スコアボーナスの時間を取得する
	m_scoreBonusMaxTime = pGameManager->GetBonusMaxTime();

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("GameBGM", L"Data/Sound/BGM/gameSceneBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームシーンBGM
	soundManager.LoadSoundClip("ResultBGM", L"Data/Sound/BGM/resultBGM.mp3", SoundBus::BGM, 1.0f, true); // リザルトBGM
	soundManager.LoadSoundClip("OK", L"Data/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false); // 決定時のSE
	soundManager.LoadSoundClip("Cancel", L"Data/Sound/SE/cancel.mp3", SoundBus::SE, 1.0f, false); // キャンセル時のSE
	soundManager.LoadSoundClip("CursorMove", L"Data/Sound/SE/cursorMove.mp3", SoundBus::SE, 1.0f, false); // カーソルが動く時のSE
	soundManager.LoadSoundClip("AddScore", L"Data/Sound/SE/addScore.mp3", SoundBus::SE, 1.0f, true); // スコア加算時のSE

	// UIマネージャーのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();

	// 画像ハンドルをロード
	uiManager.LoadGraphHandle(L"backGround_2.png"); // 背景の画像
	uiManager.LoadGraphHandle(L"backGround_logo.png"); // 文字の画像
	// UIを生成
	uiManager.CreateImage(kBackGroundPos, L"backGround_2.png");
	auto backGround = uiManager.CreateImage(Vector2Int::Zero(), L"backGround_logo.png").lock();
	backGround->SetLoopBackGround(kBackGroundScrollFrame);


	uiManager.CreateText(kResultTextPos, L"ステージクリア！", FontType::Header);

	// リザルト関連のテキストを生成
	auto timeText = uiManager.CreateText(kTimeTextPos, L"クリアタイム:", FontType::Large).lock();
	timeText->SetAlignment(UIText::AlignmentType::Right); // 右詰めにする
	timeText->SetActive(false); // 初期状態は非表示にする
	m_resultTexts["ClearTime"].push_back(timeText);

	auto scoreText = uiManager.CreateText(kScoreTextPos, L"スコア:", FontType::Large).lock();
	scoreText->SetAlignment(UIText::AlignmentType::Right); // 右詰めにする
	scoreText->SetActive(false); // 初期状態は非表示にする
	m_resultTexts["Score"].push_back(scoreText);

	auto bonusText = uiManager.CreateText(kBonusScoreTextPos, L"タイムボーナス! +", FontType::Small).lock();
	bonusText->SetAlignment(UIText::AlignmentType::Right); // 右詰めにする
	bonusText->SetActive(false); // 初期状態は非表示にする
	m_resultTexts["TimeBonus"].push_back(bonusText);

	m_pTimeText = uiManager.CreateText(kTimeTextPos, std::to_wstring(m_resultTime / 60), FontType::Large);
	auto time = m_pTimeText.lock();
	time->SetAlignment(UIText::AlignmentType::Left); // 左詰めにする
	time->SetActive(false); // 初期状態は非表示にする
	m_resultTexts["ClearTime"].push_back(time);

	m_pScoreText = uiManager.CreateText(kScoreTextPos, std::to_wstring(m_resultScore), FontType::Large);
	auto score = m_pScoreText.lock();
	score->SetAlignment(UIText::AlignmentType::Left); // 左詰めにする
	score->SetActive(false); // 初期状態は非表示にする
	m_resultTexts["Score"].push_back(score);

	m_pBonusScoreText = uiManager.CreateText(kBonusScoreTextPos, std::to_wstring(m_timeBonusScore), FontType::Small);
	auto bonusScore = m_pBonusScoreText.lock();
	bonusScore->SetAlignment(UIText::AlignmentType::Left); // 左詰めにする
	bonusScore->SetActive(false); // 初期状態は非表示にする
	m_resultTexts["TimeBonus"].push_back(bonusScore);


	m_pSelectList = uiManager.CreateSelectList(kSelectListPos, kSelectListSize, FontType::Midium);
	auto selectList = m_pSelectList.lock();
	selectList->AddOption(L"次のステージへ", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// ゲームシーンに移行
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		});
	selectList->AddOption(L"タイトルにもどる", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// タイトルシーンに移行
			m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		});
	// 最初は非表示にしておく
	selectList->SetActive(false);


	CheckBonusScore();
}

ResultScene::~ResultScene()
{
	// モデルの終了処理を行う
	MV1DeleteModel(m_playerModelHandle);
	m_pPlayerModel->End();

	// 登録したサウンドを開放
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("ResultBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cancel");
	soundManager.DeleteSoundClip("CursorMove");
	soundManager.DeleteSoundClip("AddScore");

	// 登録したUIを開放
	auto& uiManager = UIManager::GetInstance();
	uiManager.DeleteGraphHandle(L"backGround_2.png");
	uiManager.DeleteGraphHandle(L"backGround_logo.png");

}

void ResultScene::Update(Input& input)
{
	// プレイヤーのモデルを更新
	m_pPlayerModel->Update();

	(this->*m_update)(input);
}

void ResultScene::Draw()
{
	(this->*m_draw)();
}

void ResultScene::FadeInUpdate(Input&)
{
	// フレームを減少
	m_frameCount--;

	// フレームが0以下になったら
	if (m_frameCount <= 0)
	{
		ActiveText("ClearTime");
		// フェードイン完了
		m_update = &ResultScene::ClearTimeUpdate;
		m_draw = &ResultScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void ResultScene::NormalUpdate(Input& input)
{
	if (m_pPlayerModel->GetAnimationEnd() && !m_isPlayerReactionEnd)
	{
		m_isPlayerReactionEnd = true;
		m_pPlayerModel->SetAnimation(3, 0.5f, 0, true);
	}

	if (input.IsTriggered("PowerUp") && m_isPlayerReactionEnd)
	{
		m_isPlayerReactionEnd = false;
		m_pPlayerModel->SetAnimation(16, 0.5f, 10, false);
	}

	// リザルトで使用するテキストクラスを更新する
	auto timeText = m_pTimeText.lock();
	timeText->SetText(std::to_wstring(m_resultTime / 60));

	auto bonusScoreText = m_pBonusScoreText.lock();
	bonusScoreText->SetText(std::to_wstring(m_timeBonusScore));

	auto scoreText = m_pScoreText.lock();
	scoreText->SetText(std::to_wstring(m_resultScore));

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
		// 次のステージへ移行する場合
		if (selectList->IsMatchedCursor(L"次のステージへ"))
		{
			// BGMを変更
			SoundManager::GetInstance().CrossFadeBGM("GameBGM", kCrossFadeBGMTime);
		}
		// 決定時のSEを再生
		SoundManager::GetInstance().Play("OK", 1.0f, false);
		m_update = &ResultScene::FadeOutUpdate;
		m_draw = &ResultScene::FadeDraw;
		m_fadeColor = kFadeOutColor;
		
		return; // 念のため処理を抜ける
	}
}

void ResultScene::FadeOutUpdate(Input&)
{
	// フレーム数を加算
	m_frameCount++;

	// フェードアウト時間を超えたら
	if (m_frameCount >= kFadeInterval)
	{
		auto selectList = m_pSelectList.lock();

		// タイトルに戻るときに行う処理
		if (selectList->IsMatchedCursor(L"タイトルにもどる"))
		{
			// ゲームマネージャーのweak_ptrを取得
			auto wpGameManager = m_controller.GetGameManager();
			auto pGameManager = wpGameManager.lock();

			// ゲームマネージャーの状態をリセットする
			pGameManager->Init();
		}

		selectList->TriggerSelect();
		return; // 念のため処理を抜ける
	}
}

void ResultScene::ClearTimeUpdate(Input&)
{
	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	// クリアタイムを取得
	int currentTime = pGameManager->GetClearTime();

	// タイムの更新処理
	if (m_resultTime <= currentTime)
	{
		// 更新時のSEを再生
		SoundManager::GetInstance().Play("AddScore", 1.0f, false);

		// 加算する時間の量を取得
		int add = static_cast<int>((currentTime - m_resultTime) * kScoreAddRate);

		// 時間の量の割合が閾値を超えたらクリアタイムを実際のデータに合わせる
		if (add <= kScoreThreshold)
		{
			// 更新時のSEを止める
			SoundManager::GetInstance().Stop("AddScore");
			
			// リザルトに表示する時間を実際のデータにする
			m_resultTime = currentTime;

			// クリアタイムの更新が終わったとする
			m_isTimeResultEnd = true;
		}
		else // スコアを加算していく
		{
			m_resultTime += add;
		}
		// 描画用のスコアが現在のスコアを超えないようにする
		if (m_resultTime > currentTime)
		{
			// 更新時のSEを止める
			SoundManager::GetInstance().Stop("AddScore");
			m_resultTime = currentTime;
		}
	}

	auto timeText = m_pTimeText.lock();
	timeText->SetText(std::to_wstring(m_resultTime / 60));

	// クリアタイムの更新が終わったら
	if (m_isTimeResultEnd)
	{
		// 待機時間を加算
		m_waitFrame++;

		// 待機時間が終わったらスコアを表示する処理に移行
		if (m_waitFrame >= kTimeResultWaitFrame)
		{
			m_waitFrame = 0;
			m_update = &ResultScene::ScoreUpdate;
			ActiveText("Score");
		}
	}
}

void ResultScene::ScoreUpdate(Input&)
{
	m_waitFrame++;

	if (m_waitFrame >= kBonusScoreWaitFrame)
	{
		m_update = &ResultScene::BonusScoreUpdate;
		ActiveText("TimeBonus");
		return;
	}

	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	m_resultScore = pGameManager->GetTotalScore();

	auto scoreText = m_pScoreText.lock();
	scoreText->SetText(std::to_wstring(m_resultScore));
}

void ResultScene::BonusScoreUpdate(Input& input)
{
	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	// タイムボーナスのスコアを取得
	m_timeBonusScore = std::max(0.0f, (m_scoreBonusMaxTime - static_cast<int>(pGameManager->GetClearTime() / 60)) * kDefaultScore);

	auto bonusScoreText = m_pBonusScoreText.lock();
	bonusScoreText->SetText(std::to_wstring(m_timeBonusScore));

	if (input.IsTriggered("OK"))
	{
		
		m_update = &ResultScene::ScoreAddUpdate;
		return;
	}
}

void ResultScene::ScoreAddUpdate(Input& input)
{
	auto scoreText = m_pScoreText.lock();
	
	// スコアが表示中なら更新をしない
	if (scoreText->IsMoving())
	{
		return;
	}

	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	m_resultTime = pGameManager->GetClearTime();

	int currentScore = pGameManager->GetTotalScore() + std::max(0.0f, (m_scoreBonusMaxTime - static_cast<int>(pGameManager->GetClearTime() / 60)) * kDefaultScore);

	// スコアの更新処理
	if (m_resultScore < currentScore)
	{
		// 更新時のSEを再生
		SoundManager::GetInstance().Play("AddScore", 1.0f, false);

		// 加算するスコアの量を取得
		int add = static_cast<int>((currentScore - m_resultScore) * kScoreAddRate);

		// スコアの量の割合が閾値を超えたらスコアを現在のスコアに合わせる
		if (add <= kScoreThreshold)
		{
			// 更新時のSEを止める
			SoundManager::GetInstance().Stop("AddScore");
			m_resultScore = currentScore;
			m_timeBonusScore = 0;
		}
		else // スコアを加算していく
		{
			m_resultScore += add;
			m_timeBonusScore -= add;
		}
		// 描画用のスコアが現在のスコアを超えないようにする
		if (m_resultScore > currentScore)
		{
			// 更新時のSEを止める
			SoundManager::GetInstance().Stop("AddScore");
			m_resultScore = currentScore;
			m_timeBonusScore = 0;
		}
	}

	auto bonusScoreText = m_pBonusScoreText.lock();
	bonusScoreText->SetText(std::to_wstring(m_timeBonusScore));
	
	scoreText->SetText(std::to_wstring(m_resultScore));

	if (input.IsTriggered("OK"))
	{
		// リザルトのスコアの更新が終わっていなければ
		if (m_resultScore < currentScore)
		{
			// 更新時のSEを止める
			SoundManager::GetInstance().Stop("AddScore");

			// タイムボーナスを加算し終わったとする
			m_timeBonusScore = 0;
			// スコアを最新のものにして処理を抜ける
			m_resultScore = currentScore;
			return;
		}

		// スコアをリザルトのものに設定
		pGameManager->AddScore(std::max(0.0f, (m_scoreBonusMaxTime - static_cast<int>(pGameManager->GetClearTime() / 60)) * kDefaultScore));
		// 現在のステージのデータをリセット
		pGameManager->ResetCurrentStageData();

		auto selectList = m_pSelectList.lock();
		selectList->SetActive(true);
		selectList->StartAppearCenter(kSelectListAppearFrame);

		m_pPlayerModel->SetAnimation(17, 0.5f, 30, false);

		m_update = &ResultScene::NormalUpdate;
	}
}

void ResultScene::NormalDraw()
{
	// UIの描画
	UIManager::GetInstance().Draw();

	m_pPlayerModel->Draw();

#ifdef _DEBUG
	DrawString(0, 0, L"ResultScene", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FPS:%.1f", GetFPS());
#endif // _DEBUG
}

void ResultScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ResultScene::CheckBonusScore()
{
	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	// ボーナススコアを計算
	m_timeBonusScore = pGameManager->GetTotalScore() + std::max(0.0f, (m_scoreBonusMaxTime - static_cast<int>(pGameManager->GetClearTime() / 60)) * kDefaultScore);
}

void ResultScene::ActiveText(std::string activeText)
{
	for (auto text : m_resultTexts[activeText])
	{
		auto pText = text.lock();
		pText->SetActive(true);
		pText->StartFadeIn(kFadeInFrame);
	}
}
