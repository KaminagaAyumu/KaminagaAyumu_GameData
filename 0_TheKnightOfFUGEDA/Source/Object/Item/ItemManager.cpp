#include "ItemManager.h"
#include "Item.h"
#include "../../MyLib/ObjectFactory.h"
#include "../../Utility/CSV/ItemData.h"

namespace
{
	// ファイルを読み込む際のパスの最大サイズ(文字数)
	constexpr size_t kFilePathMax = 256;
}

ItemManager::ItemManager() : 
	m_acquisitionCountThisFrame(0)
{
}

ItemManager::~ItemManager()
{
}

void ItemManager::Init(int stageNo)
{
	wchar_t filePath[kFilePathMax];
	std::swprintf(filePath, kFilePathMax, L"Data/File/CSV/Item/item_resource_%d.csv", stageNo);

	m_itemResource.Load(filePath);
	m_itemResource.ConvertItemData();

	// 初期化、座標セットを行う
	for (auto spawn : m_itemResource.GetItemDatas())
	{
		std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(MyLib::ObjectFactory::CreateItem());
		item->SetPos(spawn->GetPos());
		item->Init();
		m_pItems.push_back(item);
	}
}

void ItemManager::Update()
{
	m_acquisitionCountThisFrame = 0;

	for (auto& item : m_pItems)
	{
		if (item.lock()->IsDestroy())
		{
			m_acquisitionCountThisFrame++;
		}
	}
	// アイテムの中で削除されているものを探す
	m_pItems.remove_if([](std::weak_ptr<Item> item)
		{
			// 削除されている際にリストから消去する
			return item.lock()->IsDestroy();
		});
}

void ItemManager::End()
{

}

bool ItemManager::IsItemGetAll() const
{
	return m_pItems.empty();
}
