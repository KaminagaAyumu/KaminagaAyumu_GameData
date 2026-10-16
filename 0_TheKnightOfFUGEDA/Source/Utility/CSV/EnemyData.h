#pragma once
#include "../../Object/Enemy/EnemyManager.h"

class EnemyData
{
public:

	EnemyData();
	virtual ~EnemyData() = default;

	void SetData(EnemyManager::EnemyType type, const Vector3& pos);

	EnemyManager::EnemyType GetType() const { return m_type; }
	const Vector3& GetPos() const { return m_pos; }

private:

	EnemyManager::EnemyType m_type;		// 敵のタイプ
	Vector3 m_pos;						// 配置座標
};

