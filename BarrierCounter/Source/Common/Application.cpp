#include "Application.h"
#include "Input.h"
#include "../Sound/SoundManager.h"
#include "../Scene/SceneController.h"
#include "../Scene/TitleScene.h"
#include "../Scene/DebugScene.h"
#include "../Effect/EffectManager.h"
#include "../Common/UI/UIManager.h"
#include "Game.h"
#include "../MyLib/MyLib.h"
#include "DxLib.h"
#include "EffekseerForDxLib.h"

namespace
{
	constexpr int	kOneFrameNanoSec = 16667; // 1フレームのナノ秒(60FPS)

	constexpr int	kEffectMaxNum = 8000; // Effekseerで画面に表示できる最大パーティクル数

	const wchar_t* kFontPath = L"Data/Font/CP_Revenge.ttf"; // プロジェクト内にあるフォントデータのパス
}

#ifdef _DEBUG
// デバッグの状況
enum class DebugState
{
	Normal, // 通常
	Pause,	// ポーズ(コマ送り可)
};
DebugState debugState = DebugState::Normal;
#endif

using namespace MyLib;

void Application::RequestGameEnd()
{
	m_isGameEnd = true;
}

Application::Application()
{
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
	SetMainWindowText(L"BarrierCounter");
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
	auto& input = Input::GetInstance();

	// サウンド管理クラスのインスタンスを取得
	auto& soundManager = SoundManager::GetInstance();
	// 初期化処理を行う
	soundManager.Init();

	// エフェクト管理クラスのインスタンスを取得
	auto& effectManager = EffectManager::GetInstance();
	// 初期化処理を行う
	effectManager.Init();

	// UI管理クラスのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	// 初期化処理を行う
	uiManager.Init();

	// デバッグ用の描画処理のインスタンスを取得
	auto& debugDraw = DebugDraw::GetInstance();

	// 物理挙動管理クラスのインスタンスを取得
	auto& physics = Physics::GetInstance();

	// シーン管理クラスのインスタンスを生成
	SceneController controller;
#ifdef _DEBUG
	// 最初のシーンをデバッグシーンに設定
	controller.ChangeScene(std::make_shared<DebugScene>(controller));
#else
	// 最初のシーンをタイトルシーンに設定
	controller.ChangeScene(std::make_shared<TitleScene>(controller));
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
		debugDraw.Clear();
#endif 


		// ここにゲームの処理を書く
		input.Update(); // 入力情報の更新

#ifdef _DEBUG
		// ポーズ状態(更新を止める)
		if(input.IsTriggered("Pause") && debugState == DebugState::Normal)
		{
			debugState = DebugState::Pause;
		}

		// ポーズ状態を解除
		if (input.IsTriggered("OK") && debugState == DebugState::Pause)
		{
			debugState = DebugState::Normal;
		}

		// ポーズ中ではなく、ポーズのボタンが押されていない時は更新しない
		if (debugState != DebugState::Pause || input.IsTriggered("Pause"))
		{

#endif
			controller.Update(input); // シーンの更新処理
			physics.Update(); // 物理挙動の更新処理
			soundManager.Update(); // サウンドの更新処理
			effectManager.Update(); // エフェクトの更新処理
			uiManager.Update(); // UIの更新処理

			controller.Draw(); // シーンの描画処理

#ifdef _DEBUG
			// デバッグ用の描画を行う
			debugDraw.Draw();
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
		while (GetNowHiPerformanceCount() - start < kOneFrameNanoSec)
		{

		}
	}

}

void Application::Terminate()
{
	// エフェクトマネージャーの終了処理を行う
	EffectManager::GetInstance().End();
	// サウンドマネージャーの終了処理を行う
	SoundManager::GetInstance().End();
	// UIマネージャーの終了処理を行う
	UIManager::GetInstance().End();
	// 追加したフォントデータを明示的に削除する
	RemoveFontResourceExW(kFontPath, FR_PRIVATE, nullptr);
	Effkseer_End();				// Effekseer使用の終了処理
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
