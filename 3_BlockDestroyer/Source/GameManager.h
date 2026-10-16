#pragma once
#include "BallManager.h"
#include "Player.h"
#include "BlockManager.h"
#include "Sound.h"
#include "UI.h"
#include "DxLib.h"
#include "InputManager.h"
#include "Animation.h"
#include <array>
#include <string>

namespace
{
	constexpr int kAllCardNum = 6; // カードの総数(何も書いてないカードを含む)

	constexpr int kUpGradeNum = 5; // アップグレードの種類

	constexpr int kButtonNum = 4; // ボタンの数

	constexpr int kCardNum = 3; // 画面上のカードの数

}

enum class UpgradeType
{
	PlayerSizeUp,
	BallSpeedUp,
	BallIncrease,
	BallPenetration,
	BlockSpeedDown,

	UpgradeNum
};

/// <summary>
/// スコア、プレイヤーやボールの強化などを行うクラス
/// </summary>
class GameManager
{
public:
	// コンストラクタ
	GameManager(BallManager* ballManager, Player* player, BlockManager* blockManager);
	// デストラクタ
	~GameManager();

	/*メンバ関数*/
	void Init();	// 初期化
	void End();		// 終了処理
	void Update(InputManager& input);	// 毎フレ－ムの更新処理
	void Draw();	// Update()での更新を踏まえて描画を行う

	// スコア関連
	void AddScore();	// スコアを加算する
	float GetScore() const { return m_score; }

	// 強化処理関連
	void PlayerSizeUp();	// プレイヤーの拡大率を変更
	void BallSpeedUp();	// ボールの速度を上げる
	void BallIncrease();	// ボールの数を増やす
	void BallPenetration();	// ボールを貫通弾にする
	void BlockSpeedDown();	// ブロックが下に落ちてくるスピードを遅くする

	// シーンの状態関連
	bool IsGameOver() const;	// ゲームオーバーかどうかを返す
	bool IsGameClear(bool isClear); // ゲームクリアかどうかを返す
	bool IsGameClear() const;	// ゲームクリアかどうかを返す

private:
	/*メンバ変数*/
	// -------グラフィック、サウンド関連--------
	int m_fontHandle;	// フォントハンドル
	int m_fontBigHandle; // 大きいフォントハンドル
	int m_cardEffectHandle; // カードエフェクトの画像ハンドル
	int m_upgradeSeHandle;  // 強化抽選時のSEハンドル
	int m_selectUpgradeSeHandle; // 強化選択時のSEハンドル
	int m_progressOperatorHandle; // 進行度を表示する画像ハンドル
	int m_persentHandle; // 進行度の%を表示する画像ハンドル
	std::array<int, kAllCardNum>m_cardHandles; // 強化カードの画像ハンドル
	int m_upgradeUIHandle; // アップグレード可能時にカードの上に表示する画像
	std::array<int, kButtonNum> m_buttonGraphHandles; // ボタンの画像ハンドル
	std::array<int, kUpGradeNum> m_upgradeOperatorHandle; // 何をアップグレードしたかを表示する画像
	std::array<int, kUpGradeNum> m_upgradeOperatorBordHandle; // アップグレードが何が行われているかを看板に表示する用の画像

	Player* m_player;	// プレイヤーのインスタンス
	BallManager* m_ballManager;	// ボールのインスタンス
	BlockManager* m_blockManager;	// ブロックマネージャーのインスタンス
	Sound m_sound;	// 効果音関連クラスのインスタンス
	UI m_ui; // UI関連クラスのインスタンス
	Animation m_animation; // アニメーション関連クラスのインスタンス

	// -------ゲームの状態関連--------
	int m_frameCount;	// フレームカウント
	int m_grAlpha; // 画像の透明度
	int m_fadeDirection; // フェードの向き

	float m_score;	// 演出用スコア
	float m_currentScore;	// 現在のスコア
	float m_prevScore; // 前のフレームのスコア
	float m_upgradeScoreNorma; // スコアによる強化のカウント

	int m_totalBlockDestroyCount; // 総ブロック破壊数

	int m_prevBallReflectCount;	// 前のフレームのボールの反射回数
	int m_currentBallReflectCount;	// ボールの反射回数
	int m_reflectUpgradeCounter; // 反射回数による強化のカウント
	int m_totalProgress; // 総進行度

	int m_upgradeThereshold; // 強化の条件

	// 強化関係
	std::array<UpgradeType, static_cast<int>(UpgradeType::UpgradeNum)> m_upgrades;	// 強化の種類
	std::array<UpgradeType, kCardNum> m_selectUpgrades; // 選択可能な強化の種類
	int m_upgradeStock;	// 強化のストック数 
	int m_ballSpeedUpCount;	// ボールの速度アップの回数
	int m_upgradeNum; // 強化選択時の番号

	bool m_isUpgradeNow; // 強化が行われた瞬間のフラグ
	bool m_isShuffleUpgrades; // 強化抽選用のフラグ
	bool m_isResetUpgradeType; // 強化状態リセット用のフラグ
	bool m_isPlayerUpgraded;	// プレイヤーが強化されているかどうか
	bool m_isBallUpgraded;	// ボールが強化されているかどうか
	bool m_isCanCardPressed;	// カードが押されたかどうか
	
	void ShuffleUpgrades();	// 強化抽選
	void ApplyUpgrade(UpgradeType upgrade);	// 強化を適用する
	void ResetUpgradeType(); // 強化状態のリセット

	int m_cardBlendMode; // カードの状態を管理するブレンドモード

};

