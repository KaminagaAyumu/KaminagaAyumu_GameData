#pragma once
#include "CSVResource.h"
#include <memory>
#include <vector>

class EnemyData;

/// <summary>
/// 敵の配置などのデータを管理するCSVデータクラス
/// </summary>
class EnemyResource : public CSVResource
{
public:

	EnemyResource() = default;
	virtual ~EnemyResource() = default;

	void ConvertEnemyData();

	std::vector<std::shared_ptr<EnemyData>>& GetEnemyDatas() { return m_enemyDatas; }

private:

	// 敵のデータ
	std::vector<std::shared_ptr<EnemyData>> m_enemyDatas;

};

