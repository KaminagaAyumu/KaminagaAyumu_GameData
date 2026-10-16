#include "DxLib.h"
#include "Game.h"
#include "SceneManager.h"


// 定数定義
namespace
{
	
}

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ウインドウモード設定
#ifdef _DEBUG
	ChangeWindowMode(true);
#endif // _DEBUG
	ChangeWindowMode(false);
	// ウインドウのタイトル変更
	SetMainWindowText("Block Destroyer");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, 32);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);
	SceneManager sceneManager;
	sceneManager.Init();

	while (ProcessMessage() != -1)
	{
		// このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレームに描画した内容をクリアする
		ClearDrawScreen();


		// ここにゲームの処理を書く
		sceneManager.Update();
		sceneManager.Draw();


		// 描画した内容を画面に反映する
		ScreenFlip();

		// escキーを押したらゲームを強制終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// フレームレート60に固定
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}

	sceneManager.End();

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}