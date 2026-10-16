#pragma once

class Vector2
{
public:
	Vector2();
	Vector2(float inX, float inY);

	float x;
	float y;

	/// <summary>
	/// ベクトルの長さを返す
	/// </summary>
	/// <returns>ベクトルの長さ</returns>
	float Length();
	/// <summary>
	/// ベクトルの2乗の長さを返す
	/// </summary>
	/// <returns>ベクトルの2乗の長さ</returns>
	float SqrLength();

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	void Normalize();

	/// <summary>
	/// ベクトルの値を設定する
	/// </summary>
	/// <param name="newX">設定するX成分</param>
	/// <param name="newY">設定するY成分</param>
	void Set(float newX, float newY);

	/// <summary>
	/// ベクトルの内積を返す
	/// </summary>
	/// <param name="right"></param>
	/// <returns>内積</returns>
	float Dot(const Vector2& right) const;

	/// <summary>
	/// 2つのベクトルの内積を返す
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <returns>内積</returns>
	float Dot(const Vector2& left, const Vector2& right) const;

	/// <summary>
	/// ベクトルの外積を返す
	/// </summary>
	/// <param name="right"></param>
	/// <returns>外積</returns>
	float Cross(const Vector2& right) const;

	/// <summary>
	/// 2つのベクトルの外積を返す
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <returns>外積</returns>
	float Cross(const Vector2& left, const Vector2& right) const;

	// 演算子オーバーロード
	Vector2 operator+() const;
	Vector2 operator+(const Vector2& val) const;
	Vector2 operator-() const;
	Vector2 operator-(const Vector2& val) const;
	Vector2 operator*(const Vector2& val) const;
	Vector2 operator*(const float& len) const;
	Vector2 operator/(const float& len) const;
	void operator+=(const Vector2& val);
	void operator-=(const Vector2& val);
	void operator*=(const float& len);
	void operator/=(const float& len);
	bool operator==(const Vector2& val) const;
};

