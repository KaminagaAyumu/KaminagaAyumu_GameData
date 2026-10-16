#include "SceneController.h"
#include "SceneBase.h"

SceneController::SceneController() : 
	m_pendingType(PendingType::None)
{
	m_pendingScene.reset();
}

SceneController::~SceneController()
{
}

void SceneController::RequestChangeScene(std::shared_ptr<SceneBase> scene)
{
	m_pendingScene = scene;
	m_pendingType = PendingType::Change;
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	// シーンリストが空なら追加
	if (m_scenes.empty())
	{
		m_scenes.push_back(scene);
	}
	else
	{
		// 現在のシーンの終了処理を行う
		m_scenes.back()->End();
		// シーンリストが空でなければ最後のシーンを置き換える
		m_scenes.back() = scene;
	}
	// 新しいシーンの初期化処理を行う
	m_scenes.back()->Init();
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	// シーンリストに新しいシーンを追加する
	m_scenes.push_back(scene);
	// 新しいシーンの初期化処理を行う
	m_scenes.back()->Init();
}

void SceneController::PopScene()
{
	// シーンリストの最後のシーンを削除する
	if (!m_scenes.empty()) // 念のため空でないか確認
	{
		// 現在のシーンの終了処理を行う
		m_scenes.back()->End();
		// シーンリストから取り除く
		m_scenes.pop_back();
	}
}

void SceneController::ResetScene(std::shared_ptr<SceneBase> scene)
{
	// すべてのシーンの終了処理を行う
	for (const auto& scene : m_scenes)
	{
		scene->End();
	}
	// シーンリストをクリアして新しいシーンだけにする
	m_scenes.clear();
	// シーンリストに新しいシーンを追加する
	m_scenes.push_back(scene);
	// 新しいシーンの初期化処理を行う
	m_scenes.back()->Init();
}

void SceneController::Update()
{
	ApplyPendingSceneChange();

	// 最後に追加されたシーンの描画処理のみを行う(ポーズ処理などで使う)
	m_scenes.back()->Update();
}

void SceneController::Draw() const
{
	// すべてのシーンの描画処理を行う
	for (const auto& scene : m_scenes)
	{
		scene->Draw();
	}
}

void SceneController::ApplyPendingSceneChange()
{
	if (m_pendingType == PendingType::None) return;

	if (m_pendingType == PendingType::Change)
	{
		if (!m_scenes.empty()) m_scenes.back()->End();

		if (m_scenes.empty())
		{
			m_scenes.push_back(m_pendingScene);
		}
		else
		{
			m_scenes.back() = m_pendingScene;
		}

		m_scenes.back()->Init();
	}

	m_pendingScene.reset();
	m_pendingType = PendingType::None;
}
