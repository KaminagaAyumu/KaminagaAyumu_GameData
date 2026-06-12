#include "EnemyBase.h"

EnemyBase::EnemyBase(ObjectTag tag, MyLib::ColliderBase::ColliderType type) :
	Actor(tag, type),
	m_state(EnemyState::Alive)
{
}

EnemyBase::~EnemyBase()
{
}
