#pragma once
#include "BallManager.h"

BallManager::BallManager() :
	m_isPenetration(false),
	m_maxPenetrationTime(0.0f),
	m_penetrationTime(0.0f)
{
}

BallManager::~BallManager()
{
}

void BallManager::Init()
{
	m_isPenetration = false;
	m_maxPenetrationTime = 5.0f;
	m_penetrationTime = 0.0f;
	// 最初のボールを生成
	Ball firstBall;

	// ボールに追加
	m_balls.push_back(firstBall);

	// ボールの配列の数を取得
	int num = m_balls.size();
	// ボールの数だけ初期化
	for (int i = 0; i < num; i++)
	{
		m_balls[i].Init();
	}
}
void BallManager::End()
{
	// ボールの配列の数を取得
	int num = m_balls.size();
	for (int i = 0; i < num; i++)
	{
		m_balls[i].End();
	}
}
void BallManager::Update()
{
	// ボールの配列の数を取得
	int num = m_balls.size();
	for (int i = 0; i < num; i++)
	{
		if (!m_balls[i].IsExist())
		{
			continue; // ボールが存在しない場合はスキップ
		}
		m_balls[i].Update();
	}
	// ボールの状態の更新
	if (m_isPenetration)
	{
		m_penetrationTime -= 1.0f / 60.0f;
		if (m_penetrationTime <= 0.0f)
		{
			m_penetrationTime = 0.0f;
			for (int i = 0; i < m_balls.size(); i++)
			{
				if (!m_balls[i].IsExist())
				{
					continue; // ボールが存在しない場合はスキップ
				}
				m_balls[i].SetPenetration(false);
			}
			m_isPenetration = false;
		}
	}
}
void BallManager::Draw()
{
	// ボールの配列の数を取得
	int num = m_balls.size();
	for (int i = 0; i < num; i++)
	{
		if (!m_balls[i].IsExist())
		{
			continue; // ボールが存在しない場合はスキップ
		}
		m_balls[i].Draw();
	}
}

void BallManager::AddBall(const Ball& ball, Vector2 pos)
{
	m_balls.push_back(ball);
	m_balls.back().NewBallCreate(pos); // 新しいボールを追加したら初期化
}

void BallManager::BallPenetation()
{
	m_isPenetration = true;
	m_penetrationTime = m_maxPenetrationTime;
	for (int i = 0; i < m_balls.size(); i++)
	{
		if (!m_balls[i].IsExist())
		{
			continue; // ボールが存在しない場合はスキップ
		}
		m_balls[i].SetPenetration(true);
	}
}

Ball& BallManager::GetBall(int index)
{
	return m_balls[index];
}

const Ball& BallManager::GetBall(int index) const
{
	return m_balls[index];
}

int BallManager::GetBallNum()
{
	int num = m_balls.size();

	int ballNum = 0;
	for (int i = 0; i < num; i++)
	{
		if (m_balls[i].IsExist())
		{
			ballNum++;
		}
	}
	return ballNum;
}

int BallManager::GetBallArraySize() const
{
	return m_balls.size();
}

int BallManager::GetBallReflectionCount() const
{
	int num = m_balls.size();
	int totalReflectionCount = 0;
	for (int i = 0; i < num; i++)
	{
		totalReflectionCount += m_balls[i].GetReflectionCount();
	}
	return totalReflectionCount;
}

bool BallManager::CheckHitPlayer(Player& player)
{
	int ballNum = m_balls.size();
	for (int i = 0; i < ballNum; i++)
	{
		Ball& ball = m_balls[i];
		if (!m_balls[i].IsExist())
		{
			continue; // ボールが存在しない場合はスキップ
		}

		Vector2 ballPos = ball.GetPos(); // ボールの位置を取得

		Vector2 leftBallVec = ballPos - player.GetLeftBottom();	// 辺の始点からボールまでのベクトル
		float leftCloss = Vector2::CrossZ(player.LeftVec(), leftBallVec); // 辺とボールの外積
		bool isLeftBarRight = (leftCloss >= 0); // ボールが辺の右側にいるか

		Vector2 topBallVec = ballPos - player.GetLeftTop();	// 辺の始点からボールまでのベクトル
		float topCloss = Vector2::CrossZ(player.TopVec(), topBallVec); // 辺とボールの外積
		bool isTopBarRight = (topCloss >= 0); // ボールが辺の右側にいるか

		Vector2 rightBallVec = ballPos - player.GetRightTop();	// 辺の始点からボールまでのベクトル
		float rightCloss = Vector2::CrossZ(player.RightVec(), rightBallVec); // 辺とボールの外積
		bool isRightBarRight = (rightCloss >= 0); // ボールが辺の右側にいるか

		Vector2 bottomBallVec = ballPos - player.GetRightBottom();	// 辺の始点からボールまでのベクトル
		float bottomCloss = Vector2::CrossZ(player.BottomVec(), bottomBallVec); // 辺とボールの外積
		bool isBottomBarRight = (bottomCloss >= 0); // ボールが辺の右側にいるか



		// 線分と点の距離の2乗を出す
		float delLeft = Geometry::DistancePointToSegmentMag(ball.GetPos(), player.GetLeftBottom(), player.GetLeftTop());
		float delTop = Geometry::DistancePointToSegmentMag(ball.GetPos(), player.GetLeftTop(), player.GetRightTop());
		float delRight = Geometry::DistancePointToSegmentMag(ball.GetPos(), player.GetRightTop(), player.GetRightBottom());
		float delBottom = Geometry::DistancePointToSegmentMag(ball.GetPos(), player.GetRightBottom(), player.GetLeftBottom());

		float minDel = delLeft;
		int state = 0;
		if (minDel > delTop) { minDel = delTop; state = 1; }
		if (minDel > delRight) { minDel = delRight; state = 2; }
		if (minDel > delBottom) { minDel = delBottom; state = 3; }
		// 半径の2乗よりも小さかったら当たっている
		if (minDel < ball.GetRadius() * ball.GetRadius())
		{
			if (isLeftBarRight && isTopBarRight && isRightBarRight && isBottomBarRight)
			{
				// ボールがバーの内側にいる場合
				ball.HitPlayerInOut(player.GetPos(), state, player.GetGraphSize().X, player.GetGraphSize().Y);
				//printfDx("めり込んでいます\n");
			}

			if (state == 0) { /*printfDx("バーの左辺に当たっている\n");*/ }
			if (state == 1) { /*printfDx("バーの上辺に当たっている\n");*/ }
			if (state == 2) { /*printfDx("バーの右辺に当たっている\n");*/ }
			if (state == 3) { /*printfDx("バーの下辺に当たっている\n"); */}

			if (state == 0 || state == 2)
			{
				ball.HitPlayerLeftRightChangeVelocity(player.GetVelocity());
			}
			if (state == 1 || state == 3)
			{
				ball.HitPlayerTopDownChangeVelocity(player.GetVelocity());
			}

			return true;
		}
	}
	return false;
}

int BallManager::CheckHitBlock(BlockManager& blockManager)
{
	int hitCount = 0; // この処理で当たったブロックの数をカウントする
	int ballNum = m_balls.size();
	int blockNum = blockManager.GetBlockArraySize();
	for (int i = 0; i < ballNum; i++)
	{
		Ball& ball = m_balls[i];
		if (!m_balls[i].IsExist())
		{
			continue; // ボールが存在しない場合はスキップ
		}
		for (int j = 0; j < blockNum; j++)
		{
			Block& block = blockManager.GetBlock(j);
			if (!block.IsExist)
			{
				continue; // ブロックが存在しない場合はスキップ
			}
			/*ボールの中点がブロックの内側にあるかの確認*/
			Vector2 ballPos = ball.GetPos();//ballの位置
			//左辺
			Vector2 leftBallVec = ballPos - block.GetLeftBottom();//開始からボールまでのベクトル
			float leftCross = Vector2::CrossZ(block.LeftVec(), leftBallVec);//辺とボールの外積
			bool isLeftBarRight = (leftCross <= 0);//ボールが辺の右側にいるか
			//上辺
			Vector2 topBallVec = ballPos - block.GetLeftTop();//開始からボールまでのベクトル
			float topCross = Vector2::CrossZ(block.TopVec(), topBallVec);//辺とボールの外積
			bool isTopBarRight = (topCross <= 0);//ボールが辺の右側にいるか
			//右辺
			Vector2 rightBallVec = ballPos - block.GetRightTop();//開始からボールまでのベクトル
			float rightCross = Vector2::CrossZ(block.RightVec(), rightBallVec);//辺とボールの外積
			bool isRightBarRight = (rightCross <= 0);//ボールが辺の右側にいるか
			//下辺
			Vector2 bottomBallVec = ballPos - block.GetRightBottom();//開始からボールまでのベクトル
			float bottomCross = Vector2::CrossZ(block.BottomVec(), bottomBallVec);//辺とボールの外積
			bool isBottomBarRight = (bottomCross <= 0);//ボールが辺の右側にいるか

			bool isHit = false; // 当たったかどうかのフラグ

			if (isLeftBarRight && isTopBarRight && isRightBarRight && isBottomBarRight) {
				//printfDx("内側です。\n");
				isHit = true; // ボールがブロックの内側にいる場合は当たったとする
				block.IsExist = false; // ブロックを消す
			}

			// ブロックのすべての線分との距離の２乗を出す
			float delLeft = Geometry::DistancePointToSegmentMag(ball.GetPos(), block.GetLeftBottom(), block.GetLeftTop());
			float delTop = Geometry::DistancePointToSegmentMag(ball.GetPos(), block.GetLeftTop(), block.GetRightTop());
			float delRight = Geometry::DistancePointToSegmentMag(ball.GetPos(), block.GetRightTop(), block.GetRightBottom());
			float delBottom = Geometry::DistancePointToSegmentMag(ball.GetPos(), block.GetRightBottom(), block.GetLeftBottom());

			//// 出した距離の２乗中で1番小さいものを出す
			float minDel = delLeft;
			int state = 0;
			if (minDel > delLeft) { minDel = delLeft; state = 0; }
			if (minDel > delTop) { minDel = delTop; state = 1; }
			if (minDel > delRight) { minDel = delRight;	state = 2; }
			if (minDel > delBottom) { minDel = delBottom; state = 3; }

			//半径の２乗よりも小さかったら当たっている
			if (minDel < ball.GetRadius() * ball.GetRadius())
			{
				isHit = true; // 当たったとする
				if (ball.GetIsPenetration())
				{
					block.IsExist = false; // 反射せずにブロックを消す
				}
				else
				{
					if (state == 0) { /*printfDx("左辺に当たっています\n");*/ ball.ReflectionX(); }
					if (state == 1) { /*printfDx("上辺に当たっています\n"); */ ball.ReflectionY(); }
					if (state == 2) { /*printfDx("右辺に当たっています\n"); */ ball.ReflectionX(); }
					if (state == 3) { /*printfDx("下辺に当たっています\n"); */ ball.ReflectionY(); }
					block.IsExist = false; // ブロックを消す
				}
			}
			if (isHit)
			{
				block.Animation.PlayOneShot(); // アニメーションを再生
				block.Sound.Play(); // ブロックが壊れる効果音を鳴らす
				hitCount++;
			}
		}
	}
	return hitCount;
}
