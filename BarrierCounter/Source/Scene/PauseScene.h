#pragma once
#include "SceneBase.h"
#include <memory>

class UISelectList;
class UIImage;

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
	// 更新処理用の関数ポインタの別名を定義
	using UpdateFunc_t = void(PauseScene::*)(Input&);
	UpdateFunc_t m_update; // 更新処理を定義

	// 描画処理用の関数ポインタの別名を定義
	using DrawFunc_t = void(PauseScene::*)();
	DrawFunc_t m_draw; // 描画処理を定義

	// 背景画像を表示するポインタ
	std::weak_ptr<UIImage> m_pBackGround;

	// 選択肢のポインタ
	std::weak_ptr<UISelectList> m_pSelectList;

	// 元のシーンに戻るかどうか
	bool m_isBackScene;

	// 遷移のインターバル値
	int m_transitionInterval;

	// メンバ関数
private:
	// 更新処理
	void FadeInUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);

	// 描画処理
	void NormalDraw();
	void FadeDraw();

};

