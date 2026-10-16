#pragma once
#include "SceneBase.h"

// 前方宣言
class Bg;
class SoundManager;
class TextManager;
class UIManager;
class UIImage;
class UIFormatText;
class UISelectList;
class UITextWindow;
class EffectManager;

/// <summary>
/// セレクトシーン
/// ステージ選択などを行う
/// </summary>
class SelectScene : public SceneBase
{
public:
	SelectScene(SceneController& controller);
	virtual ~SelectScene();

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
	void FadeInUpdate(Input& input); // フェードイン中の更新処理
	void NormalUpdate(Input& input); // 通常時の更新処理
	void DialogUpdate(Input& input); // ダイアログ表示時の更新処理
	void FadeOutUpdate(Input& input); // フェードアウト中の更新処理
	using UpdateFunc_t = void (SelectScene::*)(Input& input); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ

	// 描画処理用関数群
	void NormalDraw(); // 通常時の描画処理
	void DialogDraw(); // ダイアログ表示時の描画処理
	void FadeDraw(); // フェード中の描画処理
	using DrawFunc_t = void (SelectScene::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ

	/// <summary>
	/// 現在のカーソルの位置を探す
	/// </summary>
	void CheckCursor();

	std::shared_ptr<Bg> m_pBg; // 背景管理クラスへのポインタ

	std::shared_ptr<SoundManager> m_soundManager; // サウンドマネージャーへのポインタ

	std::unique_ptr<TextManager> m_pTextManager; // テキストデータを使用するためのポインタ

	std::unique_ptr<UIManager> m_pUIManager; // UIを使用するためのポインタ

	std::weak_ptr<UIImage> m_pThumbnail; // ステージのサムネイル画像

	std::shared_ptr<EffectManager> m_pEffectManager; // エフェクトを管理するためのポインタ

	std::weak_ptr<UIFormatText> m_pHighScoreText; // ハイスコアを表示するためのポインタ

	std::weak_ptr<UISelectList> m_pSelectList; // 選択できるリストを管理するためのポインタ

	std::weak_ptr<UISelectList> m_pDialogList; // はいかいいえを選択するダイアログ管理用のポインタ

	std::weak_ptr<UITextWindow> m_pDescriptionWindow; // ステージの説明を表示するためのポインタ

};

