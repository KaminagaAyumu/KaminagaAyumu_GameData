#pragma once
#include "SceneBase.h"

/// <summary>
/// プレイヤーがミスした際の残機表示シーン
/// </summary>
class MissScene : public SceneBase
{
public:
	MissScene(SceneController& controller);
	virtual ~MissScene();

	void Update(Input&) override;
	void Draw() override;


};

