#include "Actor.h"

Actor::Actor(ObjectTag tag, MyLib::ColliderBase::ColliderType type) :
	Collidable(tag, type)
{

}

Actor::~Actor()
{

}