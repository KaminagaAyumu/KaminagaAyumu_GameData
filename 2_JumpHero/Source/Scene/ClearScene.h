#pragma once
#include "SceneBase.h"

class Bg;
class GameManager;
class UIManager;
class UISelectList;
class UIText;
class UIFormatText;

/// <summary>
/// クリアシーン
/// リザルト表示などを行う
/// </summary>
class ClearScene : public SceneBase
{
public:
	ClearScene(SceneController& controller, std::shared_ptr<GameManager> gameManager, int stageNo);
	virtual ~ClearScene();

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

	int m_stageNo; // クリアしたステージの番号

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (ClearScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (ClearScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	const int GetScore()const { return m_resultScore; };

	std::shared_ptr<Bg> m_pBg; // 背景

	std::shared_ptr<GameManager> m_pGameManager;

	std::shared_ptr<UIManager> m_pUIManager;

	std::weak_ptr<UISelectList> m_pSelectList;

	std::weak_ptr<UIFormatText> m_pScoreText; // スコアを表示するために取得

	std::weak_ptr<UIText> m_pClearText; // クリアという文字を表示するために取得

	std::weak_ptr<UIText> m_pHighScoreUpdateText; // ハイスコア更新時に文字を表示するために取得

	// リザルト表示用スコア
	int m_resultScore;
	// ゲーム内のスコア
	int m_gameScore;

	// ハイスコア
	int m_highScore;


};

