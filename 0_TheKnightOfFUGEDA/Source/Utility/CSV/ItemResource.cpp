#include "ItemResource.h"
#include "ItemData.h"

namespace
{
	// CSVの列番号
	constexpr int kPosXColumn = 0;	// X座標
	constexpr int kPosYColumn = 1;	// Y座標
	constexpr int kPosZColumn = 2;	// Z座標
}

void ItemResource::ConvertItemData()
{
	// データの数を取得する
	size_t size = GetDataCount();

	for (size_t i = 0; i < size; ++i)
	{
		// データを読み込む
		float posX = Read<float>(static_cast<int>(i), kPosXColumn);
		float posY = Read<float>(static_cast<int>(i), kPosYColumn);
		float posZ = Read<float>(static_cast<int>(i), kPosZColumn);
		// アイテムデータを作成
		std::shared_ptr<ItemData> data = std::make_shared<ItemData>();
		data->SetData(Vector3{ posX,posY,posZ });
		m_itemDatas.push_back(data);
	}

	// データを開放する
	ReleaseRawData();
}
