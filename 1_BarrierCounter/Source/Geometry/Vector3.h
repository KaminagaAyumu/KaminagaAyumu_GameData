#pragma once
#include "DxLib.h"

class Vector3
{
public:
	Vector3();
	Vector3(float inX, float inY, float inZ);

	float x;
	float y;
	float z;

	/// <summary>
	/// DxLibのVECTOR構造体に変換する
	/// </summary>
	/// <returns>VECTOR構造体</returns>
	VECTOR ToDxLibVector() const;

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
	/// 正規化されたベクトルを返す
	/// </summary>
	/// <returns></returns>
	const Vector3 Normalized();

	/// <summary>
	/// すべての値が0のベクトル
	/// </summary>
	/// <returns></returns>
	static const Vector3 Zero() { return Vector3{ 0.0f,0.0f,0.0f }; }

	/// <summary>
	/// Yの値のみ1のベクトル
	/// </summary>
	/// <returns></returns>
	static const Vector3 Up() { return Vector3{ 0.0f,1.0f,0.0f }; }

	/// <summary>
	/// Xの値のみ1のベクトル
	/// </summary>
	/// <returns></returns>
	static const Vector3 Right() { return Vector3{ 1.0f,0.0f,0.0f }; }

	/// <summary>
	/// 2つのベクトルの内積を返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>2つのベクトルの内積</returns>
	static const float Dot(Vector3 left, Vector3 right);

	/// <summary>
	/// 2つのベクトルの外積を返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>2つのベクトルの外積</returns>
	static const Vector3 Cross(Vector3 left, Vector3 right);

	/// <summary>
	/// 2つのベクトルの位置から線形補完する
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="t">割合</param>
	/// <returns>割合で補完したベクトル</returns>
	static const Vector3 LerpVec3(const Vector3& start, const Vector3& end, float t);

	/// <summary>
	/// 2つのベクトル間の距離を返す
	/// </summary>
	/// <param name="from">始点</param>
	/// <param name="to">終点</param>
	/// <returns>ベクトル間の距離</returns>
	static const float GetDistance(const Vector3& from, const Vector3& to);

	// 演算子オーバーロード
	Vector3 operator+(const Vector3& len) const;
	Vector3 operator-(const Vector3& len) const;
	Vector3 operator*(const Vector3& len) const;
	Vector3 operator*(const float& len) const;
	Vector3 operator/(const float& len) const;
	Vector3 operator-()const;
	void operator+=(const Vector3& len);
	void operator-=(const Vector3& len);
	void operator*=(const float& len);
	void operator/=(const float& len);
	bool operator==(const Vector3& len) const;


};

