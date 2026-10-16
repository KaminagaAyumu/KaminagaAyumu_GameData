#pragma once
#include "SceneBase.h"
#include "../Utility/CSV/TextResource.h"
#include "../MyLib/GameObject.h"
#include "../MyLib/Component/Draw/UI/UITelop.h"
#include "../MyLib/Component/Draw/UI/UIText.h"
#include "../MyLib/Component/Draw/UI/UICombo.h"
#include "../MyLib/Component/Draw/UI/UICountDown.h"
#include <memory>

class File;
class Player;
class PlayerCamera;
class EnemyManager;
class ItemManager;
struct EventSensors;
struct EventControls;
class EventManager;
class LockOnMarkerUI;
class LifeUI;
class ParamUI;
class BoardUI;
class Stage;
class Skybox;
class ComboCounter;
class ScoreCounter;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public SceneBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit GameScene(SceneController& controller, int stageNo);
	// デストラクタ
	virtual ~GameScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:

	// 敵管理クラス
	std::shared_ptr<EnemyManager> m_pEnemyManager;

	// アイテム管理クラス
	std::shared_ptr<ItemManager> m_pItemManager;

	std::shared_ptr<Player> m_pPlayer;

	std::shared_ptr<Stage> m_pStage;

	std::shared_ptr<PlayerCamera> m_pCamera;

	std::shared_ptr<EventSensors> m_pEventSensors;
	std::shared_ptr<EventControls> m_pEventControls;
	std::unique_ptr<EventManager> m_pEventManager;

	std::unique_ptr<ComboCounter> m_pComboCounter;

	std::unique_ptr<ScoreCounter> m_pScoreCounter;

	TextResource m_textResources;

	std::shared_ptr<MyLib::GameObject> m_pUITelop;
	std::weak_ptr<MyLib::UITelop> m_pTelop;
	
	std::shared_ptr<MyLib::GameObject> m_pUICountDown;
	std::weak_ptr<MyLib::UICountDown> m_pCountDown;

	std::shared_ptr<MyLib::GameObject> m_pUIText;
	std::weak_ptr<MyLib::UIText> m_pText;

	std::shared_ptr<MyLib::GameObject> m_pUIComboText;
	std::weak_ptr<MyLib::UICombo> m_pComboText;

	std::shared_ptr<ParamUI> m_pScoreUI;

	std::shared_ptr<LockOnMarkerUI> m_pLockOnMarker;

	std::shared_ptr<LifeUI> m_pLifeUI;

	// 操作説明のボード(チュートリアルステージでのみ生成する)
	std::shared_ptr<BoardUI> m_pGuideBoard;

	// ロックオンの対象
	std::weak_ptr<MyLib::Transform> m_pLockOnTarget;

	std::shared_ptr<Skybox> m_pSkybox;

	using UpdateFunc_t = void(GameScene::*)();
	UpdateFunc_t m_update;

	using DrawFunc_t = void(GameScene::*)()const;
	DrawFunc_t m_draw;

	int m_gameTime;

	int m_stageNo;

	// ゲームが開始されているかどうか
	bool m_isGameStart;

	// ロックオンを変更できるかどうか
	bool m_isLockOnChange;

private:
	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalUpdate();

	void FadeDraw()const;
	void NormalDraw()const;

	void SetEventFunc();
};

