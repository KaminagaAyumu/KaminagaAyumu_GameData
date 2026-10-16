#pragma once
#include "../MyLib/GameObject.h"
#include "../MyLib/Component/Transform.h"

class Ground : public MyLib::GameObject
{
public:

	Ground();
	virtual ~Ground();

	void Init(int stageNo);
	void Update();
	void End();

private:

	std::shared_ptr<MyLib::Transform> m_pModelOffset;



};

