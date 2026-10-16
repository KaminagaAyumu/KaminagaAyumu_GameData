#pragma once
#include <string>

// プロトタイプ宣言
class SceneController; // シーン管理を行わせるために宣言

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
	explicit SceneBase(SceneController& controller);
	
	virtual ~SceneBase() = default; // 特に使わないのでdefault

	virtual std::string GetResourceManifestPath() const { return ""; }

	/// <summary>
	/// 初期化処理
	/// 最初に行う処理
	/// </summary>
	virtual void Init() abstract;

	/// <summary>
	/// 終了処理
	/// リソースの開放等を行う
	/// </summary>
	virtual void End() abstract;
	
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() abstract;

	/// <summary>
	/// 描画処理
	/// 値の変更等を行わないようにするためconstをつけている
	/// </summary>
	virtual void Draw() const abstract;

protected:
	// シーン遷移の際に次のシーンに渡すシーンコントローラー
	SceneController& m_sceneController;

	// フレームカウンタ(フェードで使用する)
	int m_frameCount;

	// フェードの色
	unsigned int m_fadeColor;
};

