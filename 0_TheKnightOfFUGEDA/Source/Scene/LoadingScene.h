#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>

class LoadingScene : public SceneBase
{
public:

	enum class TransitionType
	{
		Change,
		Push,
		Reset
	};

public:

	LoadingScene(std::function<std::shared_ptr<SceneBase>()> m_nextSceneFactory, std::wstring filePath, SceneController& controller, TransitionType transitionType);

	// デストラクタ
	virtual ~LoadingScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;


private:

	std::function<std::shared_ptr<SceneBase>()> m_nextSceneFactory;
	std::wstring m_filePath;
	TransitionType m_transitionType;

};

