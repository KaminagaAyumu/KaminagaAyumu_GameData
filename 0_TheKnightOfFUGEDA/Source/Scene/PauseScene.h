#pragma once
#include "SceneBase.h"
#include "../MyLib/Component/Draw/UI/UISelectList.h"
#include <memory>

class PauseScene : public SceneBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit PauseScene(SceneController& controller);

	// デストラクタ
	virtual ~PauseScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:

	using UpdateFunc_t = void(PauseScene::*)();
	UpdateFunc_t m_update;

	using DrawFunc_t = void(PauseScene::*)()const;
	DrawFunc_t m_draw;

	std::shared_ptr<MyLib::GameObject> m_pUISelectList;
	std::weak_ptr<MyLib::UISelectList> m_pSelectList;

private:
	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalUpdate();

	void FadeDraw()const;
	void NormalDraw()const;

};

