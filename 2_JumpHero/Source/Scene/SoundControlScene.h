#pragma once
#include "SceneBase.h"

// 前方宣言
class SoundManager;
class UIManager;
class UISelectList;

class SoundControlScene : public SceneBase
{
public:
	SoundControlScene(SceneController& controller);
	virtual ~SoundControlScene();

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

	// 更新処理用関数群
	void AppearUpdate(Input& input); // 出現中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void DisAppearUpdate(Input& input); // 消去中の更新処理
	using UpdateFunc_t = void (SoundControlScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void IntervalDraw(); // 時間経過中の描画処理
	using DrawFunc_t = void (SoundControlScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	std::shared_ptr<SoundManager> m_soundManager; // サウンドマネージャーへのポインタ

	std::unique_ptr<UIManager> m_pUIManager; // UIを使用するためのポインタ

	std::weak_ptr<UISelectList> m_pSelectList; // 選択できるリストを管理するためのポインタ
};

