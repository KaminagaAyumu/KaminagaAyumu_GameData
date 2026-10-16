#pragma once
#include "Vector2.h"
#include "Geometry.h"

class Collision
{
public:
	Collision();
	~Collision();

	void Init();	// 初期化
	void End();		// 終了処理
	void Update();	// 毎フレ－ムの更新処理
	void DrawBoxCollider(unsigned int color);	// 矩形の当たり判定を描画
	void DrawCircleCollider(unsigned int color);

	// セッター

	/// <summary>
	/// 中心座標のセット
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void SetCenter(float x, float y, float width, float height);
	
	void SetRect(float x, float y, float width, float height);

	void SetCircle(float x, float y, float radius);

	// ゲッター

	Vector2 GetCenter() const;

	float GetWidth() const;

	float GetHeight() const;

	float GetLeft() const { return m_left; }
	float GetTop() const { return m_top; }
	float GetRight() const { return m_right; }
	float GetBottom() const { return m_bottom; }
	float GetRadius() const { return m_radius; }

	/// <summary>
	/// 矩形同士の当たり判定
	/// </summary>
	/// <param name="rect"></param>
	/// <returns></returns>
	bool IsCollisionRectToRect(const Collision& rect);

	/// <summary>
	/// 円と線の当たり判定
	/// </summary>
	/// <param name="circle">円の判定</param>
	/// <returns></returns>
	bool IsCollisionLineToCircle(const Collision& circle);

	/// <summary>
	/// 矩形と円の当たり判定
	/// </summary>
	/// <param name="circle"></param>
	/// <returns></returns>
	bool IsCollisionRectToCircle(const Collision& circle);


private:


	float m_left;
	float m_top;
	float m_right;
	float m_bottom;
	float m_radius;

};

