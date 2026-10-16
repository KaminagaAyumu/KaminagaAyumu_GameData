#include "Coin.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kCoinDefaultWidth = 30.0f;
	constexpr float kCoinDefaultHeight = 30.0f;

	constexpr int kGraphWidth = 16; // コインの画像1枚の幅
	constexpr int kGraphHeight = 16; // コインの画像1枚の高さ

	constexpr int kAnimNum = 5; // コインの画像のアニメーションの枚数
	constexpr int kAnimFrame = 5; // アニメーションの更新フレーム

	constexpr float kCoinScale = 1.0f;
}

Coin::Coin(const Position2& pos, int handle)
{
	m_pos = pos;
	m_colRect = { {m_pos},kCoinDefaultWidth,kCoinDefaultHeight };
	m_colCircle = { {m_pos},kCoinDefaultWidth / 2 };
	m_graphHandle = handle;
	m_currentAnim.SetAnimation(handle, Size{ kGraphWidth, kGraphHeight }, kAnimNum, kAnimFrame, true);
}

void Coin::Init()
{
	
}

void Coin::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
	m_currentAnim.Update();
}

void Coin::Draw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	if (m_isExist)
	{
		//DrawRotaGraph(drawX, drawY, kCoinScale, 0.0, m_graphHandle, true);
		m_currentAnim.Draw({ drawX, drawY }, false);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void Coin::OnCollected(std::weak_ptr<GameManager> gameManager)
{
	auto manager = gameManager.lock();
	// コインが取得された際の処理を行わせる
	manager->OnItemCollected(Types::ItemType::Coin, m_pos);
	manager->NotifyItemEvent(Types::ItemType::Coin);
}

void Coin::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}