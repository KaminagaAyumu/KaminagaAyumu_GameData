#include "GameScene.h"
#include "SceneController.h"
#include "PauseScene.h"
#include "ResultScene.h"
#include "ClearScene.h"
#include "GameoverScene.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "../Actor/Player/Player.h"
#include "../Actor/Enemy/EnemyManager.h"
#include "../Actor/Model.h"
#include "../Actor/Bullet/BulletManager.h"
#include "../Common/Camera/Camera.h"
#include "../Common/GameManager.h"
#include "../Geometry/Vector3.h"
#include "../Geometry/Vector2Int.h"
#include "../Common/Input.h"
#include "../Common/Game.h"
#include "../Common/UI/UIManager.h"
#include "../Common/UI/UIBase.h"
#include "../Common/UI/UIText.h"
#include "../Common/UI/UIImage.h"
#include "../Common/UI/UISelectList.h"
#include "../Effect/EffectManager.h"
#include "../Sound/SoundManager.h"
#include <cassert>

namespace
{
	// シーン遷移関連
	constexpr int kFadeInterval = 60; // フェードを行う時間
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
	constexpr unsigned int kFadeInColor = 0x000000; // フェードインの色
	constexpr unsigned int kFadeOutNextStageColor = 0x000000; // 次ステージ遷移の際のフェードアウトの色
	constexpr unsigned int kFadeOutColor = 0xffffff; // フェードアウトの色
	constexpr float kCrossFadeBGMTime = 1.0f; // BGMをクロスフェードさせる時間(秒換算)

	// 選択リストのカーソルが動く値
	constexpr int kCursorMoveIndex = 1;

	// ゲーム中の視野角
	constexpr float kGameFovDegree = 60.0f;

	// ゲーム内の最大時間
	constexpr int kMaxGameTime = 999 * 60;// 999秒

	// スコアを加算する割合
	constexpr float kScoreAddRate = 0.15f;

	// 加算割合の閾値
	constexpr float kScoreThreshold = 0.9f;

	// マップを見下ろしているフレーム
	constexpr int kLookDownFrame = 180;
	
	// マップを見下ろし始めるフレーム
	constexpr int kLookDownStartFrame = 100;

	// マップを見下ろすカメラの加速度
	constexpr float kLookDownAcceleration = 0.0005f;

	// マップを見下ろしている際のテキストのフェードフレーム
	constexpr int kLookDownTextFadeInFrame = 30;
	constexpr int kLookDownTextFadeOutFrame = 30;

	// ゲーム開始時のカウントダウンのフレーム
	constexpr int kCountDownFrame = 120;

	// ゲーム開始時のカウントダウンのフェードフレーム
	constexpr int kCountDownFadeInFrame = 60;
	constexpr int kCountDownFadeOutFrame = 60;

	// ゲーム経過時間を表示するテキストのフェードインのフレーム
	constexpr int kTimeTextFadeInFrame = 30;

	// 背景画像を表示する位置
	const Vector2Int kBackGroundImagePos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };

	// チュートリアルの背景の透明度
	constexpr int kTutorialBackGroundAlpha = 255;

	// チュートリアル背景のフェードアウトフレーム数
	constexpr int kTutorialBackGroundFadeOutFrame = 30;

	// チュートリアルをスキップするテキストを表示する位置
	const Vector2Int kTutorialSkipTextPos = { Game::kScreenWidth - 15, Game::kScreenHeight - 35 };

	// チュートリアルのテキストを表示する位置
	const Vector2Int kTutorialTextPos = { Game::kScreenWidth / 2, 50 };
	
	// チュートリアルのムービーを描画する中心座標
	const Vector2Int kTutorialMoviePos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 - 50};

	// チュートリアルのムービーの画像サイズ
	const Vector2 kTutorialMovieScale = { 0.4f, 0.4f };

	// チュートリアルをスキップするかどうかを表示するダイアログの表示位置
	const Vector2Int kTutorialSkipDialogPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };
	// チュートリアルをスキップするかどうかを表示するダイアログの表示位置
	const Vector2Int kTutorialSkipDialogSize = { 550, 300 };

	// ダイアログの表示までの時間
	constexpr int kTutorialSkipDialogAppearFrame = 10;

	// チュートリアルのムービーの再生が終わってから待つフレーム
	constexpr int kTutorialWaitFrame = 60;

	// カウントダウンのテキストを表示する位置
	const Vector2Int kCountDownTextPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };
	// 経過時間を表示する位置
	const Vector2Int kTimeTextPos = { Game::kScreenWidth / 2, 60 };

	// スコアを表示する位置
	const Vector2Int kScoreTextPos = { Game::kScreenWidth - 20, 30 };

	// ミニマップの描画するサイズ
	const Vector2Int kMiniMapSize = { Game::kScreenWidth,Game::kScreenHeight };

	// ミニマップ画像のサイズ
	const Vector2Int kMiniMapGraphSize = { 700,700 };
	
	// ミニマップ画像のスケール
	const Vector2 kMiniMapGraphScale = { 0.54f,0.97f };

	// ミニマップ画像の後ろに少しだけ描画する背景のマージン
	constexpr int kMiniMapBackGroundMargin = 3;

	// ミニマップの表示のフレーム数
	constexpr int kFirstMiniMapAppearFrame = 120;
	constexpr int kFirstMiniMapCloseFrame = 120;
	constexpr int kNormalMiniMapAppearFrame = 15;
	constexpr int kNormalMiniMapCloseFrame = 15;

	// ミニマップで使用するカメラのNear,Far
	constexpr float kMiniMapCameraNear = 1.0f;
	constexpr float kMiniMapCameraFar = 20000.0f;

	// ミニマップで使用するカメラの座標と向き
	const Vector3 kMiniMapCameraPos = { 0.0f,20000.0f,0.0f };
	const Vector3 kMiniMapCameraUpVec = { 0.0f,0.0f,1.0f };


	// ミニマップを表示する中心座標
	const Vector2Int kMiniMapPos = { Game::kScreenWidth / 2, Game::kScreenHeight / 2 };
	//const Vector2Int kMiniMapPos = { Game::kScreenWidth - (kMiniMapGraphSize.x / 2 + 50), Game::kScreenHeight - (kMiniMapGraphSize.y / 2 + 50) };

	// 見下ろしの際に表示するテキストの位置
	const Vector2Int kLookDownTextPos = { Game::kScreenWidth / 2, 50 };

	// 見下ろしの際に表示するテキストの色
	constexpr unsigned int kLookDownTextColor = 0xddd000;

	// AボタンUIを表示する位置
	const Vector2Int kAButtonUIPos = { Game::kScreenWidth - 200, Game::kScreenHeight - 100 };
	// チュートリアルの際にAボタンを表示する位置
	const Vector2Int kTutorialAButtonUIPos = { Game::kScreenWidth - 300, Game::kScreenHeight - 150 };
	// Aボタンの説明テキストを表示する位置
	const Vector2Int kAButtonTextPos = { Game::kScreenWidth - 100, Game::kScreenHeight - 100 };

	// XボタンUIを表示する位置
	const Vector2Int kXButtonUIPos = { Game::kScreenWidth - 200, Game::kScreenHeight - 200 };
	// Xボタンの説明テキストを表示する位置
	const Vector2Int kXButtonTextPos = { Game::kScreenWidth - 100, Game::kScreenHeight - 200 };

	// ボタンUIを表示する切り取りサイズ
	const Vector2Int kButtonGraphSize = { 16,16 };

	// ボタンUIの拡大率
	const Vector2 kButtonScale = { 3.0f,3.0f };

	// ボタンのアニメーションの数
	constexpr int kButtonAnimNum = 3;
	// ボタンのアニメーション進行フレーム数
	constexpr int kButtonAnimFrame = 10;

	// 見下ろす座標の初期位置
	const Vector3 kFirstLookDownPos = { 0.0f,10000.0f,0.0f };

	// カメラがプレイヤーを見る際のオフセット
	const Vector3 kPlayerTargetOffset = { 0.0f,150.0f,0.0f };

	// カメラの補正を行うスピード
	constexpr float kCameraFirstLerpSpeed = 0.03f;

	// カメラの補正を終わるまでの距離
	constexpr float kCameraLerpEndDist = 20.0f;

	// 地面の座標(デフォルト値だと他のオブジェクトが埋まってしまうためずらす)
	const Vector3 kGroundPos = { 0.0f,-200.0f,0.0f };
	// 地面のスケール
	const Vector3 kGroundScale = { 2.0f,2.0f,2.0f };
	
	// スカイボックスのスケール
	const Vector3 kSkyboxScale = { 75.0f,75.0f,75.0f };

	// デフォルトの光の当たる向き
	const Vector3 kDefaultLightDir = { 0.0f, -1.5f, 0.0f };

	// シャドウマップ関連
	const Vector2Int kShadowMapSize = { 8192,8192 }; // 影の粗さ(2のn乗である必要がある)
	const Vector3 kShadowMapMinPos = { Game::kMapMinX, -1.0f, Game::kMapMinZ }; // シャドウマップを使用する最低座標
	const Vector3 kShadowMapMaxPos = { Game::kMapMaxX , 100.0f, Game::kMapMaxZ }; // シャドウマップを使用する最高座標
}

GameScene::GameScene(SceneController& controller) :
	SceneBase(controller),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw),
	m_isCountDownTextStartFadeOut(false),
	m_isOpenMap(false)
{
	// フェードの設定
	m_frameCount = kFadeInterval;
	m_fadeColor = kFadeInColor;

	// 見下ろす時間をセット
	m_lookDownFrame = kLookDownFrame;

	// 見下ろす位置をセット
	m_lookDownTarget = kFirstLookDownPos;

	// 見下ろす補完スピードをセット
	m_lookDownCameraLerpSpeed = kCameraFirstLerpSpeed;

	// ゲーム内の時間をカウントダウン時のものにする
	m_gameTime = kCountDownFrame;

	m_pPlayer = std::make_shared<Player>();

	m_pPlayer->Init();

	m_groundHandle = MV1LoadModel(L"Data/Model/ground.mv1");

	// スカイボックスを作成
	m_skyboxHandle = MV1LoadModel(L"Data/Model/skybox.mv1");
	// スカイボックスのスケールを調整
	MV1SetScale(m_skyboxHandle, kSkyboxScale.ToDxLibVector());

	// シャドウマップを作成
	m_shadowMapHandle = MakeShadowMap(kShadowMapSize.x, kShadowMapSize.y);

	// シャドウマップのライトの向きを設定
	SetShadowMapLightDirection(m_shadowMapHandle, kDefaultLightDir.ToDxLibVector());

	// シャドウマップの描画範囲を設定
	SetShadowMapDrawArea(m_shadowMapHandle, kShadowMapMinPos.ToDxLibVector(), kShadowMapMaxPos.ToDxLibVector());

	// ミニマップのスクリーン画像ハンドルを設定
	m_miniMapHandle = MakeScreen(kMiniMapSize.x, kMiniMapSize.y);

	m_pGround = std::make_shared<Model>();

	m_pGround->SetModelHandle(MV1DuplicateModel(m_groundHandle));

	m_pGround->SetPos(kGroundPos);
	m_pGround->SetScale(kGroundScale);
	m_pGround->Init();

	m_pCamera = std::make_shared<Camera>();

	m_pCamera->Init();

	// カメラを更新
	m_pCamera->UpdateNoRotation(m_lookDownTarget, kGameFovDegree);

	m_pBulletManager = std::make_shared<BulletManager>();
	m_pBulletManager->Init();

	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->Init();

	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	// 表示用のスコアを初期化
	m_displayedScore = 0;
	// 最終的なスコアを初期化
	m_targetScore = 0;

	// 敵の生成
	m_pEnemyManager->SpawnEnemyToStageNo(pGameManager->GetCurrentStage(), m_pPlayer, m_pBulletManager);

	// 遷移先を設定しない
	m_nextScene = NextScene::None;

	// ライトの向きを初期位置に設定
	m_lightDir = kDefaultLightDir;

	// サウンドマネージャーのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// サウンドを登録
	soundManager.LoadSoundClip("GameBGM", L"Data/Sound/BGM/gameSceneBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームシーンBGM
	soundManager.LoadSoundClip("ClearBGM", L"Data/Sound/BGM/gameClearBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームクリアBGM
	soundManager.LoadSoundClip("ResultBGM", L"Data/Sound/BGM/resultBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームクリアBGM
	soundManager.LoadSoundClip("GameoverBGM", L"Data/Sound/BGM/gameoverBGM.mp3", SoundBus::BGM, 1.0f, true); // ゲームオーバーBGM
	soundManager.LoadSoundClip("Ready", L"Data/Sound/SE/gameReady.mp3", SoundBus::SE, 1.0f, false); // ゲーム開始の合図のSE
	soundManager.LoadSoundClip("Start", L"Data/Sound/SE/gameStart.mp3", SoundBus::SE, 1.0f, false); // ゲーム開始のSE
	soundManager.LoadSoundClip("OpenMap", L"Data/Sound/SE/openMap.mp3", SoundBus::SE, 1.0f, false); // マップを開く際のSE
	soundManager.LoadSoundClip("OK", L"Data/Sound/SE/ok.mp3", SoundBus::SE, 1.0f, false); // 決定時のSE
	soundManager.LoadSoundClip("TutorialOK", L"Data/Sound/SE/tutorialOk.mp3", SoundBus::SE, 1.0f, false); // チュートリアルでの決定時のSE
	soundManager.LoadSoundClip("OpenDialog", L"Data/Sound/SE/openDialog.mp3", SoundBus::SE, 1.0f, false); // ダイアログを開く時のSE
	soundManager.LoadSoundClip("Cancel", L"Data/Sound/SE/cancel.mp3", SoundBus::SE, 1.0f, false); // キャンセル時のSE
	soundManager.LoadSoundClip("CursorMove", L"Data/Sound/SE/cursorMove.mp3", SoundBus::SE, 1.0f, false); // カーソルが動く時のSE
	soundManager.LoadSoundClip("Pause", L"Data/Sound/SE/pause.mp3", SoundBus::SE, 1.0f, false); // ポーズ時のSE

	// UIマネージャーのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	// カウントダウン時のテキストを取得
	m_pCountDownText = uiManager.CreateText(kCountDownTextPos, L"", FontType::Header);
	// 時間を表示するテキストを取得
	m_pTimeText = uiManager.CreateText(kTimeTextPos, L"", FontType::Large);
	// スコアを表示するテキストを取得
	m_pScoreText = uiManager.CreateText(kScoreTextPos, L"", FontType::Midium);
	auto scoreText = m_pScoreText.lock();
	scoreText->SetText(L"スコア : " + std::to_wstring(pGameManager->GetTotalScore()));
	// 文字を右揃えにする
	scoreText->SetAlignment(UIText::AlignmentType::Right);
	// レイヤーをインゲームのものにする
	scoreText->SetLayer(UIBase::Layer::InGame);

	// チュートリアルをスキップするテキストを生成
	auto tutorialText = uiManager.CreateText(kTutorialSkipTextPos, L"STARTボタンでスキップ", FontType::Midium).lock();
	tutorialText->SetAlignment(UIText::AlignmentType::Right);
	tutorialText->SetLayer(UIBase::Layer::Tutorial);

	// 画像ハンドルをロード
	uiManager.LoadGraphHandle(L"backGround_1.png");	// 背景の画像
	uiManager.LoadGraphHandle(L"a_button.png");		// Aボタンの画像
	uiManager.LoadGraphHandle(L"x_button.png");		// Xボタンの画像
	uiManager.LoadGraphHandle(L"frame.png");		// ミニマップの枠画像

	// 背景を表示する画像クラスを生成
	m_pBackGroundImage = uiManager.CreateImage(kBackGroundImagePos, L"backGround_1.png");
	auto backGround = m_pBackGroundImage.lock();
	backGround->SetLayer(UIBase::Layer::BackGround);
	// 背景画像のアルファ値を設定
	uiManager.SetLayerAlpha(UIBase::Layer::BackGround, kTutorialBackGroundAlpha);
	// 最初のステージならば
	if (pGameManager->GetCurrentStage() == StageType::Stage1)
	{
		// 背景を表示させる
		uiManager.SetLayerActive(UIBase::Layer::BackGround, true);
		// チュートリアルのUIを表示させる
		uiManager.SetLayerActive(UIBase::Layer::Tutorial, true);
	}
	else
	{
		// 背景を非表示にする
		uiManager.SetLayerActive(UIBase::Layer::BackGround, false);
		// チュートリアルのUIを非表示にする
		uiManager.SetLayerActive(UIBase::Layer::Tutorial, false);
	}

	// ミニマップを表示する画像クラスを生成
	m_pMiniMapImage = uiManager.CreateImageNoPath(kMiniMapPos, L"mini_map", m_miniMapHandle, L"frame.png");
	auto pMiniMapImage = m_pMiniMapImage.lock();
	pMiniMapImage->SetActive(false);
	pMiniMapImage->SetScale(kMiniMapGraphScale);

	// 見下ろし時に表示するテキストを取得
	m_pLookDownText = uiManager.CreateText(kLookDownTextPos, L"", FontType::Midium);

	// ボタンのテキストを表示
	auto aButtonText = uiManager.CreateText(kAButtonTextPos, L":バリア", FontType::Midium).lock();
	aButtonText->SetLayer(UIBase::Layer::InGame);
	auto xButtonText = uiManager.CreateText(kXButtonTextPos, L":マップ", FontType::Midium).lock();
	xButtonText->SetLayer(UIBase::Layer::InGame);

	m_pAButtonImage = uiManager.CreateImage(kTutorialAButtonUIPos, L"a_button.png");
	auto xButtonUI = uiManager.CreateImage(kXButtonUIPos, L"x_button.png");
	// shared_ptrを取得して画像の設定をする
	if (auto pAButtonUI = m_pAButtonImage.lock())
	{
		pAButtonUI->SetAnimation(kButtonGraphSize, kButtonAnimNum, kButtonAnimFrame, true);
		pAButtonUI->SetScale(kButtonScale);
		pAButtonUI->SetLayer(UIBase::Layer::Tutorial); // チュートリアルで使うためレイヤーを設定する
		pAButtonUI->SetActive(false); // 最初は非表示にしておく
	}
	if (auto pXButtonUI = xButtonUI.lock())
	{
		pXButtonUI->SetAnimation(kButtonGraphSize, kButtonAnimNum, kButtonAnimFrame, true);
		pXButtonUI->SetScale(kButtonScale);
		pXButtonUI->SetLayer(UIBase::Layer::InGame);
	}

	// チュートリアルの進行度をリセット
	m_tutorialStepNum = 0;

	// チュートリアルのデータを初期化
	InitTutorialData();

	m_pDialog = uiManager.CreateSelectList(kTutorialSkipDialogPos, kTutorialSkipDialogSize, FontType::Small);
	auto dialog = m_pDialog.lock();
	dialog->SetYesNoDialog(L"チュートリアルをスキップしますか？",
		// はいが選択された場合の処理
		[this]()
		{
			// 決定時のSEを再生
			SoundManager::GetInstance().Play("OK", 1.0f, false);
			ChangeLookDown();
		},
		// いいえが選択された場合の処理
		[this]()
		{
			// キャンセル時のSEを再生
			SoundManager::GetInstance().Play("Cancel", 1.0f, true);
			m_update = &GameScene::TutorialUpdate;
		});

	// ダイアログをチュートリアルのレイヤーに設定
	dialog->SetLayer(UIBase::Layer::Pause);
	// ダイアログを非表示にする
	dialog->SetActive(false);
}

GameScene::~GameScene()
{
	MV1DeleteModel(m_groundHandle);
	MV1DeleteModel(m_skyboxHandle);
	DeleteShadowMap(m_shadowMapHandle);
	DeleteGraph(m_miniMapHandle);
	m_pGround->End();
	m_pPlayer->End();
	m_pBulletManager->End();
	m_pEnemyManager->End();
	// 登録したサウンドを開放
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("GameBGM");
	soundManager.DeleteSoundClip("ClearBGM");
	soundManager.DeleteSoundClip("ResultBGM");
	soundManager.DeleteSoundClip("GameoverBGM");
	soundManager.DeleteSoundClip("Ready");
	soundManager.DeleteSoundClip("Start");
	soundManager.DeleteSoundClip("OpenMap");
	soundManager.DeleteSoundClip("OK");
	soundManager.DeleteSoundClip("TutorialOK");
	soundManager.DeleteSoundClip("OpenDialog");
	soundManager.DeleteSoundClip("Cancel");
	soundManager.DeleteSoundClip("CursorMove");
	soundManager.DeleteSoundClip("Pause");
	UIManager::GetInstance().DeleteGraphHandle(L"backGround_1.png");
	UIManager::GetInstance().DeleteGraphHandle(L"a_button.png");
	UIManager::GetInstance().DeleteGraphHandle(L"x_button.png");
	UIManager::GetInstance().DeleteGraphHandle(L"frame.png");
	// チュートリアルで使った動画ハンドルを開放
	for (auto& step : m_steps)
	{
		DeleteGraph(step.movieHandle);
	}
}

void GameScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void GameScene::Draw()
{
	(this->*m_draw)();
}

void GameScene::FadeInUpdate(Input&)
{
	// フレームを減少
	m_frameCount--;

	// フレームが0以下になったら
	if (m_frameCount <= 0)
	{
		// ゲームマネージャーを取得する
		if (auto pGameManager = m_controller.GetGameManager().lock())
		{
			// ステージが最初のステージだった場合
			if (pGameManager->GetCurrentStage() == StageType::Stage1)
			{
				PlayMovieToGraph(m_steps[m_tutorialStepNum].movieHandle);
				auto stepText = m_steps[m_tutorialStepNum].text.lock();
				stepText->StartFadeIn(kCountDownFadeInFrame);
				stepText->SetActive(true);

				// チュートリアルのUIを表示させる
				UIManager::GetInstance().SetLayerActive(UIBase::Layer::Tutorial, true);

				// チュートリアルの更新処理に変更
				m_update = &GameScene::TutorialUpdate;
				m_draw = &GameScene::TutorialDraw;
			}
			else
			{
				// 見下ろす処理に遷移
				ChangeLookDown();
			}
		}
		return; // 念のため処理を抜ける
	}
}

void GameScene::TutorialUpdate(Input& input)
{
	// STARTボタンが押された際はスキップして見下ろしマップの処理に移行
	if (input.IsTriggered("Pause"))
	{
		m_update = &GameScene::YesNoDialogUpdate;
		OpenYesNoDialog();
		return;
	}

	// ムービーが再生されていない時
	if(GetMovieStateToGraph(m_steps[m_tutorialStepNum].movieHandle) == 0)
	{
		// ムービーを最初から再生しなおす
		SeekMovieToGraph(m_steps[m_tutorialStepNum].movieHandle, 0);
		PlayMovieToGraph(m_steps[m_tutorialStepNum].movieHandle);
		// ムービーが終了したとする
		m_steps[m_tutorialStepNum].isMovieEnd = true;
	}

	// 動画の画像ハンドルを更新
	auto movie = m_pTutorialMovieImage.lock();
	movie->SetHandle(m_steps[m_tutorialStepNum].movieHandle);

	// Aボタンの画像を取得
	auto pAButtonImage = m_pAButtonImage.lock();
	pAButtonImage->SetActive(m_steps[m_tutorialStepNum].isMovieEnd);

	// ムービーが終わったら
	if (m_steps[m_tutorialStepNum].isMovieEnd)
	{
		// 待機時間を減少
		m_steps[m_tutorialStepNum].waitFrame--;
	}

	// 待機時間が0以下になったら
	if (m_steps[m_tutorialStepNum].waitFrame <= 0)
	{
		// 待機時間を0にする
		m_steps[m_tutorialStepNum].waitFrame = 0;
	}
	else
	{
		// 次のステップに進めないようにする
		return;
	}

	// 待機時間を過ぎた後に決定ボタンが押されたら次のステップに進む
	if (input.IsTriggered("OK"))
	{
		// ゲーム開始のSEを再生
		SoundManager::GetInstance().Play("TutorialOK", 1.0f, false);
		AdvanceStep();
	}
}

void GameScene::LookDownUpdate(Input&)
{
	// マップを見下ろすフレームを減少
	m_lookDownFrame--;

	// カメラを更新
	m_pCamera->UpdateNoRotation(m_lookDownTarget, kGameFovDegree);

	if (m_lookDownFrame <= kLookDownStartFrame)
	{
		// ターゲットを見る
		m_lookDownTarget = Vector3::LerpVec3(m_lookDownTarget, m_pPlayer->GetPos() + kPlayerTargetOffset, m_lookDownCameraLerpSpeed);

		// 補完スピードを徐々に加速させる
		m_lookDownCameraLerpSpeed += kLookDownAcceleration;
	}

	// フレームが0以下になってターゲットが既定の距離の範囲内プレイヤーに近づいたら
	if (m_lookDownFrame <= 0 && Vector3::GetDistance(m_lookDownTarget, m_pPlayer->GetPos() + kPlayerTargetOffset) <= kCameraLerpEndDist)
	{
		// 見下ろしの際に表示するテキストを設定
		auto lookDownText = m_pLookDownText.lock();
		lookDownText->StartFadeOut(kLookDownTextFadeOutFrame, true);

		if (auto pMiniMapImage = m_pMiniMapImage.lock())
		{
			pMiniMapImage->StartCloseCenter(kFirstMiniMapCloseFrame,true);
		}

		// ターゲットをプレイヤー基準の場所に合わせる
		m_lookDownTarget = m_pPlayer->GetPos() + kPlayerTargetOffset;
		// カウントダウンを開始
		auto text = m_pCountDownText.lock();
		text->SetText(L"Ready");
		text->StartFadeIn(kCountDownFadeInFrame);
		// ゲーム開始のSEを再生
		SoundManager::GetInstance().Play("Ready", 1.0f, false);
		m_update = &GameScene::CountDownUpdate;
		m_draw = &GameScene::CountDownDraw;
		return; // 念のため処理を抜ける
	}
}

void GameScene::CountDownUpdate(Input&)
{
	// カウントダウン時間を減少
	m_gameTime--;

	// カメラを更新
	m_pCamera->UpdateNoRotation(m_lookDownTarget, kGameFovDegree);

	// カウントダウン時間が0以下になったら
	if (m_gameTime <= 0)
	{
		// ゲーム内のUIを表示する
		UIManager::GetInstance().SetLayerActive(UIBase::Layer::InGame, true);

		// テキストをゲーム開始時のものにする
		auto text = m_pCountDownText.lock();
		text->SetText(L"GO!");
		// 時間テキストをフェードイン
		auto timeText = m_pTimeText.lock();
		timeText->StartFadeIn(kTimeTextFadeInFrame);

		// Aボタンの画像を取得
		auto pAButtonImage = m_pAButtonImage.lock();
		// 座標をゲーム内で表示する際の場所に変更
		pAButtonImage->SetPos(kAButtonUIPos);
		// レイヤーをゲーム内のものに変更
		pAButtonImage->SetLayer(UIBase::Layer::Game);
		// 表示状態にする
		pAButtonImage->SetActive(true);

		// ゲーム開始のSEを再生
		SoundManager::GetInstance().Play("Start", 1.0f, false);
		// 経過時間を初期化
		m_gameTime = 0;
		// ゲームスタートへ遷移
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
		return; // 念のため処理を抜ける
	}

}

void GameScene::NormalUpdate(Input& input)
{
	// ゲームマネージャーのweak_ptrを取得
	auto wpGameManager = m_controller.GetGameManager();
	auto pGameManager = wpGameManager.lock();

	// ポーズボタンが押されたら
	if (input.IsTriggered("Pause"))
	{
		// ポーズ時のSEを再生
		SoundManager::GetInstance().Play("Pause", 1.0f, true);
		// ポーズシーンをプッシュする(このシーンに戻ることも可能にする)
		m_controller.PushScene(std::make_shared<PauseScene>(m_controller));
		return;
	}

	// マップを開く入力が行われているかどうかを取得
	m_isOpenMap = input.IsPressed("OpenMap");

	// ゲーム内の時間をカウント
	m_gameTime++;

	// ゲーム内の時間が最大時間を超えたら更新しない
	if (m_gameTime >= kMaxGameTime)
	{
		m_gameTime = kMaxGameTime;
	}

	// 時間のテキストをlockして更新
	auto text = m_pTimeText.lock();
	text->SetText(L"Time : " + std::to_wstring(m_gameTime / 60));

	// プレイヤーが死んだら
	if (m_pPlayer->IsDead())
	{
		// プレイヤー以外の更新処理を行わない
		m_pPlayer->Update();

		// プレイヤーが完全に死んだら
		if (m_pPlayer->IsDied())
		{
			m_update = &GameScene::FadeOutUpdate;
			m_draw = &GameScene::FadeDraw;
			m_fadeColor = kFadeOutColor; // フェードアウトの色に変更
			m_nextScene = NextScene::Gameover; // 次のシーンをゲームオーバーシーンに設定
			CheckNextBGM();
		}
		// これ以下の更新処理は行わないようにする
		return;
	}

	// カウントダウンのテキストが存在中なら
	if (auto text = m_pCountDownText.lock())
	{
		if (m_gameTime >= kCountDownFrame && !m_isCountDownTextStartFadeOut)
		{
			text->StartFadeOut(kCountDownFadeOutFrame, true);
			m_isCountDownTextStartFadeOut = true;
		}
	}

	// この値はカメラマネージャーから取得するように変更する
	m_pCamera->Update(m_pPlayer->GetPos() + kPlayerTargetOffset, kGameFovDegree);

	m_pPlayer->GetCameraDir(m_pCamera->GetCameraDir());

	m_pPlayer->Update();

	m_pGround->Update();

	m_pBulletManager->Update();

	m_targetScore += m_pEnemyManager->Update(m_pCamera);

	if (m_displayedScore < m_targetScore)
	{
		// 加算するスコアの量を取得
		int add = static_cast<int>((m_targetScore - m_displayedScore) * kScoreAddRate);

		// スコアの量の割合が閾値を超えたらスコアを現在のスコアに合わせる
		if (add <= kScoreThreshold)
		{
			m_displayedScore = m_targetScore;
		}
		else // スコアを加算していく
		{
			m_displayedScore += add;
		}
		// 描画用のスコアが現在のスコアを超えないようにする
		if (m_displayedScore > m_targetScore)
		{
			m_displayedScore = m_targetScore;
		}
	}

	// このゲーム内でのスコアを更新する
	auto scoreText = m_pScoreText.lock();
	scoreText->SetText(L"スコア : " + std::to_wstring(pGameManager->GetTotalScore() + m_displayedScore));

	// 敵を全員倒したら
	if (m_pEnemyManager->IsKilledAll())
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;

		// このゲーム内でのスコアを加算する
		pGameManager->AddScore(m_targetScore);
		// ステージクリアの際に渡す情報を更新
		pGameManager->OnStageClear(m_gameTime);
		// ステージ番号がクリアの場合
		if (pGameManager->GetCurrentStage() == StageType::Clear)
		{
			m_fadeColor = kFadeOutColor; // クリアのフェードアウトの色に変更
			m_nextScene = NextScene::Clear; // クリアシーンに変更
		}
		else
		{
			m_fadeColor = kFadeOutNextStageColor; // 次のシーンのフェードアウトの色に変更
			m_nextScene = NextScene::Result; // 次のステージに変更
		}
		CheckNextBGM();
	}

}

void GameScene::FadeOutUpdate(Input&)
{
	// フレーム数を加算
	m_frameCount++;

	// フェードアウト時間を超えたら
	if (m_frameCount >= kFadeInterval)
	{
		// 存在しているすべてのエフェクトを止める
		EffectManager::GetInstance().StopAllEffects();
		// 次のシーンを確認
		switch (m_nextScene)
		{
		case GameScene::NextScene::Game:
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// 再びゲームシーンに移行
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
			break;
		case GameScene::NextScene::Clear:
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// クリアシーンに移行
			m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller));
			break;
		case GameScene::NextScene::Result:
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// リザルトシーンに移行
			m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller));
			break;
		case GameScene::NextScene::Gameover:
			// 先に表示しているUIをリセット
			UIManager::GetInstance().ResetUIElements();
			// 描画処理はフェードしない状態にする
			// (ゲームオーバーになった瞬間の状態を見せられるようにする)
			m_draw = &GameScene::NormalDraw;
			// ゲームオーバーシーンに移行(最後の状態がわかるようにPushSceneしておく)
			m_controller.PushScene(std::make_shared<GameoverScene>(m_controller));
			break;
		case GameScene::NextScene::None:
			assert(false && "次のシーンの取得に失敗しました");
			break;
		default:
			break;
		}
		return; // 念のため処理を抜ける
	}
}

void GameScene::YesNoDialogUpdate(Input& input)
{
	// 再びポーズボタンが押されたらチュートリアルに戻る
	if (input.IsTriggered("Pause"))
	{
		// キャンセル時のSEを再生
		SoundManager::GetInstance().Play("Cancel", 1.0f, true);
		m_update = &GameScene::TutorialUpdate;
		CloseYesNoDialog();
		return;
	}

	auto dialog = m_pDialog.lock();

	// 選択肢のカーソルを移動
	if (input.IsTriggered("Left"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("CursorMove", 1.0f, true);
		dialog->MoveCursor(-kCursorMoveIndex);
	}
	if (input.IsTriggered("Right"))
	{
		// カーソルが動く際のSEを再生
		SoundManager::GetInstance().Play("CursorMove", 1.0f, true);
		dialog->MoveCursor(kCursorMoveIndex);
	}

	if (input.IsTriggered("OK") && !dialog->IsMoving())
	{
		dialog->TriggerSelect();
		CloseYesNoDialog();
		return; // 念のため処理を抜ける
	}
}

void GameScene::TutorialDraw()
{
	/*auto& step = m_steps[m_tutorialStepNum];

	DrawExtendGraph(kTutorialMoviePos.x - kTutorialMovieSize.x / 2,
		kTutorialMoviePos.y - kTutorialMovieSize.y / 2,
		kTutorialMoviePos.x + kTutorialMovieSize.x / 2,
		kTutorialMoviePos.y + kTutorialMovieSize.y / 2,
		step.movieHandle, true);*/

	// UIの描画
	UIManager::GetInstance().Draw();
}

void GameScene::LookDownDraw()
{
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x4466aa, true);

	MV1DrawModel(m_skyboxHandle);

	// シャドウマップのセットアップ
	ShadowMap_DrawSetup(m_shadowMapHandle);

	m_pGround->Draw();

	m_pPlayer->Draw();

	m_pBulletManager->Draw();

	m_pEnemyManager->Draw();

	// シャドウマップのセットアップ終了
	ShadowMap_DrawEnd();

	// ------------------------------------------------
	// シャドウマップを使用しないオブジェクトを先に描画
	// ------------------------------------------------
	m_pPlayer->Draw();

	m_pBulletManager->Draw();

	m_pEnemyManager->Draw();
	// ------------------------------------------------

	// 使用するシャドウマップを設定
	SetUseShadowMap(0, m_shadowMapHandle);

	m_pGround->Draw();

	// シャドウマップを使用しない
	SetUseShadowMap(0, -1);

	m_pCamera->Draw();

	// エフェクトの描画
	EffectManager::GetInstance().Draw();

	// ミニマップを常に描画する
	DrawMiniMap();

	// UIの描画
	UIManager::GetInstance().Draw();
}

void GameScene::CountDownDraw()
{
	// 通常の描画を行う
	NormalDraw();
}

void GameScene::NormalDraw()
{
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x4466aa, true);

	MV1DrawModel(m_skyboxHandle);

	// シャドウマップのセットアップ
	ShadowMap_DrawSetup(m_shadowMapHandle);

	m_pGround->Draw();

	m_pPlayer->Draw();

	m_pBulletManager->Draw();

	m_pEnemyManager->Draw();

	// シャドウマップのセットアップ終了
	ShadowMap_DrawEnd();

	// ------------------------------------------------
	// シャドウマップを使用しないオブジェクトを先に描画
	// ------------------------------------------------
	m_pPlayer->Draw();

	m_pBulletManager->Draw();

	m_pEnemyManager->Draw();
	// ------------------------------------------------

	// 使用するシャドウマップを設定
	SetUseShadowMap(0, m_shadowMapHandle);

	m_pGround->Draw();

	// シャドウマップを使用しない
	SetUseShadowMap(0, -1);

	m_pCamera->Draw();

	// エフェクトの描画
	EffectManager::GetInstance().Draw();

	// UIの描画
	UIManager::GetInstance().Draw();

	// 通常の更新処理の場合
	if (m_update == &GameScene::NormalUpdate)
	{
		// ミニマップへの描画
		DrawMiniMap();
	}
	
	// マップを開くボタンを押している間ミニマップを描画する
	if (m_isOpenMap)
	{
		// 更新処理が通常の場合のみ描画する
		if (m_update == &GameScene::NormalUpdate)
		{
			if (auto pMiniMap = m_pMiniMapImage.lock())
			{
				// すでに生成されている場合は何もしない
			}
			else
			{
				// ミニマップを表示する際のSEを再生
				SoundManager::GetInstance().Play("OpenMap", 1.0f, true);
				// ミニマップを表示する処理を行う
				m_pMiniMapImage = UIManager::GetInstance().CreateImageNoPath(kMiniMapPos, L"mini_map", m_miniMapHandle, L"frame.png");
				auto miniMap = m_pMiniMapImage.lock();
				miniMap->SetScale(kMiniMapGraphScale);
				miniMap->StartAppearCenter(kNormalMiniMapAppearFrame);
			}
		}
	}
	else
	{
		// 更新処理が通常の場合のみ描画する
		if (m_update == &GameScene::NormalUpdate)
		{
			// 押していない際にミニマップが表示されている場合はミニマップを閉じる
			if (auto pMiniMap = m_pMiniMapImage.lock())
			{
				if (!pMiniMap->IsMoving())
				{
					pMiniMap->StartCloseCenter(kNormalMiniMapCloseFrame,true);
				}
			}
		}
	}

#ifdef _DEBUG
	DrawString(0, 0, L"GameScene", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), L"FPS:%.1f", GetFPS());
#endif // _DEBUG
}

void GameScene::FadeDraw()
{
	// 通常の描画を行う
	NormalDraw();

	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::InitTutorialData()
{
	// UIマネージャーを取得
	auto& uiManager = UIManager::GetInstance();

	// チュートリアルのステップを追加
	TutorialStep step;
	// 1つ目
	step.movieHandle = LoadGraph(L"Data/Movie/tutorial_1.mp4");
	step.text = uiManager.CreateText(kTutorialTextPos, L"テキがたまをうってくるぞ！", FontType::Midium);
	auto pTextFirst = step.text.lock();
	pTextFirst->SetLayer(UIBase::Layer::Tutorial);
	pTextFirst->SetActive(false);
	step.waitFrame = kTutorialWaitFrame;
	step.isMovieEnd = false;
	m_steps.push_back(step);
	// 2つ目
	step.movieHandle = LoadGraph(L"Data/Movie/tutorial_2.mp4");
	step.text = uiManager.CreateText(kTutorialTextPos, L"Aボタンではねかえせ！", FontType::Midium);
	auto pTextSecond = step.text.lock();
	pTextSecond->SetLayer(UIBase::Layer::Tutorial);
	pTextSecond->SetActive(false);
	step.waitFrame = kTutorialWaitFrame;
	step.isMovieEnd = false;
	m_steps.push_back(step);

	// チュートリアルのムービーを生成
	m_pTutorialMovieImage = uiManager.CreateImageNoPath(kTutorialMoviePos, L"movie", m_steps[m_tutorialStepNum].movieHandle);
	auto movie = m_pTutorialMovieImage.lock();
	movie->SetScale(kTutorialMovieScale);
	movie->SetLayer(UIBase::Layer::Tutorial);

	// ゲーム内レイヤーを非表示にする
	uiManager.SetLayerActive(UIBase::Layer::InGame, false);
}

void GameScene::ChangeLookDown()
{
	// チュートリアルのUIは非表示にする
	UIManager::GetInstance().SetLayerActive(UIBase::Layer::Tutorial, false);

	if (auto pBackGround = m_pBackGroundImage.lock())
	{
		pBackGround->StartFadeOut(kTutorialBackGroundFadeOutFrame, true);
	}

	if (auto pMiniMapImage = m_pMiniMapImage.lock())
	{
		pMiniMapImage->SetActive(true);
		pMiniMapImage->StartAppearCenter(kFirstMiniMapAppearFrame);
	}

	// 見下ろしの際に表示するテキストを設定
	auto lookDownText = m_pLookDownText.lock();
	lookDownText->SetTextColor(kLookDownTextColor);
	lookDownText->SetText(L"テキをすべてたおせ!");
	lookDownText->StartFadeIn(kLookDownTextFadeInFrame);

	// マップを見下ろす更新処理に変更
	m_update = &GameScene::LookDownUpdate;
	m_draw = &GameScene::LookDownDraw;
}

void GameScene::AdvanceStep()
{
	auto stepText = m_steps[m_tutorialStepNum].text.lock();
	stepText->StartFadeOut(kCountDownFadeInFrame, true);

	m_tutorialStepNum++;

	// チュートリアルのステップが完了した場合
	if (m_tutorialStepNum >= m_steps.size())
	{
		// 見下ろしマップの処理に移行する
		ChangeLookDown();
	}
	else
	{
		PlayMovieToGraph(m_steps[m_tutorialStepNum].movieHandle);
		auto stepText = m_steps[m_tutorialStepNum].text.lock();
		stepText->StartFadeIn(kCountDownFadeInFrame);
		stepText->SetActive(true);

		// 動画の画像ハンドルを更新
		auto movie = m_pTutorialMovieImage.lock();
		movie->StartAppearCenter(kCountDownFadeInFrame);
		movie->SetHandle(m_steps[m_tutorialStepNum].movieHandle);
	}

}

void GameScene::DrawMiniMap()
{
	// 描画対象をミニマップのものにする
	SetDrawScreen(m_miniMapHandle);

	// 前のフレームに描画した内容をクリアする
	ClearDrawScreen();

	// ミニマップの背景の描画をする
	DrawBox(
		kMiniMapPos.x - kMiniMapGraphSize.x,
		kMiniMapPos.y - kMiniMapGraphSize.y,
		kMiniMapPos.x + kMiniMapGraphSize.x,
		kMiniMapPos.y + kMiniMapGraphSize.y, 0xbfbfbf, true);

	// 射影行列を使って距離に関係なくオブジェクトを描画する
	MATRIX projectionMat = MGetIdent();
	// マップ全体の幅を-1.0~1.0の範囲に変換する
	projectionMat.m[0][0] = 2.0f / (Game::kMapMaxX * 2); // 横幅
	projectionMat.m[1][1] = 2.0f / (Game::kMapMaxZ * 2); // 縦幅
	// Near,Farの範囲内(カメラの奥行き)を0.0~1.0の範囲に変換する
	projectionMat.m[2][2] = 1.0f / (kMiniMapCameraFar - kMiniMapCameraNear);
	projectionMat.m[3][2] = -kMiniMapCameraNear / (kMiniMapCameraFar - kMiniMapCameraNear); // Nearを奥行きの基準(0.0)とする

	// 射影行列を設定
	SetupCamera_ProjectionMatrix(projectionMat);

	// 上から見下ろすカメラに設定
	SetCameraPositionAndTargetAndUpVec(kMiniMapCameraPos.ToDxLibVector(), Vector3::Zero().ToDxLibVector(), kMiniMapCameraUpVec.ToDxLibVector());

	// シャドウマップのセットアップ
	ShadowMap_DrawSetup(m_shadowMapHandle);

	m_pGround->Draw();

	m_pPlayer->Draw();

	m_pBulletManager->Draw();

	m_pEnemyManager->Draw();

	// シャドウマップのセットアップ終了
	ShadowMap_DrawEnd();

	// ------------------------------------------------
	// シャドウマップを使用しないオブジェクトを先に描画
	// ------------------------------------------------
	m_pPlayer->Draw();

	m_pBulletManager->Draw();

	m_pEnemyManager->Draw();
	// ------------------------------------------------

	// 使用するシャドウマップを設定
	SetUseShadowMap(0, m_shadowMapHandle);

	m_pGround->Draw();

	// シャドウマップを使用しない
	SetUseShadowMap(0, -1);

	m_pCamera->Draw();

	// 描画先を元の画面に戻す
	SetDrawScreen(DX_SCREEN_BACK);

	// カメラの座標更新をする
	// HACK:この関数はあくまでもDraw関数なのでここで更新するのは微妙
	m_pCamera->UpdateNoRotation(m_pPlayer->GetPos() + kPlayerTargetOffset, kGameFovDegree);
}

void GameScene::CheckNextBGM()
{
	// 次のシーンを確認
	switch (m_nextScene)
	{
	case GameScene::NextScene::Game:
		// BGMを変更
		SoundManager::GetInstance().CrossFadeBGM("GameBGM", kCrossFadeBGMTime);
		break;
	case GameScene::NextScene::Clear:
		// BGMを変更
		SoundManager::GetInstance().CrossFadeBGM("ClearBGM", kCrossFadeBGMTime);
		break;
	case GameScene::NextScene::Result:
		// BGMを変更
		SoundManager::GetInstance().CrossFadeBGM("ResultBGM", kCrossFadeBGMTime);
		break;
	case GameScene::NextScene::Gameover:
		// BGMを変更
		SoundManager::GetInstance().CrossFadeBGM("GameoverBGM", kCrossFadeBGMTime);
		break;
	case GameScene::NextScene::None:
		assert(false && "次のシーンの取得に失敗しました");
		break;
	default:
		break;
	}
}

void GameScene::UpdateLightDir()
{
	// カメラの正面の向きを取得
	Vector3 cameraDir = m_pCamera->GetCameraDir();
	// Y方向は判別しない(ライトは常に上にある)ので0にする
	cameraDir.y = 0.0f;
	// 念のため向きを正規化
	cameraDir.Normalize();

	// カメラの向きとデフォルトのライトの向きを比率でブレンド
	m_lightDir = (cameraDir * 0.7f) + (kDefaultLightDir * 0.3f);
	// 向きを正規化
	m_lightDir.Normalize();

	// シャドウマップのライトの向きを設定
	SetShadowMapLightDirection(m_shadowMapHandle, m_lightDir.ToDxLibVector());
}

void GameScene::OpenYesNoDialog()
{
	// ダイアログを開く時のSEを再生
	SoundManager::GetInstance().Play("OpenDialog", 1.0f, true);

	if (auto dialog = m_pDialog.lock())
	{
		dialog->SetActive(true);
		dialog->StartAppearCenter(kTutorialSkipDialogAppearFrame);
	}
}

void GameScene::CloseYesNoDialog()
{
	if (auto dialog = m_pDialog.lock())
	{
		dialog->StartCloseCenter(kTutorialSkipDialogAppearFrame, false);
	}
}
