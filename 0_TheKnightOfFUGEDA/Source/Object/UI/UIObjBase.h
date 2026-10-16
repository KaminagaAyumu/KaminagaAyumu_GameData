#pragma once
#include "../../MyLib/GameObject.h"

class UIObjBase : public MyLib::GameObject
{
public:

	UIObjBase();
	virtual ~UIObjBase();

	virtual void Init();
	virtual void Update();
	virtual void End();
};

