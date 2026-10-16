#include "EnemyBase.h"
#include "Player.h"
#include "../Utility/Camera.h"
#include "../Utility/Map.h"
#include "../Utility/Game.h"
#include "DxLib.h"
#include <cmath>

EnemyBase::EnemyBase(Player* player, std::weak_ptr<Map> map) :
	Actor(Types::ActorType::Enemy),
	m_pPlayer(player),
	m_pMap(map),
	m_direction{},
	m_isDead(false)
{
}

EnemyBase::~EnemyBase()
{
}

bool EnemyBase::IsOnScreen() const
{
	auto camera = m_pCamera.lock();
	Rect2D screenRange = camera->GetNowScreenArea();
	// 矩形の範囲内に居たらtrue、それ以外はfalse
	return m_pos.x >= screenRange.GetLeft() && m_pos.x <= screenRange.GetRight() && m_pos.y >= screenRange.GetTop() && m_pos.y <= screenRange.GetBottom();
}
