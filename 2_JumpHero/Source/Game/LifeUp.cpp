#include "LifeUp.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kLifeUpDefaultWidth = 40.0f;
	constexpr float kLifeUpDefaultHeight = 40.0f;
}

LifeUp::LifeUp(const Position2& pos, int handle)
{
	m_pos = pos;
	m_colRect = { {m_pos},kLifeUpDefaultWidth,kLifeUpDefaultHeight };
	m_colCircle = { {m_pos},kLifeUpDefaultWidth / 2 };
	m_graphHandle = handle;
}

void LifeUp::Init()
{

}

void LifeUp::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void LifeUp::Draw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	if (m_isExist)
	{
		DrawCircle(drawX, drawY, static_cast<int>(m_colCircle.radius), 0xddffff, true);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void LifeUp::OnCollected(std::weak_ptr<GameManager> gameManager)
{
	auto manager = gameManager.lock();
	// 1UPが取得された際の処理を行わせる
	manager->OnItemCollected(Types::ItemType::LifeUp, m_pos);
}

void LifeUp::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}