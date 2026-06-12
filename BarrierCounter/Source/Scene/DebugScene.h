#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>
#include <functional>

/// <summary>
/// デバッグ用シーン
/// ここからすべてのシーンに遷移可能とする
/// </summary>
class DebugScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン管理用クラス(継承元で取得しなければならない)</param>
	DebugScene(SceneController& controller);
	virtual ~DebugScene();

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
	/// 選択肢の内容
	/// </summary>
	struct SelectData
	{
		std::wstring text;
		std::function<void()> onSelect;
	};

	// カーソルの座標
	int m_cursor;

	// 選択できるシーン
	std::vector<SelectData> m_selectScenes;

};

