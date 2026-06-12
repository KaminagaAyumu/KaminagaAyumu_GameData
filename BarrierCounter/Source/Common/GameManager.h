#pragma once
#include <vector>
#include "Game.h"

/// <summary>
/// ゲーム中共通のデータを管理する構造体
/// </summary>
class GameManager
{
public:
	/// <summary>
	/// ステージごとのデータ
	/// </summary>
	struct StageData
	{
		int bestTime = 999 * 60;		// ベストタイム(クリア時間)
		int highScore = 0;				// ハイスコア
	};

public:
	GameManager();
	virtual ~GameManager() = default;

	/// <summary>
	/// 初期化処理
	/// ステージ毎のデータ作成等を行う
	/// </summary>
	void Init();

	/// <summary>
	/// 現在のステージを取得する
	/// </summary>
	/// <returns></returns>
	StageType GetCurrentStage() { return m_currentStage; }

	/// <summary>
	/// ステージのクリアタイムを取得
	/// </summary>
	/// <returns></returns>
	int GetClearTime() const { return m_currentClearTime; }

	/// <summary>
	/// 累計スコアを取得する
	/// </summary>
	/// <returns></returns>
	int GetTotalScore() const { return m_totalScore; }

	/// <summary>
	/// タイムボーナスの最大時間を取得する
	/// </summary>
	/// <returns></returns>
	const float GetBonusMaxTime()const;

	/// <summary>
	/// 指定ステージのデータを取得する
	/// ハイスコア、ベストタイム
	/// </summary>
	/// <param name="stage">対象のステージ</param>
	/// <returns></returns>
	StageData GetStageData(StageType stage);

	/// <summary>
	/// スコアを加算する
	/// </summary>
	void AddScore(int score);

	/// <summary>
	/// ステージクリアの際に呼ぶ処理
	/// ベストスコアとベストタイムを更新
	/// </summary>
	void OnStageClear(int clearTime);

	/// <summary>
	/// 現在のステージのデータをリセット
	/// </summary>
	void ResetCurrentStageData();

private:
	// 現在のステージ番号
	StageType m_currentStage;
	// 現在のクリアタイム
	int m_currentClearTime;
	// 現在のスコア
	int m_currentStageScore;
	// スコアの合計
	int m_totalScore;

	// ステージごとのデータを保存するvector
	std::vector<StageData> m_stageDatas;
};

