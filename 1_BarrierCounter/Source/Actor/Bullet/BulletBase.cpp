#include "BulletBase.h"

BulletBase::BulletBase(ObjectTag tag, MyLib::ColliderBase::ColliderType type) : 
	Collidable(tag, type),
	m_isDead(false)
{
}

BulletBase::~BulletBase()
{
}
