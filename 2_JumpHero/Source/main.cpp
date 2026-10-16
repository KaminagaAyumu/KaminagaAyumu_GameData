#include "DxLib.h"
#include "Utility/Game.h"
#include "Utility/Application.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Application クラスのインスタンスを取得
	auto& app = Application::GetInstance();
	// アプリケーションの初期化
	if (!app.Init())
	{
		return -1;			// 初期化に失敗したら終了
	}
	// ゲームループの開始
	app.Run();
	// アプリケーションの終了処理
	app.Terminate();

	return 0;				// ソフトの終了 
}