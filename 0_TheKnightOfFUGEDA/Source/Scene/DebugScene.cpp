#include "DebugScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "ResultScene.h"
#include "LoadingScene.h"
#include "../Utility/Input.h"
#include "SceneController.h"
#include "../Utility/File/FileManager.h"
#include "../Utility/File/ImageFile.h"
#include "../Utility/CSV/PlayerAttackResource.h"
#include "../Object/Player/Player.h"
#include "../Object/Enemy/EnemyManager.h"
#include "../Object/Camera/PlayerCamera.h"
#include "../MyLib/MyMath.h"
#include "../MyLib/ObjectFactory.h"
#include "../MyLib/ObjectManager.h"
#include "../MyLib/Component/Transform.h"
#include "../MyLib/Component/Draw/Drawable3D.h"
#include "../Main/Application.h"
#include "../Utility/Game.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFirstCreateList = 1; // リストを生成した際のサイズ

	constexpr int kDefaultPaddingX = 16; // リストの左右端からテキストまでの余白の初期値
	constexpr int kDefaultPaddingY = 10; // リストの上下端からテキストまでの余白の初期値
	constexpr int kDefaultItemSpacing = 30; // テキストとテキストの間の余白の初期値

	constexpr int kWindowAlpha = 200; // テキストを表示する範囲の透明度

	constexpr int kOutlineMargin = 2; // 影として使うテキストのずらす値

	constexpr float kTextWaveSpeed = 0.15f; // テキストの動く速さ(フレームカウンタを参照)
	constexpr float kTextWaveSize = 4.0f; // テキストを動かす際の動く範囲
	constexpr int kStringOneSize = 1; // 1文字のサイズを示す

	constexpr unsigned int kDefaultBgColor = 0xff2200; // 背景をBoxで表示する際の色
	constexpr unsigned int kSelectTextColor = 0xff00ff; // 選択されている際のテキストの色
	constexpr unsigned int kCursorTextColor = 0xffffff; // カーソルが合っている項目のテキストの色
}

DebugScene::DebugScene(SceneController& controller) :
	SceneBase(controller),
	m_cursor(0)
{
}

DebugScene::~DebugScene()
{
	// 念のため終了処理を呼ぶようにする
	//End();
}

void DebugScene::Init()
{
	m_selectScenes.push_back(SelectData{ L"TitleScene",
		[this]()
		{
			m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
			[&controller = m_sceneController] {return std::make_shared<TitleScene>(controller); }, L"Data/File/CSV/Resource/title_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
		}
	});
	m_selectScenes.push_back(SelectData{ L"SelectScene",
		[this]()
		{
			m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
			[&controller = m_sceneController] {return std::make_shared<SelectScene>(controller); }, L"Data/File/CSV/Resource/select_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
		}
	});
	m_selectScenes.push_back(SelectData{ L"GameScene",
		[this]()
		{
			m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
			[&controller = m_sceneController] {return std::make_shared<GameScene>(controller, Game::kTutorialStageNo); }, L"Data/File/CSV/Resource/game_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
		}
	});
	m_selectScenes.push_back(SelectData{ L"GameScene1",
		[this]()
		{
			m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
			[&controller = m_sceneController] {return std::make_shared<GameScene>(controller, Game::kStage1No); }, L"Data/File/CSV/Resource/game_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
		}
	});
	m_selectScenes.push_back(SelectData{ L"GameScene2",
		[this]()
		{
			m_sceneController.ChangeScene(std::make_shared<LoadingScene>(
			[&controller = m_sceneController] {return std::make_shared<GameScene>(controller, Game::kStage2No); }, L"Data/File/CSV/Resource/game_scene.csv", m_sceneController, LoadingScene::TransitionType::Change));
		}
	});
}

void DebugScene::End()
{
	//m_pPlayer->End();
}

void DebugScene::Update()
{
	if (m_selectScenes.empty())
	{
		return;
	}

	Input& input = Input::GetInstance();

	if (input.IsTriggered("Up"))
	{
		m_cursor = (m_cursor - 1 + static_cast<int>(m_selectScenes.size())) % static_cast<int>(m_selectScenes.size());
	}

	if (input.IsTriggered("Down"))
	{
		m_cursor = (m_cursor + 1 + static_cast<int>(m_selectScenes.size())) % static_cast<int>(m_selectScenes.size());
	}

	if (input.IsTriggered("OK"))
	{
		if (0 <= m_cursor && m_cursor < static_cast<int>(m_selectScenes.size()))
		{
			if (m_selectScenes[m_cursor].onSelect)
			{
				m_selectScenes[m_cursor].onSelect();
			}
		}
	}

}

void DebugScene::Draw() const
{

	// ここはデフォルトから変えられるようにするかもしれない
	const int paddingX = kDefaultPaddingX;

	// テキストを描画する範囲
	const int areaW = Game::kScreenWidth - paddingX * 2;

	// 項目を並べるY座標
	int y = 0 + kDefaultPaddingY;

	// 項目の数だけ並べていく処理
	for (int i = 0; i < static_cast<int>(m_selectScenes.size()); i++)
	{
		// カーソルがこの項目にあっているか
		const bool isSelected = (i == m_cursor);

		// 表示するX座標を確認
		int x = 0 + paddingX;

		// 垂直位置の基準Y座標
		int baselineY = y + (kDefaultItemSpacing) / 2;

		// 文字を1文字ずつ動かす際に進めるための値
		int advance = 0;
		// 文字が波打つスピードを調整
		float t = m_frameCount * kTextWaveSpeed;

		DrawString(x + advance, baselineY, m_selectScenes[i].text.c_str(), isSelected ? kCursorTextColor : kSelectTextColor);

		// 次の項目のy座標に進める
		y += kDefaultItemSpacing;
		// y座標がこれ以上進めない場合処理を抜ける
		if (y > Game::kScreenHeight - kDefaultPaddingY) break;
	}
}