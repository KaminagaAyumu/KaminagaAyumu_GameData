#pragma once

// ゲーム全体で使用する定数
namespace Game
{
	// 画面情報
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
	constexpr int kColorBitNum = 32;
}

/// <summary>
/// スコアをセーブするための構造体
/// </summary>
struct ScoreData
{
	int tutorialScore = 0;
	int stage1Score = 0;
	int stage2Score = 0;
	int stage3Score = 0;
};
