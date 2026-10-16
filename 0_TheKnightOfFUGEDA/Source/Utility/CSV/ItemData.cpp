#include "ItemData.h"

ItemData::ItemData() : 
	m_pos{}
{
}

void ItemData::SetData(const Vector3& pos)
{
	m_pos = pos;
}
