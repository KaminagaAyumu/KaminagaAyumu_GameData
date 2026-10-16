#include "Balloon.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kBalloonDefaultWidth = 30.0f;
	constexpr float kBalloonDefaultHeight = 30.0f;

	constexpr int kGraphWidth = 32;
	constexpr int kGraphHeight = 34;

	constexpr int kAnimNum = 3;
	constexpr int kAnimFrame = 8;

	constexpr float kBalloonScale = 1.0f;
}

Balloon::Balloon(const Position2& pos, int handle)
{
	m_pos = pos;
	m_colRect = { {m_pos},kBalloonDefaultWidth,kBalloonDefaultHeight };
	m_colCircle = { {m_pos},kBalloonDefaultWidth / 2 };
	m_graphHandle = handle;

	m_currentAnim.SetAnimation(handle, Size{ kGraphWidth,kGraphHeight }, kAnimNum, kAnimFrame, true);
	//m_currentAnim.SetScale(kBalloonScale);
}

void Balloon::Init()
{

}

void Balloon::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
	m_currentAnim.Update();
}

void Balloon::Draw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	if (m_isExist)
	{
		m_currentAnim.Draw({ drawX,drawY }, false);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void Balloon::OnCollected(std::weak_ptr<GameManager> gameManager)
{
	auto manager = gameManager.lock();
	// 風船が取得された際の処理を行わせる
	manager->OnItemCollected(Types::ItemType::Balloon, m_pos);
	manager->NotifyItemEvent(Types::ItemType::Balloon);
}

void Balloon::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}