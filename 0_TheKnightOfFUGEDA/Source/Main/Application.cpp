#include "Application.h"
#include "../Scene/SceneController.h"
#include "../Scene/DebugScene.h"
#include "../Scene/TitleScene.h"
#include "../Scene/LoadingScene.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/CameraManager.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/CSV/ResourceManifestData.h"
#include "../MyLib/Renderer.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/Physics.h"
#include "../Common/Effect/EffectManager.h"
#include "../Common/Sound/SoundManager.h"
#include "DxLib.h"
#include "EffekseerForDxLib.h"

namespace
{
	const wchar_t* kFontPath = L"Data/File/Font/HGRME.TTC";

	//const wchar_t* kFontPath = L"Data/File/Font/CP_Revenge.ttf";
}

void Application::SetHighScore(int score, int stageNo)
{
	m_highScores[stageNo] = score;
}

std::vector<int> Application::GetHighScore()
{
	return m_highScores;
}

Application::Application() :
	m_isGameEnd(false),
	m_timeScale(1.0f)
{
	// フォントデータをプロジェクトから読み込んで追加する(このプロジェクトの起動時にしか使えない)
	AddFontResourceExW(kFontPath, FR_PRIVATE, nullptr);
}

Application::~Application()
{
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	// ウインドウモード設定
#ifdef _DEBUG
	ChangeWindowMode(true);
#else
	ChangeWindowMode(false);
#endif
	// ウインドウのタイトル変更
	SetMainWindowText(L"TheKnightOfFUGEDA");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	// DirectX11を使用するようにする。
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);


	//------------------------------//
	// エフェクト関連の初期化
	//------------------------------//

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(Game::kEffectMaxNum) == -1)
	{
		// 初期化できなかった場合終わる
		DxLib_End();
		return false;
	}

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Effekseerに2D描画の設定をする。
	Effekseer_Set2DSetting(Game::kScreenWidth, Game::kScreenHeight);

	// Effekseerの歪み機能を有効にする。
	Effekseer_InitDistortion();

	// カリングの設定
	SetUseBackCulling(true);

	// Zバッファを有効にする。
	SetUseZBuffer3D(TRUE);

	// Zバッファへの書き込みを有効にする。
	SetWriteZBuffer3D(TRUE);

	// Effekseerに3D描画の設定をする。
	Effekseer_Sync3DSetting();

	return true;
}

void Application::Run()
{
	// 入力情報管理用クラスのインスタンスを取得
	Input& input = Input::GetInstance();

	// リソース管理用クラスのインスタンスを取得
	FileManager& fileManager = FileManager::GetInstance();

	// 描画管理用クラスのインスタンスを取得
	MyLib::Renderer& renderer = MyLib::Renderer::GetInstance();
	renderer.Init(); // 初期化

	// カメラ管理用クラスのインスタンスを取得
	CameraManager& cameraManager = CameraManager::GetInstance();
	cameraManager.Init(); // 初期化

	EffectManager& effectManager = EffectManager::GetInstance();
	effectManager.Init();

	SoundManager& soundManager = SoundManager::GetInstance();
	soundManager.Init();

	// オブジェクト管理用クラスのインスタンスを取得
	MyLib::ObjectManager& objectManager = MyLib::ObjectManager::GetInstance();

	// 当たり判定、位置更新用クラスのインスタンスを取得
	MyLib::Physics& physics = MyLib::Physics::GetInstance();
	// 当たり判定処理を初期化
	physics.Init(Game::kOctreeLevel, BoundingBox{Vector3::SetAll(Game::kMinStageSize), Vector3::SetAll(Game::kMaxStageSize) });

	// ステージの数分ハイスコアを設定、初期化
	m_highScores.assign(Game::kStageNum + 1, 0);

	// シーン管理クラスのインスタンスを生成
	SceneController controller;
#ifdef _DEBUG
	// 最初のシーンをデバッグシーンに設定
	controller.ChangeScene(std::make_shared<DebugScene>(controller));

#else

	// 最初のシーンをタイトルシーンに設定
	controller.ChangeScene(std::make_shared<LoadingScene>(
		[&] {return std::make_shared<TitleScene>(controller); }, L"Data/File/CSV/Resource/title_scene.csv", controller, LoadingScene::TransitionType::Change));

	//controller.ChangeScene(std::make_shared<TitleScene>(controller));
#endif
	// ゲームループ
	while (ProcessMessage() != -1 && !m_isGameEnd)
	{
		// このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレームに描画した内容をクリアする
		ClearDrawScreen();


#ifdef _DEBUG
		// デバッグ用の描画状態をリセット
		renderer.DebugClear();
#endif 

		fileManager.Update(); // リソースの更新

		input.Update(); // 入力情報の更新

		objectManager.Update(); // ゲームオブジェクトの更新

		physics.Update(); // 当たり判定、位置の更新

		effectManager.Update();

		cameraManager.Update(); // カメラ情報の更新

		soundManager.Update();

#ifdef _DEBUG
		// ポーズ状態(更新を止める)
		if (input.IsTriggered("Pause") && m_debugState == DebugState::Normal)
		{
			m_debugState = DebugState::Pause;
		}

		// ポーズ状態を解除
		if (input.IsTriggered("OK") && m_debugState == DebugState::Pause)
		{
			m_debugState = DebugState::Normal;
		}

		// ポーズ中ではなく、ポーズのボタンが押されていない時は更新しない
		if (m_debugState != DebugState::Pause || input.IsTriggered("Pause"))
		{

#endif
			controller.Update(); // シーンの更新処理


			renderer.Draw(); // 描画処理

			controller.Draw(); // シーンの描画処理

#ifdef _DEBUG
			// デバッグ用の描画を行う
			renderer.DebugDraw();
#endif 

			// escキーを押したらゲームを強制終了
			if (CheckHitKey(KEY_INPUT_ESCAPE))
			{
				RequestGameEnd();
			}

			// 描画した内容を画面に反映する
			ScreenFlip();

#ifdef _DEBUG
		}
#endif

		// フレームレート60に固定
		while (GetNowHiPerformanceCount() - start < Game::kOneFrameNanoSec)
		{

		}
	}

}

void Application::Terminate()
{
	// 描画クラスの終了処理を行う
	MyLib::Renderer::GetInstance().End();

	// ゲームオブジェクト管理用クラスの終了処理を行う
	MyLib::ObjectManager::GetInstance().End();

	SoundManager::GetInstance().End();

	// ファイルのリソースを開放する
	FileManager::GetInstance().End();

	// カメラ管理用クラスの終了処理を行う
	CameraManager::GetInstance().End();
	
	// 追加したフォントデータを明示的に削除する
	RemoveFontResourceExW(kFontPath, FR_PRIVATE, nullptr);

	Effkseer_End();				// Effekseer使用の終了処理
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}

void Application::RequestGameEnd()
{
	m_isGameEnd = true;
}
