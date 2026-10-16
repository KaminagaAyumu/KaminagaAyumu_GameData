#include <memory>
#include "Chest.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "Player.h"
#include "ItemManager.h"
#include "DxLib.h"


namespace
{
	constexpr float kDefaultWidth = 45.0f;
	constexpr float kDefaultHeight = 45.0f;
	constexpr int	kChipSize = 32; // マップ1つの大きさ
	constexpr float kChipScale = 1.40625f; // マップの拡大率

	constexpr int kChestMapChipNo = 1; // マップチップの当たり判定のみを行うチップ番号
}

Chest::Chest() :
	Actor(Types::ActorType::Chest),
	m_graphHandle(-1),
	m_state(ChestState::Normal),
	m_chipPos{}
{
}

Chest::Chest(int x, int y, std::weak_ptr<Map> map, int handle, bool isHidden) :
	Actor(Types::ActorType::Chest),
	m_pMap(map),
	m_chipPos{x,y}
{
	auto pMap = m_pMap.lock();
	// マップチップ一つのサイズを取得
	float tileSize = pMap->GetTileSize();
	// 座標をマップ座標から変換
	// 座標基準を左上から中心にする
	m_pos = { static_cast<float>(m_chipPos.x) * tileSize + tileSize * 0.5f,static_cast<float>(m_chipPos.y) * tileSize + tileSize * 0.5f };
	m_colRect = { m_pos,kDefaultWidth,kDefaultHeight };
	m_graphHandle = handle;
	// 引数で取得した条件によって初期状態を変える
	if(isHidden) // trueの場合
	{
		m_state = ChestState::Hidden; // 隠し宝箱
	}
	else
	{
		m_state = ChestState::Normal; // 通常の宝箱(最初から見えている)
	}
}

Chest::~Chest()
{
}

void Chest::Init()
{
}

void Chest::Update(Input&)
{
	m_colRect.pos = m_pos;
}

void Chest::Draw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	if (!(m_state == ChestState::Normal))
	{
		//DrawGraph(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), m_openGraphHandle, TRUE);
		//DrawString(drawX, drawY, L"アイテム", 0xffffff);
	}
	else
	{
		DrawRotaGraph(drawX, drawY, kChipScale, 0.0f, m_graphHandle, true);
#ifdef _DEBUG
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void Chest::IsCollision(const Types::CollisionInfo& info)
{
	// プレイヤーと衝突している時
	if(info.otherType == Types::ActorType::Player)
	{
#ifdef _DEBUG
		printfDx(L"Chest : プレイヤーと衝突しました\n");
#endif

	}
}

void Chest::AppearChest()
{
	// 通常の状態に変更
	m_state = ChestState::Normal;

	// マップを使えるようにlockしておく
	auto pMap = m_pMap.lock();

	// マップのデータを通常の状態と同じ番号にする
	pMap->SetMapChip(m_chipPos.x, m_chipPos.y, kChestMapChipNo);
}

void Chest::OpenChest()
{
	// 開いた状態に変更
	m_state = ChestState::Opened;

	// マップを使えるようにlockしておく
	auto pMap = m_pMap.lock();

	// マップのデータから宝箱のデータを消去する
	pMap->ResetChestData(m_chipPos.x, m_chipPos.y);
}
