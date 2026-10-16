#pragma once
#include "Vector2.h"
#include "DxLib.h"
#include "Collision.h"
#include "Block.h"
#include "Geometry.h"
#include <vector>

class BlockManager
{
private:
	/*メンバ変数*/
	float m_frameCounter; // フレームをカウントする変数

	float m_fallTime; // ブロックの落下時間

	float m_fallChangeTime; // ブロックの落下速度を変える時間のカウント用
	float m_fallChangeMaxTime; // ブロックの落下速度を変える制限時間

	bool m_isFallChange; // ブロックの落下速度を変えるかどうかのフラグ

	bool m_isAddScore; // スコアを加算するかどうかのフラグ
	int m_scoreAddNum; // スコアを加算する回数(1フレームに何回か)


	// ブロック配列の作成
	std::vector<Block> m_blocks;
	
	

public:
	/*コンストラクタ*/
	BlockManager();

	/*デストラクタ*/
	~BlockManager();

	/*メンバ関数*/
	// ブロックの初回作成
	void BlockFirstCreate(int yoko, int tate, int drawHandle,int seHandle, int effectHandle);
	// ブロックの描画
	void Draw();
	// ブロックの更新処理
	void Update(); 
	
	// ブロックの落下速度の変更
	void ChangeFallTime(float fallTime);

	// ブロックを取得
	const Block& GetBlock(int index) const;
	Block& GetBlock(int index);

	// ブロックの残りの数を取得
	int GetBlockNum();

	Vector2 GetBlockPos(int index) const;

	// スコア更新処理関係
	// スコア加算関係の状態更新
	void SetScoreUpdate(int scoreAddNum, bool isAddScore);

	// ゲッター用
	int GetScoreAddNum() const { return m_scoreAddNum; }
	bool IsAddScore() const { return m_isAddScore; }
	bool IsFallChange() const { return m_isFallChange; }
	float GetFallChangeTime() const { return m_fallChangeTime; }
	float GetFallChangeTimeRate() const { return 1.0f - (m_fallChangeTime / m_fallChangeMaxTime); }

	int GetBlockArraySize() const { return m_blocks.size(); }
	float GetBlockFallSpeed() const { return m_fallTime; }
	// 加算処理が終わった後に呼ぶ
	void ResetScoreUpdate();
};

