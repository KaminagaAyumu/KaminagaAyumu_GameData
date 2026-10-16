#include "EnemyData.h"

EnemyData::EnemyData() : 
	m_pos{},
	m_type(EnemyManager::EnemyType::BulletEnemy)
{
}

void EnemyData::SetData(EnemyManager::EnemyType type, const Vector3& pos)
{
	m_type = type;
	m_pos = pos;
}
