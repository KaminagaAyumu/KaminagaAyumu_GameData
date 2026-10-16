#pragma once
#include "../MyLib/GameObject.h"
#include "../MyLib/Component/Transform.h"

class Wall : public MyLib::GameObject
{
public:

	Wall();
	virtual ~Wall();

	void Init(int stageNo);
	void Update();
	void End();

private:

	std::shared_ptr<MyLib::Transform> m_pModelOffset;
};

