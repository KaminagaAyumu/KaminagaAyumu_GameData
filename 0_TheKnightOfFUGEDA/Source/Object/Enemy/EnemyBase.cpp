#include "EnemyBase.h"
#include "../../MyLib/Component/Transform.h"
#include "../../MyLib/Component/Controller/Enemy/EnemyController.h"
#include <cassert>

EnemyBase::EnemyBase() : 
	GameObject(MyLib::GameObject::Type::Enemy)
{
}

EnemyBase::~EnemyBase()
{
}