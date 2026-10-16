#include "EnemyResource.h"
#include "EnemyData.h"

namespace
{
	// CSVの列番号
	constexpr int kTypeColumn = 0;	// 敵の種類
	constexpr int kPosXColumn = 1;	// X座標
	constexpr int kPosYColumn = 2;	// Y座標
	constexpr int kPosZColumn = 3;	// Z座標
}

void EnemyResource::ConvertEnemyData()
{
	// データの数を取得する
	size_t size = GetDataCount();

	for(size_t i = 0; i < size; ++i)
	{
		// データを読み込む
		int type = Read<int>(static_cast<int>(i), kTypeColumn);
		float posX = Read<float>(static_cast<int>(i), kPosXColumn);
		float posY = Read<float>(static_cast<int>(i), kPosYColumn);
		float posZ = Read<float>(static_cast<int>(i), kPosZColumn);
		// 敵データを作成
		std::shared_ptr<EnemyData> data = std::make_shared<EnemyData>();
		data->SetData(static_cast<EnemyManager::EnemyType>(type), Vector3{ posX, posY, posZ });
		m_enemyDatas.push_back(data);
	}

	// データを開放する
	ReleaseRawData();
}
