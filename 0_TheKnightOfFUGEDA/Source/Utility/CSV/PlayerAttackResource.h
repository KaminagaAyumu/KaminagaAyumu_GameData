#pragma once
#include "CSVResource.h"
#include "AttackData.h"
#include <memory>
#include <vector>
#include <unordered_map>

class AttackData;

class PlayerAttackResource : public CSVResource
{
public:

	PlayerAttackResource();
	virtual ~PlayerAttackResource();

	void ConvertAttackData();

	/// <summary>
	/// 攻撃のデータを取得する
	/// </summary>
	/// <param name="id">攻撃のID</param>
	/// <returns></returns>
	AttackData* GetAttackData(AttackData::AttackID id);

private:
	// 攻撃のデータ
	std::unordered_map<AttackData::AttackID, std::shared_ptr<AttackData>> m_pAttackDatas;
};

