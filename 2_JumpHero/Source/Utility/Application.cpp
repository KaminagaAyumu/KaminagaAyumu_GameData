#include "Application.h"
#include "Input.h"
#include "Sound/SoundManager.h"
#include "../Scene/SceneController.h"
#include "../Scene/TitleScene.h"
#include "Game.h"
#include "../Game/ScoreManager.h"
#include "DxLib.h"
#include "EffekseerForDxLib.h"

namespace
{
	constexpr int	kOneFrameNanoSec = 16667; // 1フレームのナノ秒(60FPS)

	constexpr int	kEffectMaxNum = 8000; // Effekseerで画面に表示できる最大パーティクル数

	const wchar_t* kFontPath = L"data/craftmincho.otf"; // プロジェクト内にあるフォントデータのパス
}

void Application::RequestGameEnd()
{
	m_isGameEnd = true;
}

Application::Application()
{
	m_soundManager = std::make_shared<SoundManager>();
	// フォントデータをプロジェクトから読み込んで追加する(このプロジェクトの起動時にしか使えない)
	AddFontResourceExW(kFontPath, FR_PRIVATE, nullptr);

	m_isGameEnd = false;
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
	ChangeWindowMode(false);
	// ウインドウのタイトル変更
	SetMainWindowText(L"JumpHero");
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
	if (Effkseer_Init(kEffectMaxNum) == -1)
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

	// Zバッファを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetUseZBuffer3D(TRUE);

	// Zバッファへの書き込みを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetWriteZBuffer3D(TRUE);

	// スコアマネージャーの生成
	m_scoreManager = std::make_shared<ScoreManager>();
	// スコアデータのロード
	m_scoreManager->Load();

	return true;
}

void Application::Run()
{

	Input input;
	SceneController controller;

	// 最初のシーンをタイトルシーンに設定
	controller.ChangeScene(std::make_shared<TitleScene>(controller));
	// ゲームループ
	while (ProcessMessage() != -1 && !m_isGameEnd)
	{
		// このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレームに描画した内容をクリアする
		ClearDrawScreen();


		// ここにゲームの処理を書く
		input.Update(); // 入力情報の更新

		m_soundManager->Update(); // サウンドマネージャーの更新処理

		controller.Update(input); // シーンの更新処理
		controller.Draw(); // シーンの描画処理


		// escキーを押したらゲームを強制終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			RequestGameEnd();
		}

		// 描画した内容を画面に反映する
		ScreenFlip();

		// フレームレート60に固定
		while (GetNowHiPerformanceCount() - start < kOneFrameNanoSec)
		{

		}
	}

}

void Application::Terminate()
{
	// 追加したフォントデータを明示的に削除する
	RemoveFontResourceExW(kFontPath, FR_PRIVATE, nullptr);
	Effkseer_End();				// Effekseer使用の終了処理
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
