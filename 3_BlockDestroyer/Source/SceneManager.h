#pragma once
#include "StartScene.h"
#include "SelectScene.h"
#include "SceneMain.h"
#include "FadeManager.h"

enum class SceneType
{
	SceneTitle,
	SceneSelect,
	SceneMain,
	SceneResult,
	None
};

class SceneManager
{
public:
	// コンストラクタ
	SceneManager();
	// デストラクタ
	~SceneManager();

	// 初期化
	void Init();
	void End();
	void Update();
	void Draw();

	SceneType SetCurrentSceneType(SceneType newSceneType) { m_currentSceneType = newSceneType; }
	SceneType GetCurrentSceneType() const { return m_currentSceneType; }
	void ChangeScene(SceneType newSceneType);

private:
	// シーンの状態を更新する関数
	SceneType m_currentSceneType;

	bool m_isFadeInEnd; // フェードインが終了したかどうかのフラグ

	SceneBase* m_currentScene; // 現在のシーン

	// 各シーンのインスタンス
	StartScene m_startScene;
	SelectScene m_selectScene;
	SceneMain m_sceneMain;

	// シーン遷移時のフェード管理
	FadeManager m_fadeManager;
};

