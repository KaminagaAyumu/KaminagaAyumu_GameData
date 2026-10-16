#include "UpgradeMedal.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "../Utility/GameType.h"
#include "DxLib.h"

namespace
{
	constexpr float kMedalDefaultWidth = 40.0f;
	constexpr float kMedalDefaultHeight = 40.0f;

	constexpr int kGraphWidth = 16; // メダルの画像1枚の幅
	constexpr int kGraphHeight = 16; // メダルの画像1枚の高さ

	constexpr int kAnimNum = 4; // メダルの画像のアニメーションの枚数
	constexpr int kAnimFrame = 5; // アニメーションの更新フレーム

	constexpr float kMedalScale = 1.40625f;
}

UpgradeMedal::UpgradeMedal(const Position2& pos, int handle)
{
	m_pos = pos;
	m_colRect = { {m_pos},kMedalDefaultWidth,kMedalDefaultHeight };
	m_colCircle = { {m_pos},kMedalDefaultWidth / 2 };
	m_graphHandle = handle;
	m_currentAnim.SetAnimation(handle, Size{ kGraphWidth, kGraphHeight }, kAnimNum, kAnimFrame, true);
	m_currentAnim.SetScale(kMedalScale);
}

void UpgradeMedal::Init()
{

}

void UpgradeMedal::Update(Input&)
{
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
	m_currentAnim.Update();
}

void UpgradeMedal::Draw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	if (m_isExist)
	{
		//DrawRotaGraph(drawX, drawY, kMedalScale, 0.0, m_graphHandle, TRUE);
		m_currentAnim.Draw({ drawX,drawY }, false);
#ifdef _DEBUG
		m_colCircle.Draw(drawX, drawY);
		m_colRect.Draw(drawX, drawY);
#endif
	}
}

void UpgradeMedal::OnCollected(std::weak_ptr<GameManager> gameManager)
{
	auto manager = gameManager.lock();
	// 風船が取得された際の処理を行わせる
	manager->OnItemCollected(Types::ItemType::UpgradeMedal, m_pos);
	manager->NotifyItemEvent(Types::ItemType::UpgradeMedal);
}

void UpgradeMedal::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player)
	{
		m_isExist = false;
	}
}