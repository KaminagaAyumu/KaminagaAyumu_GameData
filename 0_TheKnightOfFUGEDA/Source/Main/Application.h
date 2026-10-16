#pragma once
#include <vector>

/// <summary>
/// アプリケーション管理クラス(シングルトン)
/// </summary>
class Application
{
public:
	virtual ~Application(); // デストラクタ

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns>アプリケーションクラスのインスタンス</returns>
	static Application& GetInstance();

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

	/// <summary>
	/// タイムスケールをセットする
	/// </summary>
	/// <param name="timeScale">セットしたいタイムスケール</param>
	void SetTimeScale(float timeScale) { m_timeScale = timeScale; }

	/// <summary>
	/// 現在のタイムスケールを取得する
	/// </summary>
	/// <returns>現在のタイムスケール</returns>
	float GetTimeScale() const { return m_timeScale; }

	/// <summary>
	/// ハイスコアをセットする
	/// </summary>
	/// <param name="score">スコアの値</param>
	/// <param name="stageNo">ステージ番号</param>
	void SetHighScore(int score, int stageNo);

	std::vector<int> GetHighScore();

private:
	/// <summary>
	/// コンストラクタ
	/// シングルトンクラスのためprivateで宣言する
	/// ※宣言の際にcppファイルの一番上に配置しています
	/// </summary>
	Application();
	Application(const Application&) = delete;		// コピーコンストラクタを作れないようにする
	void operator=(const Application&) = delete;	// 代入演算子を使えないようにする

	bool m_isGameEnd; // ゲーム終了フラグ

	float m_timeScale; // ゲームの時間スピード

	// ハイスコア
	std::vector<int> m_highScores;

#ifdef _DEBUG
	// デバッグの状況
	enum class DebugState
	{
		Normal, // 通常
		Pause,	// ポーズ(コマ送り可)
	};
	DebugState m_debugState = DebugState::Normal;
#endif

};

