#pragma once

// ゲーム全体で使用する定数
namespace Game
{
	// 画面情報
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
	constexpr int kColorBitNum = 32;

	// マップの端の座標
	constexpr int kMapMinX = -5000;
	constexpr int kMapMaxX = 5000;
	constexpr int kMapMinZ = -4200;
	constexpr int kMapMaxZ = 4200;
}

/// <summary>
/// ステージ番号
/// </summary>
enum class StageType : int
{
	Stage1,
	Stage2,
	Stage3,
	Clear,
	Max,
};
