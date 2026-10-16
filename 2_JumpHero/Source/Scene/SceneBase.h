#pragma once
// プロトタイプ宣言
class SceneController; // シーン管理を行わせるために宣言
class Input; // 入力情報を取得できるように宣言

/// <summary>
/// シーンの基底クラス
/// </summary>
class SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	SceneBase(SceneController& controller);

	virtual ~SceneBase() = default; // 特に使わないのでdefault

	/// <summary>
	/// シーンの更新処理
	/// </summary>
	/// <param name="input">入力情報を取得するクラス</param>
	/// <note>基底クラスのためabstractで純粋仮想関数にしています</note>
	virtual void Update(Input& input) abstract;

	/// <summary>
	/// シーンの描画処理
	/// </summary>
	/// <note>基底クラスのためabstractで純粋仮想関数にしています</note>
	virtual void Draw() abstract;


protected:
	// シーン遷移の際に使えるようにする
	SceneController& m_controller;

	int m_frameCount;
};

