#pragma once
#include <memory>
#include <list>

// プロトタイプ宣言
class SceneBase; // シーンの切り替えを行うために基底クラスを宣言

/// <summary>
/// シーンを管理するクラス
/// </summary>
class SceneController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneController();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneController();

	void RequestChangeScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// シーンを変更する
	/// </summary>
	/// <param name="scene">次のシーン</param>
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// シーンのリストに新しくシーンを追加する(ポーズ時などに使用)
	/// </summary>
	/// <param name="scene">新しいシーン</param>
	/// <note>シーンを追加するので、前のシーンも残り続ける</note>
	void PushScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// 最後に追加したシーンを削除する(ポーズ解除時などに使用)
	/// </summary>
	void PopScene();

	/// <summary>
	/// シーンのリセット(シーンのリストの内容を新しいシーンのみにする)
	/// </summary>
	/// <param name="scene">新しいシーン</param>
	void ResetScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// シーンが持っている更新処理を行う
	/// </summary>
	void Update();

	/// <summary>
	/// シーンが持っている描画処理を行う
	/// </summary>
	void Draw() const;

private:

	enum class PendingType
	{
		None,
		Change,
		Reset
	};
	std::shared_ptr<SceneBase> m_pendingScene;
	PendingType m_pendingType;

	std::list<std::shared_ptr<SceneBase>> m_scenes; // 使用するシーン

private:
	void ApplyPendingSceneChange();
};

