#pragma once
#include "../../Geometry/Vector3.h"

/// <summary>
/// アイテムのデータ
/// </summary>
class ItemData
{
public:

	ItemData();
	virtual ~ItemData() = default;

	void SetData(const Vector3& pos);

	const Vector3& GetPos() const { return m_pos; }

private:

	Vector3 m_pos;	// 配置座標

};

