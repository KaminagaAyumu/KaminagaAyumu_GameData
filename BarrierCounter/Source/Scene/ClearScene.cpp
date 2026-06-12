#include "ClearScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SceneController.h"
#include "../Actor/Model.h"
#include "../Common/Game.h"
#include "../Common/Input.h"
#include "../Common/GameManager.h"
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
	constexpr unsigned int kFadeInColor = 0xffffff; // フェードインの色
	constexpr unsigned int kFadeOutColor = 0x000000; // フェードアウトの色
	constexpr float kCrossFadeBGMTime = 1.5f; // BGMをクロスフェードさせる時間(秒換算)

	// 選択リストのカーソルが動く値
	constexpr int kCursorMoveIndex = 1;

	// 選択リストが表示される際のフレーム
	constexpr int kSelectListAppearFrame = 10;

	// スコアを加算する割合
	constexpr float kScoreAddRate = 0.15f;

	// 加算割合の閾値
	constexpr float kScoreThreshold = 0.9f;

	// クリアタイムを表示する際に待つ時間
	constexpr int kTimeResultWaitFrame = 30;

	// スコアボーナスを表示する際に待つ時間
	constexpr int kBonusScoreWaitFrame = 30;

	// モデルが動くスピード
	constexpr float kModelMoveSpeed = 7.0f;

	// 背景がループするスピード(フレーム数)
	constexpr int kBackGroundScrollFrame = 1;

	// 背景を描画する座標
	const Vector2Int kBackGroundPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };

	// プレイヤーのモデルの初期位置
	const Vector3 kPlayerModelFirstPos = { 0.0f,0.0f,0.0f };
	// 敵のモデルの初期位置
	const Vector3 kEnemyModelFirstPos = { 500.0f,0.0f,0.0f };

	// モデルの初期の向き
	const Vector3 kModelFirstMoveDir = { 1.0f,0.0f,0.0f };

	// クリアの文字を表示する位置
	const Vector2Int kClearTextPos = { Game::kScreenWidth / 2, 100 };

	// クリア時間を表示する位置
	const Vector2Int kTimeTextPos = { Game::kScreenWidth / 2 , 230 };

	// ボーナススコアを表示する位置
	const Vector2Int kBonusScoreTextPos = { Game::kScreenWidth / 2 + 30, 310 };

	// スコアを表示する位置
	const Vector2Int kScoreTextPos = { Game::kScreenWidth / 2 , 360 };

	// レコードを表示するテキストを格納するボックス画像の座標
	const Vector2Int kRecordBoxImagePos = { Game::kScreenWidth / 2, 400 };

	// レコードのステージ名を表示するテキストの位置
	const Vector2Int kRecordStageTextPos = { 500,200 };

	// レコードの時間を表示するテキストの位置
	const Vector2Int kRecordTimeTextPos = { 500, 400 };
	// レコードのスコアを表示するテキストの位置
	const Vector2Int kRecordScoreTextPos = { 500, 600 };

	// レコードのステージ名を表示するテキストのマージン
	const Vector2Int kRecordStageTextMargin = { 150,0 };

	// レコードのテキストのマージン
	const Vector2Int kRecordTextMargin = { 300,0 };

	// スコアボーナスをもらえる最大時間
	constexpr float kScoreBonusMaxTime = 40.0f;

	// クリア時にデフォルトでもらえるスコア
	constexpr int kDefaultScore = 1000;

	// 選択肢を表示する座標
	const Vector2Int kSelectListPos = { Game::kScreenWidth / 2, Game::kScreenHeight - 160 };
	// 選択肢のサイズ
	const Vector2Int kSelectListSize = { 450, 250 };

	// UIをフェードインさせる際のフレーム数
	constexpr int kFadeInFrame = 10;
	// UIをフェードアウトさせる際のフレーム数
	constexpr int kFadeOutFrame = 10;

	// モデルが画面端に行って折り返すまでのフレーム数
	constexpr int kModelTurnMaxFrame = 120;

	// モデルが折り返すまでの座標
	constexpr float kModelTurnX = 1600.0f;


	// このシーンで使用するカメラの設定関連
	// 視野角(度数)
	constexpr float kFovDegree = 60.0f;
	// 半円の角度(視野角をラジアンに変換する際に使う)
	constexpr float kHalfCircleDegree = 180.0f;
	// Near,far
	constexpr float kCameraNear = 8.0f;
	constexpr float kCameraFar = 14400.0f;
	// カメラの座標とターゲット
	const Vector3 kCameraPos = { 0.0f,350.0f,-800.0f };
	const Vector3 kCameraTarget = { 0.0f,300.0f,0.0f };
}

ClearScene::ClearScene(SceneController& controller) : 
	SceneBase(controller),
	m_update(&ClearScene::FadeInUpdate),
	m_draw(&ClearScene::FadeDraw),
	m_resultScore(0),
	m_timeBonusScore(0),
	m_resultTime(0),
	m_waitFrame(0),
	m_isTimeResultEnd(false)
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
	// 敵のモデルをロード
	m_enemyModelHandle = MV1LoadModel(L"Data/Model/moveEnemy.mv1");

	// プレイヤーと敵の初期座標を設定
	m_playerModelPos = kPlayerModelFirstPos;
	m_enemyModelPos = kEnemyModelFirstPos;

	// プレイヤー用モデルクラスを取得
	m_pPlayerModel = std::make_shared<Model>();
	// プレイヤーのモデルハンドルをモデルクラスにセット
	m_pPlayerModel->SetModelHandle(MV1DuplicateModel(m_playerModelHandle));
	m_pPlayerModel->Init();
	m_pPlayerModel->SetAnimation(11, 0.5f, 0, true);
	m_pPlayerModel->SetPos(m_playerModelPos);

	// 敵用モデルクラスを取得
	m_pEnemyModel = std::make_shared<Model>();
	// プレイヤーのモデルハンドルをモデルクラスにセット
	m_pEnemyModel->SetModelHandle(MV1DuplicateModel(m_enemyModelHandle));
	m_pEnemyModel->Init();
	m_pEnemyModel->SetAnimation(8, 0.5f, 0, true);
	m_pEnemyModel->SetPos(m_enemyModelPos);

	// 動く向きを設定
	m_moveDir = kModelFirstMoveDir;

	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();
	// スコアボーナスの時間を取得する
	m_scoreBonusMaxTime = pGameManager->GetBonusMaxTime();

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("GameBGM", L"Data/Sound/BGM/gameSceneBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームシーンBGM
	soundManager.LoadSoundClip("ClearBGM", L"Data/Sound/BGM/gameClearBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームクリアBGM
	soundManager.LoadSoundClip("OK", L"Data/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false); // 決定時のSE
	soundManager.LoadSoundClip("Cancel", L"Data/Sound/SE/cancel.mp3", SoundBus::SE, 1.0f, false); // キャンセル時のSE
	soundManager.LoadSoundClip("CursorMove", L"Data/Sound/SE/cursorMove.mp3", SoundBus::SE, 1.0f, false); // カーソルが動く時のSE
	soundManager.LoadSoundClip("AddScore", L"Data/Sound/SE/addScore.mp3", SoundBus::SE, 1.0f, true); // スコア加算時のSE


	// UIマネージャーのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	// 画像ハンドルをロード
	uiManager.LoadGraphHandle(L"textBox.png");
	uiManager.LoadGraphHandle(L"backGround_2.png"); // 背景の画像
	uiManager.LoadGraphHandle(L"backGround_logo.png"); // 文字の画像
	// UIを生成
	auto backGround = uiManager.CreateImage(kBackGroundPos, L"backGround_2.png").lock();
	backGround->SetLayer(UIBase::Layer::BackGround);
	auto backGroundText = uiManager.CreateImage(Vector2Int::Zero(), L"backGround_logo.png").lock();
	backGroundText->SetLoopBackGround(kBackGroundScrollFrame);
	backGroundText->SetLayer(UIBase::Layer::BackGround);
	uiManager.SetLayerActive(UIBase::Layer::BackGround, true);

	// ゲームクリアのテキストを描画
	uiManager.CreateText(kClearTextPos, L"クリア!", FontType::Header);
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

	// 選択肢リストを作成
	m_pSelectList = uiManager.CreateSelectList(kSelectListPos, kSelectListSize, FontType::Midium);
	auto selectList = m_pSelectList.lock();
	selectList->AddOption(L"リスタート", [this]()
		{
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// ゲームシーンに移行
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		});
	selectList->AddOption(L"ベストスコア", [this]()
		{

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

	// ステージ毎の記録をセットする
	SetRecord();
	// 記録テキストのアクティブ状態を非表示にしておく
	SetRecordActive(false);
}

ClearScene::~ClearScene()
{
	// モデルの終了処理を行う
	MV1DeleteModel(m_playerModelHandle);
	MV1DeleteModel(m_enemyModelHandle);
	m_pPlayerModel->End();
	m_pEnemyModel->End();

	// 登録したサウンドを開放
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("ClearBGM");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("Cancel");
	soundManager.DeleteSoundClip("CursorMove");
	soundManager.DeleteSoundClip("AddScore");

	// 登録したUIを開放
	auto& uiManager = UIManager::GetInstance();
	uiManager.DeleteGraphHandle(L"textBox.png");
	uiManager.DeleteGraphHandle(L"backGround_2.png");
	uiManager.DeleteGraphHandle(L"backGround_logo.png");
}

void ClearScene::Update(Input& input)
{
	UpdateModel();
	(this->*m_update)(input);
}

void ClearScene::Draw()
{
	(this->*m_draw)();
}

void ClearScene::FadeInUpdate(Input&)
{
	// フレームを減少
	m_frameCount--;

	// フレームが0以下になったら
	if (m_frameCount <= 0)
	{
		// クリアタイムを表示する
		ActiveText("ClearTime");
		// フェードイン完了
		m_update = &ClearScene::ClearTimeUpdate;
		m_draw = &ClearScene::NormalDraw;
		return; // 念のため処理を抜ける
	}
}

void ClearScene::NormalUpdate(Input& input)
{
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
		// 決定時のSEを再生
		SoundManager::GetInstance().Play("OK", 1.0f, true);
		
		auto selectList = m_pSelectList.lock();
		if(selectList->IsMatchedCursor(L"ベストスコア"))
		{
			SetRecordActive(true);
			m_update = &ClearScene::RecordUpdate;
			return;
		}
		else if (selectList->IsMatchedCursor(L"リスタート"))
		{
			// BGMを変更
			SoundManager::GetInstance().CrossFadeBGM("GameBGM", kCrossFadeBGMTime);
		}
		
		m_update = &ClearScene::FadeOutUpdate;
		m_draw = &ClearScene::FadeDraw;
		m_fadeColor = kFadeOutColor;
		
		return; // 念のため処理を抜ける
	}
}

void ClearScene::FadeOutUpdate(Input&)
{
	// フレーム数を加算
	m_frameCount++;

	// フェードアウト時間を超えたら
	if (m_frameCount >= kFadeInterval)
	{
		// ゲームマネージャーのweak_ptrを取得
		auto wpGameManager = m_controller.GetGameManager();
		auto pGameManager = wpGameManager.lock();

		// ゲームマネージャーの状態をリセットする
		pGameManager->Init();

		// 選択された処理を行う
		auto selectList = m_pSelectList.lock();
		selectList->TriggerSelect();
		return; // 念のため処理を抜ける
	}
}

void ClearScene::ClearTimeUpdate(Input&)
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
			m_update = &ClearScene::ScoreUpdate;
			ActiveText("Score");
		}
	}
}

void ClearScene::ScoreUpdate(Input&)
{
	m_waitFrame++;

	if (m_waitFrame >= kBonusScoreWaitFrame)
	{
		m_update = &ClearScene::BonusScoreUpdate;
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

void ClearScene::BonusScoreUpdate(Input& input)
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

		m_update = &ClearScene::ScoreAddUpdate;
		return;
	}
}

void ClearScene::ScoreAddUpdate(Input& input)
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

		m_update = &ClearScene::NormalUpdate;
	}
}

void ClearScene::RecordUpdate(Input& input)
{
	// もう一度決定が押されたら
	if (input.IsTriggered("OK"))
	{
		// レコードのテキストを非表示にして元の状態に戻る
		SetRecordActive(false);
		m_update = &ClearScene::NormalUpdate;
		return;
	}

}


void ClearScene::NormalDraw()
{
	// UIの描画
	UIManager::GetInstance().DrawLayer(UIBase::Layer::BackGround, UIBase::Layer::Game);

	// モデルの描画
	m_pPlayerModel->Draw();
	m_pEnemyModel->Draw();

	UIManager::GetInstance().DrawLayer(UIBase::Layer::Game, UIBase::Layer::Max);

#ifdef _DEBUG
	DrawString(0, 0, L"ClearScene", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FPS:%.1f", GetFPS());
#endif // _DEBUG
}

void ClearScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::CheckBonusScore()
{
	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	// ボーナススコアを計算
	m_timeBonusScore = pGameManager->GetTotalScore() + std::max(0.0f, (m_scoreBonusMaxTime - static_cast<int>(pGameManager->GetClearTime() / 60)) * kDefaultScore);
}

void ClearScene::SetRecord()
{
	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	auto& uiManager = UIManager::GetInstance();
	m_pTextBoxImage = uiManager.CreateImage(kRecordBoxImagePos, L"textBox.png");

	auto bestTimeText = uiManager.CreateText(kRecordTimeTextPos - kRecordStageTextMargin, L"ベストタイム:", FontType::Midium).lock();
	bestTimeText->SetAlignment(UIText::AlignmentType::Right);
	m_pRecordTitleTexts.push_back(bestTimeText);

	auto highScoreText = uiManager.CreateText(kRecordScoreTextPos - kRecordStageTextMargin, L"ハイスコア:", FontType::Midium).lock();
	highScoreText->SetAlignment(UIText::AlignmentType::Right);
	m_pRecordTitleTexts.push_back(highScoreText);


	// ステージの開始番号を取得
	int stage = static_cast<int>(StageType::Stage1);
	for (int i = stage; i < static_cast<int>(StageType::Clear); i++)
	{
		// ステージのデータを取得
		StageType stage = static_cast<StageType>(i);

		// ステージのデータを取得
		auto stageData = pGameManager->GetStageData(stage);

		m_recordTexts[stage].push_back(
		uiManager.CreateText(kRecordStageTextPos + kRecordTextMargin * i, L"ステージ" + std::to_wstring(i + 1), FontType::Midium)
		);
		m_recordTexts[stage].push_back(
		uiManager.CreateText(kRecordTimeTextPos + kRecordTextMargin * i, std::to_wstring(stageData.bestTime / 60), FontType::Midium)
		);
		m_recordTexts[stage].push_back(
		uiManager.CreateText(kRecordScoreTextPos + kRecordTextMargin * i, std::to_wstring(stageData.highScore), FontType::Midium)
		);
	}
}

void ClearScene::SetRecordActive(bool isActive)
{
	auto textBoxImage = m_pTextBoxImage.lock();
	// アクティブ状態になる場合
	if (isActive)
	{
		// アクティブ状態をセットする
		textBoxImage->SetActive(isActive);
		// フェードインしながら表示する
		textBoxImage->StartAppearCenter(kFadeInFrame);
	}
	else
	{
		textBoxImage->StartCloseCenter(kFadeOutFrame, false);
	}

	// ベストスコアとベストタイムのテキストの表示状態の管理
	for (auto titleText : m_pRecordTitleTexts)
	{
		auto pTitleText = titleText.lock();
		// アクティブ状態になる場合
		if (isActive)
		{
			// アクティブ状態をセットする
			pTitleText->SetActive(isActive);
			// フェードインしながら表示する
			pTitleText->StartFadeIn(kFadeInFrame);
		}
		else
		{
			pTitleText->StartFadeOut(kFadeOutFrame, false);
		}
	}

	// ステージ毎のレコードのテキストの表示状態を管理
	for (auto record : m_recordTexts)
	{
		// テキストはmapの中のvectorなのでsecondの要素を取得する
		for (auto text : record.second)
		{
			auto pText = text.lock();
			// アクティブ状態になる場合
			if (isActive)
			{
				// アクティブ状態をセットする
				pText->SetActive(isActive);
				// フェードインしながら表示する
				pText->StartFadeIn(kFadeInFrame);
			}
			else
			{
				pText->StartFadeOut(kFadeOutFrame, false);
			}
		}
	}
}

void ClearScene::ActiveText(std::string activeText)
{
	for (auto text : m_resultTexts[activeText])
	{
		auto pText = text.lock();
		pText->SetActive(true);
		pText->StartFadeIn(kFadeInFrame);
	}
}

void ClearScene::UpdateModel()
{
	// モデルを向きに応じて進ませる
	m_playerModelPos += m_moveDir * kModelMoveSpeed;
	m_enemyModelPos += m_moveDir * kModelMoveSpeed;


	// 後方のモデル基準で折り返す処理
	// プレイヤーのモデルの座標が折り返す座標に達したら
	if (m_playerModelPos.x >= kModelTurnX)
	{
		// 右に進んでいるときは向きを反転させて折り返す
		if (m_moveDir.x > 0)
		{
			m_moveDir *= -1;
		}
	}
	// 敵のモデルの座標が折り返す座標に達したら
	if (m_enemyModelPos.x <= -kModelTurnX)
	{
		// 左に進んでいるときは向きを反転させて折り返す
		if (m_moveDir.x < 0)
		{
			m_moveDir *= -1;
		}
	}

	// 座標を更新
	m_pPlayerModel->SetPos(m_playerModelPos);
	m_pEnemyModel->SetPos(m_enemyModelPos);

	// 向きを更新
	m_pPlayerModel->SetDir(m_moveDir);
	m_pEnemyModel->SetDir(m_moveDir);

	// アニメーション等を更新
	m_pPlayerModel->Update();
	m_pEnemyModel->Update();


}