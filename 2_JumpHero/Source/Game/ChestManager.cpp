#include "ChestManager.h"
#include "Chest.h"
#include "../Utility/Camera.h"
#include "../Utility/Input.h"
#include "../Utility/Map.h"
#include "Actor.h"
#include "GameManager.h"
#include "Player.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "DxLib.h"

namespace
{
	constexpr int kChestChipNo = 106;
	constexpr int kHiddenChestChipNo = 107;

	constexpr float kChestOpenSEVolume = 20.0f; // 宝箱を開けたときのSEのボリューム
}


ChestManager::ChestManager(std::weak_ptr<Camera> camera, GameManager* gameManager) :
	m_pCamera(camera),
	m_pGameManager(gameManager)
{
	m_chestGraphHandle = LoadGraph(L"data/chest.png");
	Application::GetInstance().GetSoundManager()->LoadSoundClip("chestOpenSE", L"data/sound/SE/openChest.mp3", SoundBus::SE, kChestOpenSEVolume, false);
}

ChestManager::~ChestManager()
{
	DeleteGraph(m_chestGraphHandle);
}

void ChestManager::Init(std::weak_ptr<Camera> camera)
{
	m_chests.clear();
	m_pCamera = camera;
}

void ChestManager::Update(Input& input)
{
	for (auto& chest : m_chests)
	{
		chest->Update(input);
		if (chest->IsOpen())
		{
			Application::GetInstance().GetSoundManager()->Play("chestOpenSE", 1.0f, true);
			//m_pGameManager->DropItem(static_cast<int>(chest->GetPos().x), static_cast<int>(chest->GetPos().y));
		}
	}

	// 宝箱に存在しないものがある場合削除
	m_chests.remove_if(
		[](std::shared_ptr<Chest> chest)
		{
			return chest->IsOpen();
		}
	);
}

void ChestManager::Draw()
{
	/*for (auto& chest : m_chests)
	{
		chest->Draw();
	}*/
}

void ChestManager::PushActors(std::vector<std::weak_ptr<Actor>>& actors)
{
	for (auto& chest : m_chests)
	{
		actors.push_back(chest);
	}
}

size_t ChestManager::GetChestNum()
{
	return m_chests.size();
}

void ChestManager::SpawnChest(std::weak_ptr<Map> map)
{
	auto pMap = map.lock(); // マップを見るようにする
	for (int x = 0; x < pMap->GetMapWidth(); x++)
	{
		for (int y = 0; y < pMap->GetMapHeight(); y++)
		{
			if (pMap->GetPositioningData(x, y) == kChestChipNo)
			{
				auto chest = std::make_shared<Chest>(x, y, pMap,m_chestGraphHandle,false);
				chest->SetCamera(m_pCamera);
				m_chests.push_back(chest);
				m_chestMap.emplace(TilePos{ x,y }, chest.get());
			}
			else if (pMap->GetPositioningData(x, y) == kHiddenChestChipNo)
			{
				auto chest = std::make_shared<Chest>(x, y, pMap, m_chestGraphHandle, true);
				chest->SetCamera(m_pCamera);
				m_chests.push_back(chest);
				m_chestMap.emplace(TilePos{ x,y }, chest.get());
			}
		}
	}
}

void ChestManager::OpenChestAtPosition(int tileX, int tileY)
{
	if(m_chests.empty())
	{
		return;
	}
	TilePos pos{ tileX, tileY };
	auto it = m_chestMap.find(pos);
	if (it != m_chestMap.end())
	{
		// 隠し宝箱だった場合出現するのみ
		if(it->second->IsHidden())
		{
			it->second->AppearChest();
			return;
		}
		it->second->OpenChest();
	}
}
