#pragma once
#include "SceneBase.h"
#include "../MyLib/Component/Draw/UI/UISelectList.h"
#include <memory>

class GameoverScene : public SceneBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit GameoverScene(SceneController& controller, int stageNo);

	// デストラクタ
	virtual ~GameoverScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:

	using UpdateFunc_t = void(GameoverScene::*)();
	UpdateFunc_t m_update;

	using DrawFunc_t = void(GameoverScene::*)()const;
	DrawFunc_t m_draw;

	// ゲームオーバーの文字の画像
	std::shared_ptr<MyLib::GameObject> m_pUIGameOverImage;

	std::shared_ptr<MyLib::GameObject> m_pUISelectList;
	std::weak_ptr<MyLib::UISelectList> m_pSelectList;

	int m_stageNo;

private:
	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalUpdate();

	void FadeDraw()const;
	void NormalDraw()const;
};

