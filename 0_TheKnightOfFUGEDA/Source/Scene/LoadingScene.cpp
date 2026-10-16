#include "LoadingScene.h"
#include "SceneController.h"
#include "../Utility/Game.h"
#include "../Utility/CSV/ResourceManifestData.h"
#include "../Geometry/Vector2Int.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// ロード進捗ゲージの大きさ
	const Vector2Int kLoadingGaugeSize = { 640, 100 };
	// ロード進捗ゲージの左端のX座標(画面の左右中央に配置する)
	const int kLoadingGaugeLeft = (Game::kScreenWidth - kLoadingGaugeSize.x) / 2;
	constexpr unsigned int kFillColor = 0xffd700;
	constexpr unsigned int kFrameColor = 0xffffff; // ゲージの枠の色

	constexpr unsigned int kTextColor = 0xffffff; // 文字色
	constexpr int kTextOffsetY = 70; // 画面中央から文字までのY方向のずらし量
	constexpr int kProgressTextOffsetX = 50; // 画面中央から進捗率の文字までのX方向のずらし量

	constexpr float kPercentRate = 100.0f; // 進捗率(0～1)を百分率に変換する値
}

LoadingScene::LoadingScene(std::function<std::shared_ptr<SceneBase>()> m_nextSceneFactory, std::wstring filePath, SceneController& controller, TransitionType transitionType) :
	SceneBase(controller),
	m_nextSceneFactory(m_nextSceneFactory),
	m_filePath(filePath),
	m_transitionType(transitionType)
{
}

LoadingScene::~LoadingScene()
{
}

void LoadingScene::Init()
{
	ResourceManifestData manifest;

	if (manifest.Load(m_filePath))
	{
		manifest.ConvertManifestData();

		auto& fileManager = FileManager::GetInstance();

		for (const auto& request : manifest.GetResourceRequests())
		{
			if (request.type == FileManager::FileType::Image)
			{
				fileManager.ReserveImage(request.path.c_str(), request.isEternal);
			}
			else if (request.type == FileManager::FileType::Model)
			{
				fileManager.ReserveModel(request.path.c_str(), request.isEternal);
			}
			else
			{
				assert(false && "LoadingScene:不明なリソースの種類です");
			}
		}
	}
	else
	{
		assert(false && "LoadingScene:リソースマニフェストの読み込みに失敗しました");
	}

	
}

void LoadingScene::End()
{
}

void LoadingScene::Update()
{
	if (!FileManager::GetInstance().IsLoadingComplete()) return;

	auto next = m_nextSceneFactory();
	switch (m_transitionType)
	{
	case LoadingScene::TransitionType::Change:
		m_sceneController.ChangeScene(next);
		break;
	case LoadingScene::TransitionType::Push:
		m_sceneController.PushScene(next);
		break;
	case LoadingScene::TransitionType::Reset:
		m_sceneController.ResetScene(next);
		break;
	default:
		break;
	}
}

void LoadingScene::Draw() const
{
	// ローディング画面の描画処理をここに追加する
	// 例: "Loading..."と表示する
	const int textY = Game::kScreenHeight / 2 - kTextOffsetY;
	const int gaugeTop = Game::kScreenHeight / 2 - kLoadingGaugeSize.y / 2;
	const int gaugeBottom = Game::kScreenHeight / 2 + kLoadingGaugeSize.y / 2;

	DrawString(kLoadingGaugeLeft, textY, L"Loading...", kTextColor);
	DrawFormatString(Game::kScreenWidth / 2 - kProgressTextOffsetX, textY, kTextColor, L"%.0f％", FileManager::GetInstance().GetLoadProgress() * kPercentRate);

	DrawBox(kLoadingGaugeLeft, gaugeTop, static_cast<int>(kLoadingGaugeLeft + kLoadingGaugeSize.x * (FileManager::GetInstance().GetLoadProgress())), gaugeBottom, kFillColor, true);
	DrawBox(kLoadingGaugeLeft, gaugeTop, kLoadingGaugeLeft + kLoadingGaugeSize.x, gaugeBottom, kFrameColor, false);

}
