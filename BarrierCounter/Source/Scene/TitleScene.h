#pragma once
#include "SceneBase.h"
#include "../Geometry/Vector3.h"
#include <memory>

class Model;
class Camera;
class UISelectList;

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

	// 更新処理用の関数ポインタの別名を定義
	using UpdateFunc_t = void(TitleScene::*)(Input&);
	UpdateFunc_t m_update; // 更新処理を定義

	// 描画処理用の関数ポインタの別名を定義
	using DrawFunc_t = void(TitleScene::*)();
	DrawFunc_t m_draw; // 描画処理を定義

	// スカイボックスのモデルハンドル
	int m_skyboxHandle;

	// スカイボックスのモデル
	std::shared_ptr<Model> m_pSkyboxModel;

	// カメラクラス
	std::shared_ptr<Camera> m_pCamera;

	// スカイボックスの向き
	Vector3 m_skyboxDir;

	// シーン遷移のセレクトリストを取得するポインタ
	std::weak_ptr<UISelectList> m_pSelectList;

	// ゲーム終了を確認するダイアログを使うためのポインタ
	std::weak_ptr<UISelectList> m_pDialog;

	/// <summary>
	/// UIの選択状態
	/// </summary>
	enum class UIMode
	{
		TitleSelect,
		YesNoDialog
	};

	enum class FadeEndState
	{
		None,
		GameStart,
		GameEnd
	};

	// UIの選択状態
	UIMode m_mode;
	// フェードが終わった後の処理
	FadeEndState m_fadeEndState;

	// メンバ関数
private:
	// 更新処理
	void FadeInUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);

	// 描画処理
	void NormalDraw();
	void FadeDraw();

	/// <summary>
	/// タイトルの選択を行っている際の更新
	/// </summary>
	/// <param name="input"></param>
	void UpdateTitleSelect(Input& input);
	/// <summary>
	/// はいかいいえの選択を行っている際の更新
	/// </summary>
	/// <param name="input"></param>
	void UpdateYesNoDialog(Input& input);

};

