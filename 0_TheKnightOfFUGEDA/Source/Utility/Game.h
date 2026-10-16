#pragma once

// ゲーム全体で使用する定数
namespace Game
{
	// 画面情報
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
	constexpr int kColorBitNum = 32;

	// 更新情報
	constexpr int	kOneFrameNanoSec = 16667; // 1フレームのナノ秒(60FPS)
	constexpr int	kFrameRate = 60; // 1秒あたりのフレーム数

	// エフェクトの最大数
	constexpr int kEffectMaxNum = 8000; // Effekseerで画面に表示できる最大パーティクル数

	// ステージの数
	constexpr int kStageNum = 2;

	// ステージ番号
	constexpr int kTutorialStageNo = 0;	// チュートリアルステージ
	constexpr int kStage1No = 1;		// ステージ1
	constexpr int kStage2No = 2;		// ステージ2

	// 当たり判定の八分木分割のレベル
	constexpr uint32_t kOctreeLevel = 2;
	// ステージの最大値、最小値(ステージによって変わるため今後変更する可能性がある)
	constexpr float kMinStageSize = -180.0f;
	constexpr float kMaxStageSize = 180.0f;
}
