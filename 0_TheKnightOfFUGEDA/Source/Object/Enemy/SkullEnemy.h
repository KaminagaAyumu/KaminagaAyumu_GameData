#pragma once
#include "EnemyBase.h"
#include "../../MyLib/GameObject.h"

class SkullEnemy : public EnemyBase
{
public:

	SkullEnemy();
	virtual ~SkullEnemy();

	void Init() override;
	void Update() override;
	void End() override;

	void SetPos(const Vector3& pos) override;

	void SetPlayer(std::weak_ptr<MyLib::GameObject> player) override;

	void SetCanAct(bool canAct) override;


private:

	std::weak_ptr<MyLib::GameObject> m_pPlayer;	// プレイヤーの位置
};

