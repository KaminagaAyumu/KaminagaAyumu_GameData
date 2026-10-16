#pragma once
#include "DxLib.h"

class Vector3
{
public:
	Vector3();
	Vector3(float inX, float inY, float inZ);

	Vector3(const VECTOR& val);

	float x;
	float y;
	float z;

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
	/// すべての値を指定された値にする
	/// </summary>
	/// <param name="val">指定の値</param>
	/// <returns>すべての値が指定された値のベクトル</returns>
	static const Vector3 SetAll(float val);

	/// <summary>
	/// すべての値が0のベクトル
	/// </summary>
	/// <returns></returns>
	static const Vector3 Zero() { return Vector3{ 0.0f,0.0f,0.0f }; }

	/// <summary>
	/// すべての値が1のベクトル
	/// </summary>
	/// <returns></returns>
	static const Vector3 One() { return Vector3{ 1.0f,1.0f,1.0f }; }

	/// <summary>
	/// Yの値のみ1のベクトル
	/// ワールドから見た上方向
	/// </summary>
	/// <returns></returns>
	static const Vector3 Up() { return Vector3{ 0.0f,1.0f,0.0f }; }

	/// <summary>
	/// Yの値のみ-1のベクトル
	/// ワールドから見た下方向
	/// </summary>
	/// <returns></returns>
	static const Vector3 Down() { return Vector3{ 0.0f,-1.0f,0.0f }; }

	/// <summary>
	/// Xの値のみ1のベクトル
	/// ワールドから見た右方向
	/// </summary>
	/// <returns></returns>
	static const Vector3 Right() { return Vector3{ 1.0f,0.0f,0.0f }; }

	/// <summary>
	/// Zの値のみ1のベクトル
	/// ワールドから見た正面方向
	/// </summary>
	/// <returns></returns>
	static const Vector3 Forward() { return Vector3{ 0.0f,0.0f,1.0f }; }

	/// <summary>
	/// Zの値のみ-1のベクトル
	/// ワールドから見た背後方向
	/// </summary>
	/// <returns></returns>
	static const Vector3 Back() { return Vector3{ 0.0f,0.0f,-1.0f }; }

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
	/// 2つのベクトルの成分ごとの最小値を返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>2つのベクトルの成分ごとの最小値</returns>
	static const Vector3 Min(Vector3 left, Vector3 right);

	/// <summary>
	/// 2つのベクトルの成分ごとの最大値を返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>2つのベクトルの成分ごとの最大値</returns>
	static const Vector3 Max(Vector3 left, Vector3 right);

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

	/// <summary>
	/// 線分上の最近点を求める
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="lineA"></param>
	/// <param name="lineB"></param>
	/// <returns></returns>
	static const Vector3 GetClosestPositionOnLineSegment(const Vector3& pos, const Vector3& lineA, const Vector3& lineB);

	/// <summary>
	/// 三角形からの最近点を取得
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="tri0"></param>
	/// <param name="tri1"></param>
	/// <param name="tri2"></param>
	/// <returns></returns>
	static const Vector3 GetClosestPositionOnTriangle(const Vector3& pos, const Vector3& triA, const Vector3& triB, const Vector3& triC);


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

	/// <summary>
	/// DXライブラリのVECTOR型に変換する
	/// </summary>
	operator VECTOR() const;

	/// <summary>
	/// VECTOR型からVector3型に変換する
	/// </summary>
	/// <param name="val"></param>
	/// <returns></returns>
	Vector3& operator=(const VECTOR& val);

};

// 座標情報を管理するために別名で定義
using Position3 = Vector3;