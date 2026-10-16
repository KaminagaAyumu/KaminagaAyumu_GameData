#pragma once
#include "EnemyBase.h"
#include "../../MyLib/GameObject.h"

class BulletEnemy : public EnemyBase
{
public:

	BulletEnemy();
	virtual ~BulletEnemy();

	void Init() override;
	void Update() override;
	void End() override;

	void SetPos(const Vector3& pos) override;

	void SetPlayer(std::weak_ptr<MyLib::GameObject> player) override;

	void SetCanAct(bool canAct) override;

private:


};

