#include "Ball.h"
#include "DxLib.h"
#include "Game.h"
#include <cmath>

namespace
{
	constexpr float kStartX = 640.0f;
	constexpr float kStartY = 585.0f;

	constexpr float kGraphWidth = 2.0f;
	constexpr float kGraphHeight = 1.0f;
	constexpr float kGraphRadius = 4.0f;

	constexpr float kFirstSpeed = 4.0f;
	constexpr float kFirstMinSpeed = 4.0f;
	constexpr float kFirstMaxSpeed = 7.0f;
	constexpr float kAccel = 0.01f;

	constexpr float kPosMargin = 10.0f;
}

Ball::Ball() : 
	m_soundHandle(-1),
	m_ballColor(0),
	m_minSpeed(0.0f),
	m_maxSpeed(0.0f),
	m_radius(0.0f),
	m_isPenetration(false),
	m_isExist(true),
	m_reflectionCount(0)
{
	
}

Ball::~Ball()
{

}

void Ball::Init()
{
	m_soundHandle = LoadSoundMem("data/ball_Se.mp3");
	m_sound.Init(m_soundHandle);
	m_ballColor = GetColor(255, 255, 0); // 黄色
	m_pos.X = kStartX;
	m_pos.Y = kStartY;
	m_velocity.X = kFirstSpeed;
//	m_velocity.X = 0;
	m_velocity.Y = -kFirstSpeed;
//	m_velocity.Y = 0;
	m_minSpeed = kFirstMinSpeed;
	m_maxSpeed = kFirstMaxSpeed;
	m_accelSize = Vector2(kAccel,kAccel);
//	m_accelSize = Vector2(0.0f, 0.0f);
	m_radius = kGraphRadius;
	m_isPenetration = false;
	m_isExist = true;
	m_reflectionCount = 0;
}

void Ball::End()
{
	m_soundHandle = DeleteSoundMem(m_soundHandle);
}

void Ball::Update()
{
	// ボールが存在していない場合は、何もしない
	if (!m_isExist)
	{
		return;
	}
	if (m_velocity.X < 0) // 左に移動しているとき
	{
		if (m_pos.X <= Game::kGameLeftWidth)
		{
			m_pos.X = Game::kGameLeftWidth;
			ReflectionX();
		}
	}
	if (m_velocity.X > 0) // 右に移動しているとき
	{
		if (m_pos.X >= Game::kGameRightWidth)
		{
			m_pos.X = Game::kGameRightWidth;
			ReflectionX();
		}
	}
	if (m_velocity.Y < 0) // 上に移動しているとき
	{
		if (m_pos.Y <= Game::kGameUpHeight)
		{
			m_pos.Y = Game::kGameUpHeight;
			ReflectionY();
		}
	}
	if (m_velocity.Y > 0) // 下に移動しているとき
	{
		if (m_pos.Y >= Game::kGameDownHeight)
		{
			m_pos.Y = Game::kGameDownHeight;
			m_velocity.Y *= -1.0f;
			// ボールが下に落ちたら消す
			//printfDx("ボールが落ちた\n");
			m_isExist = false; // ボールの存在を無くす
		}
	}

	// 加速度の計算
	m_accelSize = m_velocity.Nomalize() * m_accelSize.Length();

	// 速度の更新
	m_velocity = m_velocity + m_accelSize;
	
	/*速度制限*/
	if (m_velocity.Magnitude() > (m_maxSpeed * m_maxSpeed))
	{
		// 正規化して向いてる方向を取得している
		m_velocity = m_velocity.Nomalize() * m_maxSpeed;
	}

	if(Geometry::Abs(m_velocity.X) < m_minSpeed) // X方向の速度補正 最小速度未満にはならないようにする
	{
		if (m_velocity.X < 0)
		{
			m_velocity.X = -m_minSpeed;
		}
		else
		{
			m_velocity.X = m_minSpeed;
		}
	}
	if (Geometry::Abs(m_velocity.Y) < m_minSpeed) // Y方向の速度補正 最小速度未満にはならないようにする
	{
		if (m_velocity.Y < 0)
		{
			m_velocity.Y = -m_minSpeed;
		}
		else
		{
			m_velocity.Y = m_minSpeed;
		}
	}

	// 速度の更新
	m_pos = m_pos + m_velocity;
	

	/*ボールのデバッグ用*/
	// ボールの当たり判定の表示
	//m_colCircle.SetCircle(m_pos.X, m_pos.Y, m_radius);

	// ボールのスピードの表示
	//printfDx("X: %f Y: %f\n", m_velocity.X,m_velocity.Y);

}

void Ball::Draw()
{
	if (!m_isExist)
	{
		return; // ボールが存在していない場合は、描画しない
	}
	DrawCircle(static_cast<int>(m_pos.X), static_cast<int>(m_pos.Y), static_cast<int>(m_radius), m_ballColor, true);
	//m_colCircle.DrawCircleCollider(0x00ff00);
}

void Ball::ReflectionX()
{
	m_sound.Play();
	m_reflectionCount++;
	m_velocity.X *= -1.0f;
}

void Ball::ReflectionY()
{
	m_sound.Play();
	m_reflectionCount++;
	m_velocity.Y *= -1.0f;
}

void Ball::RandomReflection(float angle)
{
	m_sound.Play();
	m_reflectionCount++;
	m_velocity.X = m_velocity.X * cos(angle);
	m_velocity.Y *= -1.0f;
}

void Ball::HitPlayerTopDownChangeVelocity(Vector2 playerVec)
{
	// 効果音を鳴らす
	m_sound.Play();

	// 反射回数を足す
	m_reflectionCount++;

	// ボールとプレイヤーの速度を足す
	Vector2 ans = m_velocity + playerVec;

	// y方向だけ反転
	ans.Y = -ans.Y;

	// 大きさを調整
	ans = ans.Nomalize() * m_velocity.Length();

	// 速度の更新
	m_velocity = ans;
}

void Ball::HitPlayerLeftRightChangeVelocity(Vector2 playerVec)
{
	// 効果音を鳴らす
	m_sound.Play();

	// 反射回数を足す
	m_reflectionCount++;

	// ボールとプレイヤーの速度を足す
	Vector2 ans = m_velocity + playerVec;

	// x方向だけ反転
	ans.X = -ans.X;

	// 大きさを調整
	ans = ans.Nomalize() * m_velocity.Length();

	// 速度の更新
	m_velocity = ans;
}

void Ball::HitPlayerInOut(Vector2 playerPos, int state, float width, float height)
{
	// 左辺が一番近い時
	if (state == 0)
	{
		m_pos.X = playerPos.X - width / 2.0f - m_radius;
	}
	// 上辺が一番近い時
	else if (state == 1)
	{
		m_pos.Y = playerPos.Y - height / 2.0f - m_radius;
	}
	// 右辺が一番近い時
	else if (state == 2)
	{
		m_pos.X = playerPos.X + width / 2.0f + m_radius;
	}
	// 下辺が一番近い時
	else if (state == 3)
	{
		m_pos.Y = playerPos.Y - height + m_radius;
	}
}

void Ball::NewBallCreate(Vector2 pos)
{
	m_soundHandle = LoadSoundMem("data/ball_Se.mp3");
	m_sound.Init(m_soundHandle);
	m_ballColor = GetColor(255, 255, 0); // 黄色
	m_pos.X = pos.X;
	m_pos.Y = pos.Y - kPosMargin;
	m_velocity.X = kFirstSpeed;
	m_velocity.Y = -kFirstSpeed;
	m_minSpeed = kFirstMinSpeed;
	m_maxSpeed = kFirstMaxSpeed;
	m_accelSize = Vector2(kAccel, kAccel);
	m_radius = kGraphRadius;
	m_isPenetration = false;
	m_isExist = true;
	m_reflectionCount = 0;
}

void Ball::ChangeMinSpeed(float speed)
{
	m_minSpeed = speed;
}

void Ball::ChangeMaxSpeed(float speed)
{
	m_maxSpeed += speed;
	m_ballColor = GetColor(255, static_cast<int>(m_maxSpeed) * 10, 0); // スピードに応じて色が変わる
}

void Ball::ChangeAccelSize(Vector2 accel)
{
	m_accelSize = accel;
}
