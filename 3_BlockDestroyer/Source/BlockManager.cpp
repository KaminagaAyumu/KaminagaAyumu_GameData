#include "BlockManager.h"
#include "DxLib.h"
#include "Game.h"
#include "Block.h"

namespace
{
	constexpr int kBlockWidth = 80;
	constexpr int kBlockHeight = 20;
	constexpr int kBlockRectX = kBlockWidth / 2;
	constexpr int kBlockRectY = kBlockHeight / 2;
	constexpr int kBlockDownHeight = 25; // ブロックが一定時間ごとに下にずれる座標
	constexpr float kFirstFallChangeMaxTime = 600.0f; // 初期状態のブロック落下強化後の速度
	constexpr float kEffectWidth = 128.0f; // エフェクトの幅
	constexpr float kEffectHeight = 128.0f; // エフェクトの高さ
	constexpr float kBlockDefaultFallTime = 180.0f; // ブロックの落下時間
}



BlockManager::BlockManager()
{
	m_frameCounter = 0.0f;
	m_fallTime = kBlockDefaultFallTime;
	m_fallChangeTime = 0.0f;
	m_fallChangeMaxTime = kFirstFallChangeMaxTime;
	m_isFallChange = false;
	m_isAddScore = false;
	m_scoreAddNum = 0; // スコア加算数を初期化
}

BlockManager::~BlockManager()
{
}


void BlockManager::ResetScoreUpdate()
{
	m_scoreAddNum = 0; // スコア加算数をリセット
	m_isAddScore = false; // スコア加算フラグをリセット
}

void BlockManager::BlockFirstCreate(int yoko, int tate, int drawHandle,int seHandle, int effectHandle)
{
	int left = Game::kGameLeftWidth + kBlockRectX;
	int top = Game::kGameUpHeight + kBlockRectY;

	for (int s = 0; s < tate; s++)
	{
		for (int i = 0; i < yoko; i++)
		{
			// 追加するブロックの作成
			Block temp;// ブロックの作成
			Vector2 pos(left + i * kBlockWidth, top + kBlockHeight * s);// 座標の設定
			Vector2 rect(kBlockRectX, kBlockRectY);// 幅の設定
			Vector2 effectAnimSize(kEffectWidth, kEffectHeight);// エフェクトのサイズ設定
			int handle = drawHandle;// 画像ハンドルの設定
			int sHandle = seHandle;// SEハンドルの設定
			int effect = effectHandle;// エフェクトハンドルの設定
			temp.Init(pos, rect,effectAnimSize, handle, sHandle, effect);// 生成

			// ブロック配列への追加
			m_blocks.push_back(temp);
		}
	}


}

void BlockManager::Draw()
{
	//vectorの要素数を取得
	int num = m_blocks.size();
	//要素数の分だけ描画
	for (int i = 0; i < num; i++)
	{
		m_blocks[i].Draw();
	}

}

void BlockManager::Update()
{
	/*当たり判定*/
	//vectorの要素数を取得
	int num = m_blocks.size();

	m_frameCounter++;//フレームをカウント

	for (int i = 0; i < num; i++)
	{
		m_blocks[i].Animation.Update(); // アニメーションの更新
	}

	if (m_frameCounter >= m_fallTime) // 一定時間経過したらブロックを落とす
	{
		// 要素数の数だけy座標をずらす
		for (int i = 0; i < num; i++)
		{
			if (!m_blocks[i].IsExist) { continue; }
			m_blocks[i].Pos.Y += kBlockDownHeight; // ブロック1つ分下に移動
			m_blocks[i].EffectPos.Y += kBlockDownHeight; // エフェクトの位置も同様に下に移動
			m_blocks[i].Animation.SetPos(m_blocks[i].EffectPos); // アニメーションの位置を更新
		}
		m_frameCounter = 0.0f; // フレームカウンターをリセット
	}
	if (m_isFallChange)
	{
		m_fallChangeTime += 1.0f; // 落下時間変更のカウントを進める
		if (m_fallChangeTime >= m_fallChangeMaxTime) // 一定時間経過したら落下時間を変更
		{
			m_fallTime = kBlockDefaultFallTime; // ブロックの落下時間を変更
			m_fallChangeTime = 0.0f; // 落下時間変更のカウントをリセット
			m_isFallChange = false; // 落下時間が変更されたことを示すフラグを倒す
		}
	}
	

}

void BlockManager::ChangeFallTime(float fallTime)
{
	m_fallTime = fallTime; // ブロックの落下時間を変更
	m_fallChangeTime = 0.0f; // 落下時間変更のカウントをリセット
	m_isFallChange = true; // 落下時間が変更されたことを示すフラグを立てる
}

const Block& BlockManager::GetBlock(int index) const
{
	return m_blocks[index]; // 指定したインデックスのブロックを返す
}

Block& BlockManager::GetBlock(int index)
{
	return m_blocks[index]; // 指定したインデックスのブロックを返す
}

int BlockManager::GetBlockNum()
{
	//vectorの要素数を取得
	int num = m_blocks.size();

	int blockCount = 0; // 存在するブロックの数をカウント
	// ブロックの数を取得
	for (int i = 0; i < num; i++)
	{
		if (!m_blocks[i].IsExist) { continue; }
		blockCount++;
	}
	return blockCount;
}

Vector2 BlockManager::GetBlockPos(int index) const
{
	return m_blocks[index].Pos; // 指定したインデックスのブロックの位置を返す
}

void BlockManager::SetScoreUpdate(int scoreAddNum, bool isAddScore)
{
	m_scoreAddNum = scoreAddNum; // スコア加算数を設定
	m_isAddScore = isAddScore; // スコア加算フラグを設定
}
