#pragma once
#include "Vector2.h"
#include "Collision.h"
#include "Geometry.h"
#include "InputManager.h"

class Player
{
public:
	// コンストラクタ
	Player();
	// デストラクタ
	~Player();

	// メンバ関数
	void Init(int handle);	// 初期化
	void End();		// 終了処理
	void Update(InputManager& input);	// 毎フレ－ムの更新処理	キャラクターの移動など
	void Draw();	// Update()での更新を踏まえて描画を行う

	// プレイヤーの強化用関数
	void ChangeGraphExtRate(float extRateX, float extRateY); // グラフィックの拡大率を変更する

	// 当たり判定の情報取得用関数
	float GetPosX() const { return m_pos.X; }
	float GetPosY() const { return m_pos.Y; }
	Vector2 GetPos() const { return m_pos; }

	Vector2 GetVelocity() const { return m_velocity; }

	Vector2 GetGraphSize() const; // グラフィックのサイズを取得する

	void ResetSpeed(); // 速度を0にリセットする
	void PosCorrection(); // 座標補正用

	float GetExtendTime() const { return m_maxExtTime - m_extTime; }
	float GetExtendTimeRate() const { return (m_maxExtTime - m_extTime) / m_maxExtTime; }
	bool IsExtend() const { return m_isExtend; }

	// 角の座標
	Vector2 GetLeftTop() const;
	Vector2 GetRightTop() const;
	Vector2 GetLeftBottom() const;
	Vector2 GetRightBottom() const;

	// 左辺のベクトル
	Vector2 LeftVec() const;
	// 右辺のベクトル
	Vector2 RightVec() const;
	// 上辺のベクトル
	Vector2 TopVec() const;
	// 下辺のベクトル
	Vector2 BottomVec() const;

	// 真ん中からのベクトル
	Vector2 CenterVec() const;

	Collision GetCollider() const { return m_colRect; }

private: 
	// メンバ変数
	int m_handle; // グラフィックハンドル
	Vector2 m_pos;
	Collision m_colRect;

	Vector2 m_velocity; // 速度
	Vector2 m_initVelocity; // 初期速度
	float m_maxSpeed; // 最大速度
	float m_accelSize; // 加速度の大きさ
	float m_extRateX; // 横方向の拡大率 extention
	float m_extRateY; // 縦方向の拡大率

	float m_extTime; // 拡大時間
	float m_maxExtTime; // 最大拡大時間
	bool m_isExtend; // 拡大中かどうか

};