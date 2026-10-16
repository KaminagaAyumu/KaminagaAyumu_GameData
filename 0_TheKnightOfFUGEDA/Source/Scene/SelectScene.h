#pragma once
#include "SceneBase.h"
#include "../MyLib/Component/Draw/UI/UIText.h"
#include "../MyLib/Component/Draw/UI/UISelectList.h"
#include <memory>

class File;
class Skybox;

class SelectScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit SelectScene(SceneController& controller);
	// デストラクタ
	virtual ~SelectScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:
	using UpdateFunc_t = void(SelectScene::*)();
	UpdateFunc_t m_update;

	using DrawFunc_t = void(SelectScene::*)()const;
	DrawFunc_t m_draw;

	std::shared_ptr<File> m_file;

	std::shared_ptr<Skybox> m_pSkybox;

	std::shared_ptr<MyLib::GameObject> m_pUIText;
	std::weak_ptr<MyLib::UIText> m_pText;

	std::shared_ptr<MyLib::GameObject> m_pUITextTime;
	std::weak_ptr<MyLib::UIText> m_pTextTime;

	std::shared_ptr<MyLib::GameObject> m_pUISelectList;
	std::weak_ptr<MyLib::UISelectList> m_pSelectList;

private:
	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalUpdate();

	void FadeDraw()const;
	void NormalDraw()const;
};

