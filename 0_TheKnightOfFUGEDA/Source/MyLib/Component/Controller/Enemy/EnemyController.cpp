#include "EnemyController.h"
#include "../../../State/Enemy/EnemyStateFreeze.h"
#include "../../../State/Enemy/EnemyStateSearch.h"
#include "../../../GameObject.h"
#include "../../Transform.h"
#include <cassert>

MyLib::EnemyController::EnemyController() : 
	m_isDetectNotified(false)
{
}

MyLib::EnemyController::~EnemyController()
{
}

void MyLib::EnemyController::SetPlayer(std::weak_ptr<MyLib::GameObject> player)
{
	if (std::shared_ptr<MyLib::GameObject> pPlayer = player.lock())
	{
		m_pPlayer = player;
	}
	else
	{
		assert(false && "EnemyController : プレイヤーオブジェクトのセットに失敗しました");
	}
}

std::weak_ptr<MyLib::Transform> MyLib::EnemyController::GetPlayerPos() const
{
	if (std::shared_ptr<MyLib::GameObject> pPlayer = m_pPlayer.lock())
	{
		return pPlayer->GetComponent<MyLib::Transform>();
	}
	else
	{
		assert(false && "EnemyController : プレイヤーオブジェクトの取得に失敗しました");
		return std::weak_ptr<MyLib::Transform>{};
	}
}

void MyLib::EnemyController::SetCanAct(bool canAct)
{
	if (canAct)
	{
		m_stateMachine.ChangeState<MyLib::EnemyStateSearch>();
	}
	else
	{
		m_stateMachine.ChangeState<MyLib::EnemyStateFreeze>();
	}
}

bool MyLib::EnemyController::TryNotifyDetect()
{
	if (m_isDetectNotified) return false;
	m_isDetectNotified = true;
	return true;
}

bool MyLib::EnemyController::ResetDetectNotify()
{
	if (!m_isDetectNotified)return false;
	m_isDetectNotified = false;
	return true;
}
