#pragma once
#include "CSVResource.h"
#include <memory>
#include <vector>

class ItemData;

class ItemResource : public CSVResource
{
public:

	ItemResource() = default;
	virtual ~ItemResource() = default;

	void ConvertItemData();

	std::vector<std::shared_ptr<ItemData>>& GetItemDatas() { return m_itemDatas; }

private:

	// アイテムのデータ
	std::vector<std::shared_ptr<ItemData>> m_itemDatas;

};

