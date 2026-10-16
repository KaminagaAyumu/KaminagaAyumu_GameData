#include "Collision.h"
#include "DxLib.h"

Collision::Collision():
	m_left(0.0f),
	m_top(0.0f),
	m_right(0.0f),
	m_bottom(0.0f),
	m_radius(0.0f)
{
}

Collision::~Collision()
{
}

void Collision::Init()
{

}

void Collision::End()
{

}
void Collision::Update()
{

}
void Collision::DrawBoxCollider(unsigned int color)
{
	DrawBox(static_cast<int>(m_left), static_cast<int>(m_top), static_cast<int>(m_right), static_cast<int>(m_bottom), color, false);
}

void Collision::DrawCircleCollider(unsigned int color)
{
	DrawCircle(static_cast<int>(m_left), static_cast<int>(m_top), static_cast<int>(m_radius), color, false);
}

void Collision::SetCenter(float x, float y, float width, float height)
{
	m_left = x - width / 2;
	m_top = y - height / 2;
	m_right = x + width / 2;
	m_bottom = y + height / 2;
}

void Collision::SetRect(float x, float y, float width, float height)
{
	m_left = x;
	m_top = y;
	m_right = x + width;
	m_bottom = y + height;
}

void Collision::SetCircle(float x, float y, float radius)
{
	m_left = x;
	m_top = y;
	m_radius = radius;
}

Vector2 Collision::GetCenter() const
{
	float x = (m_left + m_right) / 2;
	float y = (m_top + m_bottom) / 2;
	return Vector2(x, y);
}

float Collision::GetWidth() const
{
	return m_right - m_left;
}

float Collision::GetHeight() const
{
	return m_bottom - m_top;
}

bool Collision::IsCollisionRectToRect(const Collision& rect)
{
	return false;
}

bool Collision::IsCollisionLineToCircle(const Collision& circle)
{
//	Vector2 circlePos = circle.GetCenter();

//	Vector2 topCircleVec = circlePos - GetLeftTop();

	return false;
}

bool Collision::IsCollisionRectToCircle(const Collision& circle)
{
	float circleX = circle.m_left;
	float circleY = circle.m_top;
	float circleRadius = circle.m_radius;

	float rectX;
	float rectY;

	bool collision;

	if (circleX > m_right)
	{
		rectX = m_right;
	}
	else if (circleX < m_left)
	{
		rectX = m_left;
	}
	else
	{
		rectX = circleX;
	}

	if (circleY > m_top)
	{
		rectY = m_top;
	}
	else if (circleX < m_bottom)
	{
		rectY = m_bottom;
	}
	else
	{
		rectY = circleX;
	}

	float dstX = circleX - rectX;
	float dstY = circleY - rectY;

	if ((dstX * dstX) + (dstY * dstY) <= (circleRadius * circleRadius))
	{
		collision = true;
	}
	else
	{
		collision = false;
	}
	return collision;
	//return (dstX * dstX) + (dstY * dstY) <= (circleRadius * circleRadius);
}


