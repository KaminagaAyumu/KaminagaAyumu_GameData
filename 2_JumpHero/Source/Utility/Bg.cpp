#include <vector>
#include <memory>
#include <string>
#include "Bg.h"
#include "Game.h"
#include "Camera.h"
#include <cassert>
#include "DxLib.h"

namespace
{
	constexpr int kStageSelectBgNo = 0;
	constexpr int kStageClearBgNo = 1;

	const Position2 kDefaultSpeed = { 1.0f,1.0f };
	const Position2 kDefaultParallax = { 0.0f,0.0f };
	const Position2 kParallaxStep = { 0.2f,0.2f };
	const Position2 kDefaultBasePos = { 0.0f,0.0f };
}

Bg::Bg() :
	m_pos{}
{
	//m_bgHandle = LoadGraph(L"data/img/logo_back.png");
	//m_bgHandle = LoadGraph(L"data/img/bg_underground.png");
	m_bgHandle = -1;
	m_normalBgHandle = -1;
	m_layers.clear();

	auto handle = LoadGraph(L"data/background_1.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_2.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_3.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_4.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_5.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_6.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);

	SetBgType(Types::BgType::Loop);
}

Bg::~Bg()
{
	DeleteGraph(m_bgHandle);
	DeleteGraph(m_normalBgHandle);
	for (auto handle : m_bgHandles)
	{
		DeleteGraph(handle);
	}
}

void Bg::Init()
{
	if (m_updateFunc == &Bg::LoopUpdate)
	{
		m_bgHandle = LoadGraph(L"data/img/logo_back.png");
	}
	else
	{
		m_bgHandle = LoadGraph(L"data/img/bg_underground.png");
	}
}

void Bg::Update()
{
	(this->*m_updateFunc)();
}

void Bg::Draw()
{
	// 動かない背景画像のデータが入っていたらそれを描画する
	if (m_normalBgHandle != -1)
	{
		DrawGraph(0, 0, m_normalBgHandle, true);
	}
	(this->*m_drawFunc)(nullptr);
}


void Bg::Draw(std::weak_ptr<Camera> camera)
{
	(this->*m_drawFunc)(camera.lock());
}

void Bg::SetBgType(Types::BgType type)
{
	switch (type)
	{
	case Types::BgType::Loop:
			m_updateFunc = &Bg::LoopUpdate;
			m_drawFunc = &Bg::LoopDraw;
			break;
	case Types::BgType::ScrollX:
		m_updateFunc = &Bg::ScrollXUpdate;
		m_drawFunc = &Bg::ScrollXDraw;
		break;
	case Types::BgType::ScrollXY:
		m_updateFunc = &Bg::ScrollUpdate;
		m_drawFunc = &Bg::ScrollDraw;
		break;
	}
}

void Bg::SetLayer()
{
	const size_t layerSize = m_bgHandles.size();
	m_layers.reserve(layerSize);

	for (size_t i = 0; i < layerSize; i++)
	{
		const int handle = m_bgHandles[i];
		
		// ハンドルが見つからなかった場合はレイヤーに追加しない
		if (handle == -1)
		{
			continue;
		}

		// カメラのスクロールを変える値をレイヤーごとに設定
		Position2 parallax = { kDefaultParallax.x + kParallaxStep.x * static_cast<float>(i),
			kDefaultParallax.y + kParallaxStep.y * static_cast<float>(i)
		};

		BgLayer layer;
		layer.Init(handle, kDefaultSpeed, parallax, kDefaultBasePos);
		m_layers.emplace_back(std::move(layer));
	}
}

void Bg::LoadNormalBg(int pathNo)
{
	if (pathNo == kStageSelectBgNo)
	{
		m_normalBgHandle = LoadGraph(L"data/img/bg_select.png");
	}
	else if (pathNo == kStageClearBgNo)
	{
		m_normalBgHandle = LoadGraph(L"data/img/bg_gameClear.png");
	}
	else
	{
		m_normalBgHandle = LoadGraph(L"data/img/bg_gameOver.png");
	}
}

void Bg::LoopUpdate()
{
	m_pos.x++;
	m_pos.y++;
}

void Bg::ScrollUpdate()
{
}

void Bg::ScrollXUpdate()
{
}

void Bg::LoopDraw(std::shared_ptr<Camera>)
{
	Size bgSize = {};
	GetGraphSize(m_bgHandle, &bgSize.width, &bgSize.height);

	Position2Int pos = { 
		-Geometry::RemainderToNaturalNumber(static_cast<int>(m_pos.x), bgSize.width),
		-Geometry::RemainderToNaturalNumber(static_cast<int>(m_pos.y), bgSize.height) };

	DrawTile(m_bgHandle, bgSize, pos);
}

void Bg::ScrollDraw(std::shared_ptr<Camera> camera)
{
	// レイヤーが複数ある場合
	if (!m_layers.empty())
	{
		// カメラのスクロールを取得
		Position2 scroll = { 0.0f,0.0f };

		// カメラが取得できている場合、スクロールを使う
		if (camera)
		{
			scroll.x = camera->scroll.x;
			scroll.y = camera->scroll.y;
		}

		// レイヤーの分背景を描画
		for (auto& layer : m_layers)
		{
			// スクロール量を調節
			Position2 offset = {
			scroll.x * layer.parallax.x + layer.offset.x,
			scroll.y * layer.parallax.y + layer.offset.y,
			};

			// 開始位置を調整する値
			Position2Int pos = {
				Geometry::RemainderToNaturalNumber(static_cast<int>(offset.x),layer.size.width),
				Geometry::RemainderToNaturalNumber(static_cast<int>(offset.y),layer.size.height)
			};

			// 実際の開始位置
			Position2Int basePos{
			static_cast<int>(layer.basePos.x) - pos.x,
			static_cast<int>(layer.basePos.y) - pos.y
			};

			DrawTile(layer.handle, layer.size, basePos);
		}
	}
	else // レイヤーが存在しない場合1枚の背景描画処理を行う
	{
		DrawScrollSingle(camera);
	}
}

void Bg::ScrollXDraw(std::shared_ptr<Camera> camera)
{
	// レイヤーが複数ある場合
	if (!m_layers.empty())
	{
		// カメラのスクロールを取得
		Position2 scroll = { 0.0f,0.0f };

		// カメラが取得できている場合、スクロールを使う
		if (camera)
		{
			scroll.x = camera->scroll.x;
		}

		// レイヤーの分背景を描画
		for (auto& layer : m_layers)
		{
			// スクロール量を調節
			Position2 offset = {
			scroll.x * layer.parallax.x + layer.offset.x,
			0.0f,
			};

			// 開始位置を調整する値
			Position2Int pos = {
				Geometry::RemainderToNaturalNumber(static_cast<int>(offset.x),layer.size.width),
				0
			};

			// 実際の開始位置
			Position2Int basePos{
			static_cast<int>(layer.basePos.x) - pos.x,
			0
			};

			DrawTile(layer.handle, layer.size, basePos);
		}
	}
	else // レイヤーが存在しない場合1枚の背景描画処理を行う
	{
		DrawScrollSingleX(camera);
	}
}

void Bg::DrawTile(int handle, const Size& size, const Position2Int& pos)
{
	DrawGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), handle, true);

	const bool needX = (pos.x + size.width < Game::kScreenWidth);
	const bool needY = (pos.y + size.height < Game::kScreenHeight);

	if (needX)
	{
		DrawGraph(pos.x + size.width, pos.y, handle, true);
	}

	if (needY)
	{
		DrawGraph(pos.x, pos.y + size.height, handle, true);
	}

	if (needX && needY)
	{
		DrawGraph(pos.x + size.width, pos.y + size.height, handle, true);
	}

}

void Bg::DrawScrollSingle(std::shared_ptr<Camera> camera)
{
	Size bgSize = {};
	GetGraphSize(m_bgHandle, &bgSize.width, &bgSize.height);

	Position2 scroll = {0.0f,0.0f};
	
	// カメラが取得できている場合、スクロールを使う
	if (camera)
	{
		scroll.x = camera->scroll.x;
		scroll.y = camera->scroll.y;
	}

	scroll.x = Geometry::RemainderToNaturalNumberF(scroll.x, static_cast<float>(bgSize.width));
	scroll.y = Geometry::RemainderToNaturalNumberF(scroll.y, static_cast<float>(bgSize.height));

	Position2Int pos = {
		-static_cast<int>(scroll.x),
		-static_cast<int>(scroll.y) };

	DrawTile(m_bgHandle, bgSize, pos);
}

void Bg::DrawScrollSingleX(std::shared_ptr<Camera> camera)
{
	Size bgSize = {};
	GetGraphSize(m_bgHandle, &bgSize.width, &bgSize.height);

	Position2 scroll = { 0.0f,0.0f };

	// カメラが取得できている場合、スクロールを使う
	if (camera)
	{
		scroll.x = camera->scroll.x;
	}

	scroll.x = Geometry::RemainderToNaturalNumberF(scroll.x, static_cast<float>(bgSize.width));

	Position2Int pos = {
		-static_cast<int>(scroll.x),
		0 };

	DrawTile(m_bgHandle, bgSize, pos);
}

void Bg::BgLayer::Init(int _handle, const Position2& _speed, const Position2& _parallax, Position2 _base)
{
	handle = _handle;
	GetGraphSize(handle, &size.width, &size.height);
	speed = _speed;
	parallax = _parallax;
	basePos = _base;
	offset = { 0.0f,0.0f };
}
