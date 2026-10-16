#include"DxLib.h"
#include "Player.h"
#include "Game.h"

namespace
{
	constexpr float kStartX = 640.0f;
	constexpr float kStartY = 600.0f;

	constexpr float kGraphWidth = 128;
	constexpr float kGraphHeight = 20;

	constexpr float kFirstSpeed = 3.0f;
	constexpr float kMaxSpeed = 10.0f;
	
	constexpr float kAccel = 0.30f;

}

Player::Player() : // コンストラクタ
	m_handle(-1),
	m_pos(),
	m_colRect(),
	m_velocity(),
	m_initVelocity(),
	m_maxSpeed(0),
	m_accelSize(0.0f),
	m_extRateX(0.0f),
	m_extRateY(0.0f),
	m_extTime(0.0f),
	m_maxExtTime(0.0f),
	m_isExtend(false)
{
	
}

Player::~Player() // デストラクタ
{

}

void Player::Init(int handle) // 初期化
{
	m_handle = handle;
	m_pos.X = kStartX;
	m_pos.Y = kStartY;
	m_velocity = Vector2(0,0);
	m_initVelocity = Vector2(kFirstSpeed, 0);
	m_maxSpeed = kMaxSpeed;
	m_accelSize = kAccel;
	m_extRateX = 1.0f; // 横の拡張率
	m_extRateY = 1.0f; // 縦の拡張率
	m_extTime = 0.0f;
	m_maxExtTime = 10.0f;
	m_isExtend = false;
}

void Player::End()
{

}

void Player::Update(InputManager& input)
{
	PosCorrection(); // プレイヤーのバーがゲーム画面外に出ないようにする
	
	/*速度の変更*/

	// 左に入力されたとき
	if(input.GetPad(PAD_INPUT_LEFT) || input.GetKey(KEY_INPUT_LEFT))
	{
		// 左に移動
		if (m_velocity.X == 0)
		{
			m_velocity.X = -m_initVelocity.X;
		}
		m_velocity.X -= m_accelSize;
	}
	else if(input.GetPadUp(PAD_INPUT_LEFT) || input.GetKeyUp(KEY_INPUT_LEFT))
	{
		// 離したら速度を0にする
		m_velocity.X = 0;
	}
	// 右に入力されたとき
	if(input.GetPad(PAD_INPUT_RIGHT) || input.GetKey(KEY_INPUT_RIGHT))
	{
		// 右に移動
		if (m_velocity.X == 0)
		{
			m_velocity.X = m_initVelocity.X;
		}
		m_velocity.X += m_accelSize;
	}
	else if(input.GetPadUp(PAD_INPUT_RIGHT) || input.GetKeyUp(KEY_INPUT_RIGHT))
	{
		// 離したら速度を0にする
		m_velocity.X = 0;
	}

	/*速度制限*/
	if (m_velocity.Magnitude() > (m_maxSpeed * m_maxSpeed))
	{
		// 正規化して向いてる方向を取得している
		m_velocity = m_velocity.Nomalize() * m_maxSpeed;
	}



	// 状態の更新
	if (m_isExtend)
	{
		m_extTime += 1.0f / 60.0f; // 1フレーム分の時間を加算
		if (m_extTime >= m_maxExtTime)
		{
			//printfDx("プレイヤー拡張の終了\n");
			m_isExtend = false;
			m_extRateX = 1.0f; // 横の拡張率を元に戻す
			m_extRateY = 1.0f; // 縦の拡張率を元に戻す
			m_extTime = 0.0f; // 時間をリセット
		}
	}

	// 位置の更新
	m_pos = m_pos + m_velocity;


	// 当たり判定の表示用関数
	m_colRect.SetCenter(m_pos.X, m_pos.Y, kGraphWidth * m_extRateX, kGraphHeight * m_extRateY);
}

void Player::Draw()
{
	DrawRotaGraph3(m_pos.X, m_pos.Y, kGraphWidth / 2, kGraphHeight / 2, m_extRateX, m_extRateY, 0, m_handle, false);
	//m_colRect.DrawBoxCollider(0xff0000);
}

void Player::ChangeGraphExtRate(float extRateX, float extRateY)
{
	m_extRateX = extRateX; // 横の拡張率
	m_extRateY = extRateY; // 縦の拡張率
	m_isExtend = true; // 拡張状態にする
}

Vector2 Player::GetGraphSize() const
{
	return Vector2((kGraphWidth * m_extRateX), (kGraphHeight * m_extRateY));
}

void Player::ResetSpeed()
{
	m_velocity.X = 0.0f;
}

void Player::PosCorrection()
{
	// 移動できる範囲の制限
	if (m_pos.X >= Game::kGameRightWidth - (kGraphWidth * m_extRateX) / 2)
	{
		m_pos.X = Game::kGameRightWidth - (kGraphWidth * m_extRateX) / 2;
		m_velocity.X = 0;
	}
	if (m_pos.X <= Game::kGameLeftWidth + (kGraphWidth * m_extRateX) / 2)
	{
		m_pos.X = Game::kGameLeftWidth + (kGraphWidth * m_extRateX) / 2;
		m_velocity.X = 0;
	}
}

Vector2 Player::GetLeftTop() const
{
	return Vector2(m_pos.X - (kGraphWidth * m_extRateX) / 2, m_pos.Y - (kGraphHeight * m_extRateY) / 2);
}

Vector2 Player::GetRightTop() const
{
	return Vector2(m_pos.X + (kGraphWidth * m_extRateX) / 2, m_pos.Y - (kGraphHeight * m_extRateY) / 2);
}

Vector2 Player::GetLeftBottom() const
{
	return Vector2(m_pos.X - (kGraphWidth * m_extRateX) / 2, m_pos.Y + (kGraphHeight * m_extRateY) / 2);
}

Vector2 Player::GetRightBottom() const
{
	return Vector2(m_pos.X + (kGraphWidth * m_extRateX) / 2, m_pos.Y + (kGraphHeight * m_extRateY) / 2);
}

Vector2 Player::LeftVec() const
{
	// 右回りなので左上 - 左下
	return Vector2(GetLeftTop() - GetLeftBottom());
}

Vector2 Player::RightVec() const
{
	// 右回りなので右下 - 右上
	return Vector2(GetRightBottom() - GetRightTop());
}

Vector2 Player::TopVec() const
{
	// 右回りなので右上 - 左上
	return Vector2(GetRightTop() - GetLeftTop());
}
Vector2 Player::BottomVec() const
{
	// 右回りなので左下 - 右下 
	return Vector2(GetLeftBottom() - GetRightBottom());
}
Vector2 Player::CenterVec() const
{
	return Vector2(GetPos() - Vector2(m_pos.X,m_pos.Y - kGraphHeight));
}



