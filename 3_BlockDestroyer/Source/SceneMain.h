#pragma once
#include "SceneBase.h"
#include "Player.h"
//#include "Block.h"
#include "BlockManager.h"
#include "GameManager.h"
#include "Ball.h"
#include "BallManager.h"
#include "UI.h"
#include "Game.h"
#include "Vector2.h"
#include <vector>

//struct Block
//{
//	
//};

class SceneMain : public SceneBase
{
public:
	// コンストラクタ
	SceneMain();
	// デストラクタ
	virtual ~SceneMain();

	// メンバ関数
	virtual void Init() override;	// 初期化
	virtual void End() override;		// 終了処理
	virtual void Update() override;	// 毎フレ－ムの更新処理	キャラクターの移動など
	virtual void Draw() override;	// Update()での更新を踏まえて描画を行う

	// シーン固有の更新処理
	void PauseUpdate(); // ポーズ時の更新処理
	void ClearUpdate(); // クリア時の更新処理
	void OverUpdate(); // ゲームオーバー時の更新処理

	bool IsSceneEnd() const { return m_isEnd; }	// 終了フラグの取得

private:
	int m_resultFontHandle; // 結果を表示するフォントのハンドル
	int m_defaultFontHandle; // 通常サイズのフォントのハンドル
	int m_bgGraphHandle; // 背景画像のハンドル
	int m_playerGraphHandle;
	int m_blockGraphHandle;
	int m_startUIHandle;
	int m_scoreUIHandle; // スコアUIのハンドル
	int m_clearUIHandle;
	int m_gameoverUIHandle;
	int m_pauseUIHandle;
	int m_retryUIHandle;
	int m_titleUIHandle;
	int m_restartUIHandle;
	int m_blockSeHandle;
	int m_blockEffectGraphHandle;
	int m_countdownSeHandle;
	int m_startSeHandle;
	int m_pauseSeHandle; // ポーズ時の効果音のハンドル
	int m_clearBgmHandle;
	int m_gameoverBgmHandle;

	int m_frameCount; // フレームカウント用
	int m_countdown; // カウントダウンの数字
	int m_prevCountdown; // 前のフレームのカウントダウンの数字
	int m_startDispCount; // スタートの文字を表示する時間
	int m_startFadeAlpha; // スタートの文字の透明度 255が不透明　0で透明

	int m_clearScore; // クリア時用スコア

	// ぼかし用のグラフハンドル
	int m_blurGraphHandle;

	bool m_isStartSePlayed; // スタートのSEが鳴ったかどうか
	bool m_isClearBgmPlayed;
	bool m_isGameoverBgmPlayed;
	bool m_isPause; // ポーズ中かどうか
	bool m_isCardSelectPause; // カード選択中断しているかどうか

	Vector2 m_cursorPos; // カーソルの位置

	//	int m_handle[4];
	Player* m_player;

	BallManager* m_ballManager;
	BlockManager* m_blockManager;
	GameManager* m_gManager;

	//Block m_blockTbl[Game::kGameDownHeight / kBlockHeight][(Game::kGameRightWidth - Game::kGameLeftWidth) / kBlockWidth];
//	Block m_block;

};

