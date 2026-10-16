#pragma once
#include "Ball.h"
#include "Player.h"
#include "BlockManager.h"
#include <vector>


class BallManager
{
public:
	/*コンストラクタ*/
	BallManager();
	/*デストラクタ*/
	~BallManager();


	/*メンバ関数*/
	void Init(); // 初期化
	void End(); // 終了処理
	void Update(); // 更新処理
	void Draw(); // 描画処理

	/// <summary>
	/// ボールを追加する関数
	/// </summary>
	/// <param name="ball">追加するボール</param>
	void AddBall(const Ball& ball, Vector2 pos);

	void BallPenetation(); // ボールを貫通弾にする
	bool IsPenetration() const { return m_isPenetration; }
	float GetPenetrationTime() const { return m_penetrationTime; }
	float GetPenetrationTimeRate() const { return m_penetrationTime / m_maxPenetrationTime; }

	// ボールを取得
	Ball& GetBall(int index);
	const Ball& GetBall(int index) const;

	// 存在しているボールの数を取得
	int GetBallNum();

	// ボールの配列の大きさを取得
	int GetBallArraySize() const;

	int GetBallReflectionCount() const;

	/*当たり判定関係*/
	bool CheckHitPlayer(Player& player);

	int CheckHitBlock(BlockManager& blockManager);

private:
	std::vector<Ball> m_balls; // ボールの生成管理

	bool m_isPenetration; // 貫通弾かどうか
	float m_maxPenetrationTime; // 貫通弾の最大時間
	float m_penetrationTime; // 貫通弾の時間

};

