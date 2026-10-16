#include <memory>
#include "Charge.h"
#include "../../Utility/Geometry.h"
#include "../../Utility/Camera.h"

namespace
{
	constexpr int kGraphWidth = 48;
	constexpr int kGraphHeight = 48;

	constexpr int kAnimNum = 12;
	constexpr int kAnimFrame = 4;
}

Charge::Charge(int handle, const Position2& pos)
{
	m_handle = handle;

	m_pos = pos;

	m_animation.SetAnimation(handle, { kGraphWidth,kGraphHeight }, kAnimNum, kAnimFrame, false);
}

Charge::~Charge()
{
}

void Charge::Update()
{
	m_animation.Update();
	if (m_animation.IsEnd())
	{
		m_isAlive = true;
	}
}

void Charge::Draw(const std::weak_ptr<Camera> camera)
{
	auto pCamera = camera.lock();
	int drawX = static_cast<int>(m_pos.x - pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - pCamera->scroll.y);

	m_animation.Draw({ drawX, drawY }, false);
}
