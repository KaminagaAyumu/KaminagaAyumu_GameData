#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

// プロトタイプ宣言
class Actor;
class Player;
class Chest;
class Bg;
class Map;
class Camera;
class GameManager;
class UIManager;
class UIFormatText;
class UIGauge;
class TextManager;
class CollisionManager;
class SoundManager;
class EffectManager;
class EventManager;
struct EventControls;
struct EventSensors;
class PositionRegistry;

/// <summary>
/// ゲームのメインとなるシーン
/// </summary>
class GameScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	GameScene(SceneController& controller, int stageNo);
	virtual ~GameScene();
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
	uint32_t m_fadeColor; // 単色フェード用の色
	
	int m_chestOpenNum; // 宝箱を開ける用

	int m_stageNo; // ステージの番号を保持しておく

	bool m_isInputOK; // OKボタンで進むイベントを指定するためのもの

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void MissUpdate(Input&); // プレイヤーがミスになった時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (GameScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (GameScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ
	
	/// <summary>
	/// イベントの内容を設定する関数
	/// </summary>
	void SetEventFunc();

	// 背景画像を管理するクラスのポインタ
	std::shared_ptr<Bg> m_bg;

	// ゲーム内座標での描画を管理するクラスのポインタ
	std::shared_ptr<Camera> m_pCamera;

	// ゲームシーン内の状態管理用ポインタ
	std::shared_ptr<GameManager> m_pGameManager;

	// マップのデータを取得するポインタ
	std::shared_ptr<Map> m_pMap;

	std::vector<std::weak_ptr<Actor>> m_pActors; // ゲームシーン内に存在するオブジェクトを管理するコンテナ
	

	// UI関連
	std::shared_ptr<UIManager> m_pUIManager; // UIを管理するために取得

	std::weak_ptr<UIFormatText> m_pScoreText; // スコアを表示するときに使用

	std::weak_ptr<UIFormatText> m_pLifeText; // 残機を表示するときに使用
	
	std::weak_ptr<UIFormatText> m_pMedalText; // メダルの数を表示するときに使用

	std::weak_ptr<UIGauge> m_pDropItemGauge; // 敵をコインに変えるアイテムが出るかわかるようにするUI
	
	std::unique_ptr<TextManager> m_pTextManager; // テキストデータを管理するために取得

	std::unique_ptr<CollisionManager> m_pCollisionManager; // 当たり判定管理クラス

	std::shared_ptr<SoundManager> m_pSoundManager; // サウンドマネージャークラス

	std::shared_ptr<EffectManager> m_pEffectManager; // エフェクトマネージャークラス

	std::unique_ptr<PositionRegistry> m_pPositionRegistry; // イベントで使う座標を取得するクラスを持つ

	std::shared_ptr<EventSensors> m_pEventSensors; // イベントの発動条件を設定するように持つ

	std::shared_ptr<EventControls> m_pEventControls; // 行うイベントを設定するように持つ

	std::unique_ptr<EventManager> m_pEventManager; // イベントマネージャークラス

};

