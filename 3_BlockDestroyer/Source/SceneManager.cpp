#include "SceneManager.h"

SceneManager::SceneManager() :
	m_isFadeInEnd(false),
	m_currentSceneType(SceneType::SceneTitle),
	m_currentScene(nullptr)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	m_isFadeInEnd = false; // フェードイン終了フラグをfalseに設定
	m_currentSceneType = SceneType::SceneTitle;
	m_startScene.Init();
	m_currentScene = &m_startScene; // 現在のシーンをスタートシーンに設定
	m_fadeManager.Init(); // フェードマネージャーの初期化
}

void SceneManager::End()
{
	if (m_currentScene != nullptr)
	{
		m_currentScene = nullptr; // 現在のシーンをnullptrに設定
	}
}

void SceneManager::Update()
{
	m_fadeManager.Update(); // フェードマネージャーの更新
	if (m_currentScene == nullptr)
	{
		return; // 現在のシーンが設定されていない場合は何もしない
	}
	if (m_fadeManager.IsFading())
	{
		return; // フェード中はシーンの更新を行わない
	}

	if (m_currentScene->IsEnd())
	{
	
		if (!m_isFadeInEnd)
		{
			m_fadeManager.StartFadeIn(); // フェードインを開始
			m_isFadeInEnd = true; // フェードイン終了フラグをtrueに設定
			return; // フェードイン中はシーンの更新を行わない
		}
		if(m_fadeManager.IsFadeIn())
		{
			return; // フェードイン中はシーンの更新を行わない
		}
	
		switch (m_currentScene->GetNextScene())
		{
		case NextScene::SceneTitle:
			m_currentSceneType = SceneType::SceneTitle; // タイトルシーンに変更
			break;
		case NextScene::SceneSelect:
			m_currentSceneType = SceneType::SceneSelect; // セレクトシーンに変更
			break;
		case NextScene::SceneMain:
			m_currentSceneType = SceneType::SceneMain; // メインシーン(ゲームシーン)に変更
			break;
		case NextScene::SceneResult:
			m_currentSceneType = SceneType::SceneResult; // リザルトシーンに変更
			break;
		}
		
		m_currentScene->End(); // 現在のシーンの終了処理を行う

		ChangeScene(m_currentSceneType); // 次のシーンに変更
		m_currentScene->Init(); // 新しいシーンの初期化を行う
		m_fadeManager.Init(); // フェードアウトを開始
		m_isFadeInEnd = false; // フェードイン終了フラグをtrueに設定
	}
	else
	{
		m_currentScene->Update(); // 現在のシーンの更新を行う
		m_isFadeInEnd = false; // フェードイン終了フラグをtrueに設定
	}

}

void SceneManager::Draw()
{
	if (m_currentScene == nullptr)
	{
		return; // 現在のシーンが設定されていない場合は何もしない
	}
	m_currentScene->Draw(); // 現在のシーンの描画を行う
	m_fadeManager.Draw(); // フェードマネージャーの描画
}

void SceneManager::ChangeScene(SceneType newSceneType)
{
	switch (newSceneType)
	{
	case SceneType::SceneTitle:
		// タイトルシーン
		m_currentScene = &m_startScene; // 現在のシーンをタイトルシーンに設定
		break;
	case SceneType::SceneSelect:
		// セレクトシーン
		m_currentScene = &m_selectScene; // 現在のシーンをセレクトシーンに設定
		break;
	case SceneType::SceneMain:
		// メインシーン(ゲームシーン)
		m_currentScene = &m_sceneMain; // 現在のシーンをメインシーン(ゲームシーン)に設定
		break;
	default:
		break;
	}
}
