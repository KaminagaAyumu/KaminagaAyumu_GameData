#pragma once

/// <summary>
/// ゲームシーン内のスコアを管理するクラス
/// </summary>
class ScoreCounter
{
public:
	ScoreCounter();
	virtual ~ScoreCounter();

	/// <summary>
	/// スコアを加算する
	/// </summary>
	/// <param name="score">加算するスコア</param>
	void AddScore(int score);

	/// <summary>
	/// 現在のスコアを取得する
	/// </summary>
	/// <returns>現在のスコア</returns>
	const int GetScore() { return m_score; }

	/// <summary>
	/// 表示用のスコアを取得する
	/// </summary>
	/// <returns>表示用のスコア</returns>
	const int GetDisplayedScore() { return m_displayedScore; }

	void Update();

private:
	// スコア
	int m_score;
	// 加算演出に使用するスコア
	int m_displayedScore;
};

