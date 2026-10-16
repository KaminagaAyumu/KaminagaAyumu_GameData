#include <memory>
#include <vector>
#include "ItemManager.h"
#include "ItemBase.h"
#include "Coin.h"
#include "Balloon.h"
#include "UpgradeMedal.h"
#include "LifeUp.h"
#include "ChangeToCoin.h"
#include "AttackItem.h"
#include "../Utility/Map.h"
#include "../Utility/Input.h"
#include <cassert>
#include "DxLib.h"

namespace
{
	constexpr int kBallonChipNo = 33; // 風船のマップチップ番号

	// 画像を識別するためのenum
	enum
	{
		kGraphBalloon, // 風船
		kGraphUpgradeMedal, // 強化メダル
		kGraphLifeUp, // 1UP
		kGraphCoin, // スコア加算用コイン
		kGraphChangeToCoin, // 敵をコインに変える
		kGraphAttackItem, // 攻撃アイテム

		kGraphNum // 画像の数
	};

	// 画像ファイル名の配列
	const std::wstring kGraphFileName[] =
	{
		L"data/img/balloon_red.png", // 風船
		L"data/img/upgrade_medal.png", // 強化メダル
		L"data/img/life_up.png", // 1UP
		L"data/img/coin.png", // スコア加算用コイン
		L"data/img/change_to_coin.png", // 敵をコインに変える
		L"data/img/attack_item.png", // 攻撃アイテム
	};

	// 画像枚数が違うときにエラーを出せるようにする
	static_assert(kGraphNum == _countof(kGraphFileName), "画像枚数の定義が間違っています");
}

ItemManager::ItemManager(std::weak_ptr<Camera> camera, std::weak_ptr<GameManager> gameManager) :
	m_pCamera(camera),
	m_pGameManager(gameManager),
	m_firstBalloonNum(0)
{
	m_pItems.clear();
	for(int i = 0; i < kGraphNum; i++)
	{
		int graphHandle = LoadGraph(kGraphFileName[i].c_str());
		assert(graphHandle != -1 && "画像の読み込みに失敗しました");
		m_graphHandles.push_back(graphHandle);
	}
}

ItemManager::~ItemManager()
{
	for(auto& handle : m_graphHandles)
	{
		DeleteGraph(handle);
	}
}

void ItemManager::Init(std::weak_ptr<Camera> camera)
{
	m_pItems.clear();
	m_firstBalloonNum = 0;
	m_pCamera = camera;
}

void ItemManager::Update(Input& input)
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		item->Update(input);
		if (!item->IsExist())
		{
			item->OnCollected(m_pGameManager);
		}
	}

	// アイテムに存在しないものがある場合削除
	m_pItems.remove_if(
		[](std::shared_ptr<ItemBase> item )
		{ 
			return !item->IsExist();
		}
	);
}

void ItemManager::Draw()
{
	/*if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		item->Draw();
	}*/
}

void ItemManager::SpawnItem(const Position2& pos, Types::ItemType itemType)
{

	// アイテムの種類によって生成するクラスを変える
	switch (itemType)
	{
	case Types::ItemType::Balloon: // 風船
	{
		auto item = std::make_shared<Balloon>(pos, m_graphHandles[kGraphBalloon]);
		item->SetCamera(m_pCamera);
		m_pItems.push_back(item);
	}
		break;
	case Types::ItemType::UpgradeMedal: // 強化メダル
	{
		auto item = std::make_shared<UpgradeMedal>(pos, m_graphHandles[kGraphUpgradeMedal]);
		item->SetCamera(m_pCamera);
		m_pItems.push_back(item);
	}
		break;
	case Types::ItemType::LifeUp: // 1UP
	{
		auto item = std::make_shared<LifeUp>(pos, m_graphHandles[kGraphLifeUp]);
		item->SetCamera(m_pCamera);
		m_pItems.push_back(item);
	}
		break;
	case Types::ItemType::Coin: // スコア加算用コイン
	{
		auto item = std::make_shared<Coin>(pos, m_graphHandles[kGraphCoin]);
		item->SetCamera(m_pCamera);
		m_pItems.push_back(item);
	}
		break;
	case Types::ItemType::ChangeToCoin: // 敵をコインに変える
	{
		auto item = std::make_shared<ChangeToCoin>(pos, m_graphHandles[kGraphChangeToCoin]);
		item->SetCamera(m_pCamera);
		m_pItems.push_back(item);
	}
		break;
	case Types::ItemType::AttackItem: // 攻撃アイテム
	{
		auto item = std::make_shared<AttackItem>(pos, m_graphHandles[kGraphChangeToCoin]);
		item->SetCamera(m_pCamera);
		m_pItems.push_back(item);
	}
	break;
	default:
		break;
	}

	
}

void ItemManager::SpawnFloatingChangeToCoin(const Position2& pos, std::weak_ptr<Map> map)
{
	auto item = std::make_shared<ChangeToCoin>(pos, m_graphHandles[kGraphChangeToCoin]);
	item->SetCamera(m_pCamera);
	item->SetFloating(map);
	m_pItems.push_back(item);
}

void ItemManager::FirstSpawnItem(std::weak_ptr<Map> map)
{
	auto pMap = map.lock();
	for (int y = 0; y < pMap->GetMapHeight(); y++)
	{
		for (int x = 0; x < pMap->GetMapWidth(); x++)
		{
			if (pMap->GetPositioningData(x, y) == kBallonChipNo)
			{
				// マップチップの1マスのサイズを取得(拡大を含む)
				float tileSize = pMap->GetTileSize();
				// 風船のスポーン位置はマップの中心にする
				Position2 pos = { x * tileSize + tileSize * 0.5f,y * tileSize + tileSize * 0.5f };
				// 風船生成
				auto balloon = std::make_shared<Balloon>(pos,m_graphHandles[kGraphBalloon]);
				balloon->SetCamera(m_pCamera); // カメラセット
				m_pItems.push_back(balloon); // アイテムリストに追加
				m_firstBalloonNum++; // 風船の数をカウント
			}
		}
	}
}

void ItemManager::PushActors(std::vector<std::weak_ptr<Actor>>& actors)
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		actors.push_back(item);
	}
}

size_t ItemManager::GetItemNum()
{
	if (m_pItems.empty())
	{
		return 0;
	}
	return m_pItems.size();
}
