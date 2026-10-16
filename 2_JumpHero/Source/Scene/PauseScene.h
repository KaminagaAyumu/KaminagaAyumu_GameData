#pragma once
#include "SceneBase.h"

class UIManager;
class UISelectList;

/// <summary>
/// シーンを一時停止するクラス
/// </summary>
class PauseScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	PauseScene(SceneController& controller);
	virtual ~PauseScene();
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
	int m_transitionInterval;	// シーン遷移のインターバル

	// 更新処理用関数群
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (PauseScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (PauseScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	std::unique_ptr<UIManager> m_pUIManager; // UIを管理するために取得
	std::weak_ptr<UISelectList> m_pSelectList; // 選択できるリストを取得

};

