#pragma once
#include "../../MyLib/GameObject.h"

class EnemyBase : public MyLib::GameObject
{
public:

	EnemyBase();
	virtual ~EnemyBase();

	virtual void SetPos(const Vector3& pos) abstract;

	virtual void SetPlayer(std::weak_ptr<MyLib::GameObject> player) abstract;

	virtual void SetCanAct(bool canAct) abstract;

protected:

	std::weak_ptr<MyLib::GameObject> m_pPlayer;	// プレイヤーの位置

};

