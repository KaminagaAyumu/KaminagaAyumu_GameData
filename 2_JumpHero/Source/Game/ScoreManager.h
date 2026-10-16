#pragma once
#include "../Utility/Game.h"

/// <summary>
/// スコアデータを管理するクラス
/// </summary>
class ScoreManager
{
public:
	void Load();

	/// <summary>
	/// ハイスコアを更新する
	/// </summary>
	/// <param name="stageNo">ステージ番号</param>
	/// <param name="score">ハイスコア</param>
	void UpdateHighScore(int stageNo, int score);

	/// <summary>
	/// 指定したステージ番号のハイスコアを取得する
	/// </summary>
	/// <param name="stageNo"></param>
	int GetHighScore(int stageNo);

	void Save();

private:
	ScoreData m_data;
};

