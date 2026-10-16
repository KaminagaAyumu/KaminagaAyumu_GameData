#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class Bg;
class GameManager;
class EffectManager;
class UIManager;
class UIFormatText;
class UIGauge;
class TextManager;
class Map;
class Camera;
class Actor;
class CollisionManager;
class EventManager;
struct EventControls;
struct EventSensors;

/// <summary>
/// ミニゲーム用シーン
/// ゲームシーンから遷移
/// </summary>
class MiniGameScene : public SceneBase
{
public:
	MiniGameScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo);
	virtual ~MiniGameScene();

	/// <summary>
	/// シーンの更新処理
	/// </summary>
	/// <param name="input">入力情報を取得するクラス</param>
	void Update(Input& input) override;
	/// <summary>
	/// シーンの描画処理
	/// </summary>
	void Draw() override;

private:
	unsigned int m_fadeColor; // 単色フェード用の色

	int m_stageNo; // ステージの番号を取得

	bool m_isInputOK; // OKボタンが押されたかどうかの関数

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (MiniGameScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (MiniGameScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	bool IsEndDescription() const; // 説明が終了したかどうかを取得する関数

	void GetBalloonNum() const; // 風船の数を取得する関数

	/// <summary>
	/// イベントの内容を設定する関数
	/// </summary>
	void SetEventFunc();

	// 背景画像を管理するクラスのポインタ
	std::shared_ptr<Bg> m_bg;

	// ゲームシーン内の状態管理用ポインタ(GameSceneから引き継ぐ)
	std::shared_ptr<GameManager> m_pGameManager;

	std::shared_ptr<EffectManager> m_pEffectManager;

	std::shared_ptr<UIManager> m_pUIManager;

	std::weak_ptr<UIFormatText> m_pScoreText;

	std::weak_ptr<UIFormatText> m_pLifeText; // 残機を表示するときに使用

	std::weak_ptr<UIFormatText> m_pMedalText; // メダルの数を表示するときに使用

	std::weak_ptr<UIGauge> m_pDropItemGauge; // 敵をコインに変えるアイテムが出るかわかるようにするUI

	std::unique_ptr<TextManager> m_pTextManager;

	// マップのデータを取得するポインタ
	std::shared_ptr<Map> m_pMap;

	std::shared_ptr<Camera> m_pCamera;

	std::vector<std::weak_ptr<Actor>> m_pActors; // ゲームシーン内に存在するオブジェクトを管理するコンテナ

	std::unique_ptr<CollisionManager> m_pCollisionManager; // 当たり判定管理クラス

	std::shared_ptr<EventSensors> m_pEventSensors; // イベントの発動条件を設定するように持つ

	std::shared_ptr<EventControls> m_pEventControls; // 行うイベントを設定するように持つ

	std::unique_ptr<EventManager> m_pEventManager; // イベントマネージャークラス
};

