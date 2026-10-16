#pragma once
#include "../MyLib/GameObject.h"

class Ground;
class Wall;

/// <summary>
/// ステージクラス
/// </summary>
class Stage
{
public:

	Stage();
	virtual ~Stage();

	void Init(int stageNo);
	void Update();
	void End();

private:

	std::shared_ptr<Ground> m_pGround;
	std::shared_ptr<Wall> m_pWall;

};

