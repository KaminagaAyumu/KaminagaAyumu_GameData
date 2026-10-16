#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>
#include "../MyLib/MyLib.h"

class Player;
class Enemy;
class Model;
class Camera;
class EnemyManager;
class BulletManager;
class UIText;
class UIImage;
class UISelectList;

class GameScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	GameScene(SceneController& controller);
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
	/// <summary>
	/// 次のシーン遷移先
	/// </summary>
	enum class NextScene
	{
		Game, // ゲームシーン続行
		Clear, // ゲームクリア
		Result, // リザルト
		Gameover, // ゲームオーバー
		None // 遷移先シーンなし
	};

	/// <summary>
	/// UIの表示状態
	/// </summary>
	enum class UIMode
	{
		Tutorial,
		SkipDialog
	};

	/// <summary>
	/// チュートリアルの状態内部
	/// </summary>
	struct TutorialStep
	{
		int movieHandle;			// ムービーの画像ハンドル
		std::weak_ptr<UIText> text;	// 表示するテキスト
		int waitFrame;				// 待機するフレーム
		bool isMovieEnd;			// ムービーが終わったか
	};

	// 床モデルのハンドル
	int m_groundHandle;

	// スカイボックスのモデルのハンドル
	int m_skyboxHandle;

	// シャドウマップを使用するためのハンドル
	int m_shadowMapHandle;

	// ゲームの経過時間
	int m_gameTime;

	// マップを見下ろす時間
	int m_lookDownFrame;

	// ミニマップを表示する画像ハンドル
	int m_miniMapHandle;

	// 表示用のスコア
	int m_displayedScore;

	// 加算後のスコア
	int m_targetScore;

	// マップを見下ろすカメラの補完スピード
	float m_lookDownCameraLerpSpeed;

	// カウントダウンのテキストがフェードアウト開始したか
	bool m_isCountDownTextStartFadeOut;

	// マップを開くかどうか
	bool m_isOpenMap;

	std::shared_ptr<Player> m_pPlayer;

	std::shared_ptr<Model> m_pGround;

	std::shared_ptr<Camera> m_pCamera;

	// 弾管理クラス
	std::shared_ptr<BulletManager> m_pBulletManager;

	// 敵管理クラス
	std::shared_ptr<EnemyManager> m_pEnemyManager;

	// カウントダウン時に表示するテキストのポインタ
	std::weak_ptr<UIText> m_pCountDownText;

	// 時間を表示するテキストのポインタ
	std::weak_ptr<UIText> m_pTimeText;

	// スコアを表示するテキストのポインタ
	std::weak_ptr<UIText> m_pScoreText;

	// マップ全体を見下ろしているときのテキストのポインタ
	std::weak_ptr<UIText> m_pLookDownText;

	// 背景画像を表示するポインタ
	std::weak_ptr<UIImage> m_pBackGroundImage;

	// ミニマップを表示する画像のポインタ
	std::weak_ptr<UIImage> m_pMiniMapImage;

	// Aボタンの画像ポインタ
	std::weak_ptr<UIImage> m_pAButtonImage;
	
	// チュートリアルのムービーを表示する画像ポインタ
	std::weak_ptr<UIImage> m_pTutorialMovieImage;

	// チュートリアルをスキップするかどうかのダイアログを表示する用の選択肢ポインタ
	std::weak_ptr<UISelectList> m_pDialog;

	// チュートリアルの状態を管理するvector
	std::vector<TutorialStep> m_steps;

	// チュートリアルの進行度
	int m_tutorialStepNum;

	// 更新処理用の関数ポインタの別名を定義
	using UpdateFunc_t = void(GameScene::*)(Input&);
	UpdateFunc_t m_update; // 更新処理を定義

	// 描画処理用の関数ポインタの別名を定義
	using DrawFunc_t = void(GameScene::*)();
	DrawFunc_t m_draw; // 描画処理を定義

	// 次のシーン
	NextScene m_nextScene;

	// ライトの向き(変数にする必要がない可能性がある)
	Vector3 m_lightDir;

	// マップ見下ろしの際の座標
	Vector3 m_lookDownTarget;

	// UIの状態
	UIMode m_uiMode;

// メンバ関数
private:
	// 更新処理
	void FadeInUpdate(Input&);
	void TutorialUpdate(Input&);
	void LookDownUpdate(Input&);
	void CountDownUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);
	void YesNoDialogUpdate(Input& input);

	// 描画処理
	void TutorialDraw();
	void LookDownDraw();
	void CountDownDraw();
	void NormalDraw();
	void FadeDraw();

	/// <summary>
	/// チュートリアルのデータを初期化
	/// </summary>
	void InitTutorialData();

	/// <summary>
	/// 見下ろしマップの更新処理に変更する際の処理
	/// </summary>
	void ChangeLookDown();

	/// <summary>
	/// チュートリアルのステップを進める
	/// </summary>
	void AdvanceStep();

	// ミニマップを描画する
	void DrawMiniMap();

	/// <summary>
	/// 次のシーンのBGMを確認
	/// </summary>
	void CheckNextBGM();

	/// <summary>
	/// ライトの向きを更新
	/// </summary>
	void UpdateLightDir();

	/// <summary>
	///  はいかいいえのダイアログを表示する
	/// </summary>
	void OpenYesNoDialog();

	/// <summary>
	/// はいかいいえのダイアログを非表示にする
	/// </summary>
	void CloseYesNoDialog();

};

