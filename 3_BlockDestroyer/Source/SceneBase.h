#pragma once
#include "InputManager.h"
#include "Bg.h"
#include "Sound.h"
#include "UI.h"

/// <summary>
/// すべてのシーンに必要な要素が入ったクラス 
/// </summary>
enum class NextScene
{
	SceneTitle, // タイトルシーン
	SceneSelect, // キャラ選択シーン
	SceneMain, // ゲームシーン
	SceneResult, // リザルトシーン
	SceneNum, // シーンの総数
	None // シーンなし
};

class SceneBase
{
public:
	// コンストラクタ
	SceneBase();
	// デストラクタ
	virtual ~SceneBase();

	virtual void Init();

	virtual void End();

	virtual void Update();

	virtual void Draw();

	virtual bool IsEnd() const { return m_isEnd; } // シーン終了フラグを取得

	virtual NextScene GetNextScene() const { return m_nextScene; } // 次のシーンを取得

	virtual void SetNextScene(NextScene nextScene) { m_nextScene = nextScene; } // 次のシーンを設定

protected:
	// 描画関係
	int m_bgHandle; // 背景画像のハンドル
	int m_bgmHandle;// BGMのハンドル
	int m_selectSeHandle; // 決定音のハンドル
	int m_cursorSeHandle; // カーソル移動音のハンドル

	Bg* m_bg;

	Sound* m_sound;

	UI* m_ui; // UIクラスのポインタ

	NextScene m_nextScene; // 次のシーン

	bool m_isEnd; // シーン終了フラグ

	InputManager* m_inputManager; // 入力管理クラスのポインタ

};

