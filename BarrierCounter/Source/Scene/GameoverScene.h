#pragma once
#include "SceneBase.h"
#include <memory>

class UIText;
class UISelectList;

class GameoverScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	GameoverScene(SceneController& controller);
	virtual ~GameoverScene();

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
	using UpdateFunc_t = void(GameoverScene::*)(Input&);
	UpdateFunc_t m_update; // 更新処理を定義

	// 描画処理用の関数ポインタの別名を定義
	using DrawFunc_t = void(GameoverScene::*)();
	DrawFunc_t m_draw; // 描画処理を定義

	// シーン遷移の選択肢を表示するリスト
	std::weak_ptr<UISelectList> m_pSelectList;

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

