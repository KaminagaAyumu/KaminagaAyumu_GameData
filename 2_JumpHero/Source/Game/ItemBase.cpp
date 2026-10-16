#include "ItemBase.h"
#include "DxLib.h"

ItemBase::ItemBase() :
	Actor(Types::ActorType::Item),
	m_graphHandle(-1),
	m_isExist(true)
{
}

ItemBase::~ItemBase()
{
}
