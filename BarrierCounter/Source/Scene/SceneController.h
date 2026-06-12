#pragma once
#include <memory>
#include <list>
// プロトタイプ宣言
class Input; // シーンからUpdateを呼び出すため宣言
class SceneBase; // シーンの切り替えを行うため宣言
class GameManager; // ゲーム共通の状態を保持しておくため宣言

/// <summary>
/// シーンを管理するクラス
/// </summary>
class SceneController
{
public:

	/// <summary>
	/// コンストラクタ
	/// エフェクトリソースマネージャーを取得する
	/// </summary>
	SceneController();

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
	/// <param name="input">入力情報を取得するクラス</param>
	void Update(Input& input);
	
	/// <summary>
	/// シーンが持っている描画処理を行う
	/// </summary>
	void Draw();

	/// <summary>
	/// ゲームマネージャーを取得
	/// </summary>
	/// <returns>ゲームマネージャーのweak_ptr</returns>
	std::weak_ptr<GameManager> GetGameManager();

private:
	std::list<std::shared_ptr<SceneBase>> m_scenes; // スタックに確保する用のシーン

	std::shared_ptr<GameManager> m_pGameManager; // ゲーム共通のデータを持つクラス
};

