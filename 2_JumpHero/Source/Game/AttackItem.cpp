#include "AttackItem.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kAttackItemDefaultWidth = 30.0f;
	constexpr float kAttackItemDefaultHeight = 30.0f;
	constexpr float kAttackItemScale = 1.0f;
}

AttackItem::AttackItem(const Position2& pos, int handle)
{
	m_pos = pos;
	m_colRect = { {m_pos},kAttackItemDefaultWidth,kAttackItemDefaultHeight };
	m_colCircle = { {m_pos},kAttackItemDefaultWidth / 2 };
	m_graphHandle = handle;
}

void AttackItem::Init()
{
}

void AttackItem::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void AttackItem::Draw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	if (m_isExist)
	{
		DrawRotaGraph(drawX, drawY, kAttackItemScale, 0.0f, m_graphHandle, true);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void AttackItem::OnCollected(std::weak_ptr<GameManager> gameManager)
{
	auto manager = gameManager.lock();
	// 攻撃アイテムが取得された際の処理を行わせる
	manager->OnItemCollected(Types::ItemType::AttackItem, m_pos);
	manager->NotifyItemEvent(Types::ItemType::AttackItem);
}

void AttackItem::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}
