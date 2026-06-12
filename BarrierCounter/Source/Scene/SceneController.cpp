#include "SceneController.h"
#include "SceneBase.h"
#include "../Common/GameManager.h"
#include "EffekseerForDxLib.h"

SceneController::SceneController()
{
	// ゲーム管理クラスのインスタンスを作成
	m_pGameManager = std::make_shared<GameManager>();
	// ゲームマネージャーを初期化
	m_pGameManager->Init();
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
		// シーンリストが空でなければ最後のシーンを置き換える
		m_scenes.back() = scene;
	}
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	// シーンリストに新しいシーンを追加する
	m_scenes.push_back(scene);
}

void SceneController::PopScene()
{
	// シーンリストの最後のシーンを削除する
	if (!m_scenes.empty()) // 念のため空でないか確認
	{
		m_scenes.pop_back();
	}
}

void SceneController::ResetScene(std::shared_ptr<SceneBase> scene)
{
	// シーンリストをクリアして新しいシーンだけにする
	m_scenes.clear();
	m_scenes.push_back(scene);
}

void SceneController::Update(Input& input)
{
	// 最後に追加されたシーンの更新処理のみ行う
	m_scenes.back()->Update(input);
	UpdateEffekseer2D();
}

void SceneController::Draw()
{
	// すべてのシーンの描画処理を行う(ポーズシーンなどで使えるため)
	for (auto& scene : m_scenes)
	{
		scene->Draw();
	}
	DrawEffekseer2D();
}

std::weak_ptr<GameManager> SceneController::GetGameManager()
{
	return m_pGameManager;
}
