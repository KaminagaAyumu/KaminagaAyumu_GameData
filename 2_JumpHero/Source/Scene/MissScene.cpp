#include "MissScene.h"
#include "SceneController.h"
#include "DxLib.h"
#include <string>

namespace
{
	constexpr int kMaxFrame = 300;
	constexpr int kMaxFadeRate = 255; // フェード進行率の最大値
}

MissScene::MissScene(SceneController& controller) : SceneBase(controller)
{
}

MissScene::~MissScene()
{
}

void MissScene::Update(Input& input)
{
	m_frameCount++;

	if (m_frameCount >= kMaxFrame)
	{
		m_controller.PopScene();
		return;
	}
	
}

void MissScene::Draw()
{

#ifdef _DEBUG
	DrawString(0, 0, L"MissScene: Draw", 0xFFFFFF);
#endif
}
