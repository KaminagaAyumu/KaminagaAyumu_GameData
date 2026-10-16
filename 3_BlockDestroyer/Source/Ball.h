#pragma once
#include "Vector2.h"
#include "Collision.h"
#include "Sound.h"
class Ball
{
public:
	// コンストラクタ
	Ball();
	// デストラクタ
	~Ball();

	void Init();	// 初期化
	void End();		// 終了処理
	void Update();	// 毎フレ－ムの更新処理
	void Draw();	// Update()での更新を踏まえて描画を行う
	void ReflectionX(); // 玉の反射
	void ReflectionY(); // 玉の反射
	void RandomReflection(float angle);

	// 当たり判定の情報取得用関数
	float GetPosX() const { return m_pos.X; }
	float GetPosY() const { return m_pos.Y; }
	Vector2 GetPos() const { return m_pos; }
	float GetRadius() const { return m_radius; }

	Collision GetCollider() const { return m_colCircle; }

	// 貫通弾かどうかの情報を取得
	bool GetIsPenetration() const { return m_isPenetration; }
	
	// ボールが存在しているかどうか
	bool IsExist() const { return m_isExist; }

	// ボールが何回反射したか
	int GetReflectionCount() const { return m_reflectionCount; }

	// プレイヤーと当たった際に、速度を変更する
	void HitPlayerTopDownChangeVelocity(Vector2 playerVec);
	void HitPlayerLeftRightChangeVelocity(Vector2 playerVec);
	// プレイヤーに当たった際に、めり込んでいたら押し出す
	void HitPlayerInOut(Vector2 playerPos, int state, float width, float height);

	// 強化カードを選択した際の処理
	void NewBallCreate(Vector2 pos); // 新しいボールを生成
	void ChangeMinSpeed(float speed);
	void ChangeMaxSpeed(float speed);
	void ChangeAccelSize(Vector2 accel);
	void SetPenetration(bool isPenetration) { m_isPenetration = isPenetration; }

private:
	// メンバ変数
	int m_soundHandle; // 効果音のハンドル
	int m_ballColor; // ボールの色
	Sound m_sound; // 効果音
	Vector2 m_pos; // 中心座標
	Vector2 m_velocity; // 速度
	float m_minSpeed; // 最小速度
	float m_maxSpeed; // 最大速度
	Vector2 m_accelSize; // 加速度の大きさ
	float m_radius; // 半径
	Collision m_colCircle;
	bool m_isPenetration; // 貫通弾になるかどうか
	bool m_isExist; // 存在しているかどうか
	int m_reflectionCount; // 反射回数

};

