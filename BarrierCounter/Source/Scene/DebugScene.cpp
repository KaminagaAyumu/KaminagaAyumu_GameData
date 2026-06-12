#include "DebugScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"
#include "ClearScene.h"
#include "GameoverScene.h"
#include "SceneController.h"
#include "../Common/Input.h"
#include "../Common/Game.h"
#include "../Common/GameManager.h"

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
}

DebugScene::DebugScene(SceneController& controller) : 
	SceneBase(controller),
	m_cursor(0)
{
	m_selectScenes.push_back(SelectData{ L"TitleScene",
		[this]()
		{
			m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		} 
	});

	m_selectScenes.push_back(SelectData{ L"GameScene",
		[this]()
		{
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
		} 
	});

	m_selectScenes.push_back(SelectData{ L"ResultScene",
		[this]()
		{
			m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller));
		} 
	});

	m_selectScenes.push_back(SelectData{ L"ClearScene",
		[this]()
		{
			m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller));
		} 
	});

	m_selectScenes.push_back(SelectData{ L"GameoverScene",
		[this]()
		{
			m_controller.ChangeScene(std::make_shared<GameoverScene>(m_controller));
		} 
	});

}

DebugScene::~DebugScene()
{
}

void DebugScene::Update(Input& input)
{
	if (m_selectScenes.empty())
	{
		return;
	}

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

void DebugScene::Draw()
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

		DrawString(x + advance, baselineY, m_selectScenes[i].text.c_str(), isSelected ? 0xffffff : kSelectTextColor);

		// 次の項目のy座標に進める
		y += kDefaultItemSpacing;
		// y座標がこれ以上進めない場合処理を抜ける
		if (y > Game::kScreenHeight - kDefaultPaddingY) break;
	}
}
