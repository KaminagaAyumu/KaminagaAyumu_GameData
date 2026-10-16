#pragma once
#include "SceneBase.h"
#include "../MyLib/Component/Draw/UI/UIText.h"
#include "../MyLib/Component/Draw/UI/UISelectList.h"
#include <memory>

class Skybox;
class ParamUI;
class BoardUI;
struct ResultData;

class ResultScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit ResultScene(SceneController& controller, std::shared_ptr<ResultData> data);
	// デストラクタ
	virtual ~ResultScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:
	using UpdateFunc_t = void(ResultScene::*)();
	UpdateFunc_t m_update;

	using DrawFunc_t = void(ResultScene::*)()const;
	DrawFunc_t m_draw;

	std::shared_ptr<MyLib::GameObject> m_pUIText;
	std::weak_ptr<MyLib::UIText> m_pText;
	
	std::shared_ptr<ParamUI> m_pScoreUI;
	std::shared_ptr<ParamUI> m_pTimeUI;
	std::shared_ptr<ParamUI> m_pLifeUI;
	std::shared_ptr<ParamUI> m_pMaxComboUI;

	std::shared_ptr<ParamUI> m_pTimeBonusUI;
	std::shared_ptr<ParamUI> m_pLifeBonusUI;
	std::shared_ptr<ParamUI> m_pComboBonusUI;
	
	std::shared_ptr<BoardUI> m_pHighScoreBoard;

	std::shared_ptr<MyLib::GameObject> m_pUISelectList;
	std::weak_ptr<MyLib::UISelectList> m_pSelectList;

	std::shared_ptr<Skybox> m_pSkybox;

	std::shared_ptr<ResultData> m_pResultData;

	std::shared_ptr<ResultData> m_pDisplayedResultData;

	struct BonusScore
	{
		int timeBonus = 0;
		int lifeBonus = 0;
		int comboBonus = 0;
	};

	BonusScore m_bonusScore;

	BonusScore m_displayedBonusScore;

	// リザルト画面に遷移した時点でのハイスコア
	int m_prevHighScore;

private:
	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalUpdate();

	void FadeDraw()const;
	void NormalDraw()const;

	/// <summary>
	/// UIにリザルト用のデータを適用させる
	/// </summary>
	void SetResultData();

};

