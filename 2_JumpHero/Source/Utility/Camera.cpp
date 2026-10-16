#include "Camera.h"
#include "Game.h"
#include <cassert>

namespace
{
	constexpr float kLerpSpeed = 0.05f;

	constexpr float kLerpEndDistance = 0.5f;
}

Camera::Camera(const Size& size) :
	m_mode(Mode::None),
	scroll{},
	m_pos{},
	m_mapSize(size),
	m_fixedTarget{}
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	const Position2 target = GetCurrentTarget(); // 現在のターゲットを取得

	// カメラの座標を初期化
	if (!IsLerpEnd()) // 補正が終わっていたら
	{
		m_pos = target; // カメラの座標をターゲットに固定
	}
}

void Camera::Update()
{

	if (m_mode == Mode::None)
	{
		// モードなしの場合ポジション補正を行わない
	}
	else
	{
		const Position2 target = GetCurrentTarget(); // 現在のターゲットを取得
		// カメラの座標を補正
		if (IsLerpEnd()) // 補正が終わっていたら
		{
			m_pos = target; // カメラの座標をターゲットに固定
		}
		else
		{
			m_pos = Geometry::LerpVec2(m_pos, target, kLerpSpeed); // 補正してカメラを動かす
		}
	}

	// スクロール量の計算
	// カメラの座標を画面中央にずらす
	scroll.x = m_pos.x - Game::kScreenWidth / 2;
	scroll.y = m_pos.y - Game::kScreenHeight / 2;

	// カメラがスクロール開始位置より左にいる時はスクロールさせない
	if (scroll.x < 0)
	{
		scroll.x = 0;
	}
	// カメラがスクロール終了位置より右にある時はそれ以上スクロールさせない
	if (scroll.x > m_mapSize.width - Game::kScreenWidth)
	{
		scroll.x = static_cast<float>(m_mapSize.width) - Game::kScreenWidth;
	}

	// カメラがスクロール開始位置より上にいるときはスクロールさせない
	if (scroll.y < 0)
	{
		scroll.y = 0;
	}
	// カメラがスクロール終了位置より下にいるときはそれ以上スクロールさせない
	if (scroll.y > m_mapSize.height - Game::kScreenHeight)
	{
		scroll.y = static_cast<float>(m_mapSize.height) - Game::kScreenHeight;
	}
}

void Camera::SetTarget(const Position2& pos)
{
	m_mode = Mode::Fixed;
	m_fixedTarget = pos;
}

void Camera::SetTargetProvider(std::function<Position2()> provider)
{
	m_mode = Mode::Provider;
	m_targetProvider = std::move(provider); // 関数オブジェクトの内容の所有権を受け取る
}

Rect2D Camera::GetNowScreenArea() const
{
	return { scroll.x,scroll.x + Game::kScreenWidth,scroll.y, scroll.y + Game::kScreenHeight };
}

bool Camera::IsLerpEnd()
{
	return Geometry::GetDistance(m_pos, GetCurrentTarget()) <= kLerpEndDistance;
}

Position2 Camera::GetCurrentTarget()
{
	switch (m_mode)
	{
	case Mode::Fixed:
		return m_fixedTarget;
	case Mode::Provider:
		return m_targetProvider();
	default:
		break;
	}

	// ターゲットが何もない場合は今の座標を返す
	return m_pos;
}
