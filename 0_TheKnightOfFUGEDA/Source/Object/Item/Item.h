#pragma once
#include "../../MyLib/GameObject.h"

class Item : public MyLib::GameObject
{
public:

	Item();
	virtual ~Item();

	void Init() override;
	void Update() override;
	void End() override;

	void SetPos(const Vector3& pos);

	bool IsDestroy();
};

