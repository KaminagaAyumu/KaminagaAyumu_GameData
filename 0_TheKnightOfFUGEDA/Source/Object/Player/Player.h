#pragma once
#include "../../MyLib/GameObject.h"
#include "../../MyLib/Component/Transform.h"

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public MyLib::GameObject
{
public:

	Player();
	virtual ~Player();

	void Init();
	void Update();
	void End();

	const Vector3& GetPos();

	int GetLife();

	int GetMaxLife();

	void SetCanAct(bool canAct);

	void SetLockOnTarget(std::weak_ptr<MyLib::Transform> target);
	void ClearLockOn();

	bool IsDied();

};

