#include "SceneController.h"
#include "SceneBase.h"
#include "../Game/Effect/EffekseerResourceManager.h"
#include "EffekseerForDxLib.h"

SceneController::SceneController()
{
	m_pEffekseerResourceManager = std::make_shared<EffekseerResourceManager>();
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

std::shared_ptr<EffekseerResourceManager> SceneController::GetEffekseerResourceManager() const
{
	return m_pEffekseerResourceManager;
}
