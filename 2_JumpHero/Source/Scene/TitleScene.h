#pragma once
#include "SceneBase.h"

// プロトタイプ宣言
class Bg;
class Map;
class Camera;
class Player;
class SoundManager;
class UIManager;
class EffectManager;

/// <summary>
/// タイトルシーンクラス
/// </summary>
class TitleScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	TitleScene(SceneController& controller);
	virtual ~TitleScene();
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
	int m_titleImageHandle; // タイトル画像のハンドル
	int m_frameCount; // フレームカウント

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (TitleScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (TitleScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	std::shared_ptr<Bg> m_bg;
	std::shared_ptr<Map> m_pMap; // タイトル用のマップを準備
	std::shared_ptr<Camera> m_pCamera; // スクロール用のカメラを準備
	std::shared_ptr<SoundManager> m_soundManager; // サウンドマネージャークラス
	std::shared_ptr<UIManager> m_pUIManager; // UIマネージャークラス
	std::shared_ptr<EffectManager> m_pEffectManager; 
	std::shared_ptr<Player> m_pTitlePlayer; // タイトル画面専用のプレイヤー

};

