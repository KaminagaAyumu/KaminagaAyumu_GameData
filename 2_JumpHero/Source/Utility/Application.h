#pragma once
#include <memory>

// 前方宣言
class SoundManager;
class ScoreManager;

/// <summary>
/// アプリケーションを管理するシングルトンクラス
/// </summary>
class Application
{
public:
	virtual ~Application(); // デストラクタ

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static Application& GetInstance();

	/// <summary>
	/// サウンドマネージャーのスマートポインタを取得する
	/// </summary>
	/// <returns>サウンドマネージャーのポインタ</returns>
	std::shared_ptr<SoundManager> GetSoundManager() { return m_soundManager; }

	/// <summary>
	/// スコアマネージャーのスマートポインタを取得する
	/// </summary>
	/// <returns>スコアマネージャーのポインタ</returns>
	std::shared_ptr<ScoreManager> GetScoreManager() { return m_scoreManager; }

	/// <summary>
	/// ゲームの初期化
	/// </summary>
	/// <returns>成功 : true 失敗 : false</returns>
	bool Init();
	
	/// <summary>
	/// ゲームループを行う
	/// </summary>
	void Run();
	
	/// <summary>
	/// ゲームの終了
	/// </summary>
	void Terminate();

	/// <summary>
	///  ゲームを終了するという命令を飛ばす
	/// </summary>
	void RequestGameEnd();

private:
	Application(); // コンストラクタをprivateで宣言
	Application(const Application&) = delete; // コピーコンストラクタを作れないようにする
	void operator=(const Application&) = delete; // 代入演算子も使えないようにする

	std::shared_ptr<SoundManager> m_soundManager; // 多くのクラスで使用するためサウンドマネージャーを保持する
	std::shared_ptr<ScoreManager> m_scoreManager; // スコアを管理するマネージャーを保持

	bool m_isGameEnd; // ゲームを終了するためのフラグ
};

