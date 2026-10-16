#pragma once
#include "SceneBase.h"
#include "../MyLib/GameObject.h"
#include "../MyLib/Component/Draw/UI/UISelectList.h"
#include <memory>

class File;
class Player;
class PlayerCamera;
class Skybox;
class Effect;

class TitleScene : public SceneBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit TitleScene(SceneController& controller);
	// デストラクタ
	virtual ~TitleScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:

	// 後で消す
	std::shared_ptr<File> m_file;
	std::shared_ptr<File> m_file2;

	std::shared_ptr<PlayerCamera> m_pCamera;

	std::shared_ptr<MyLib::GameObject> m_titleLogo;
	std::shared_ptr<MyLib::GameObject> m_startText;

	std::shared_ptr<Skybox> m_skybox;

	std::weak_ptr<Effect> m_pEffect;

	using UpdateFunc_t = void(TitleScene::*)();
	UpdateFunc_t m_update;

	using DrawFunc_t = void(TitleScene::*)()const;
	DrawFunc_t m_draw;

	int m_fadeInterval;

	std::shared_ptr<MyLib::GameObject> m_pUISelectList;
	std::weak_ptr<MyLib::UISelectList> m_pSelectList;

private:

	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalUpdate();
	// ゲームを開始するか終了するかを選ぶ際の更新処理
	void SelectUpdate();

	void FadeDraw()const;
	void NormalDraw()const;

};

