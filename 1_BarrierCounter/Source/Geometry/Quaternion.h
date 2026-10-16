#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

/// <summary>
/// クオータニオン(四元数)を表すクラス
/// </summary>
class Quaternion
{
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// すべての値を0で初期化
	/// </summary>
	Quaternion();

	/// <summary>
	/// コンストラクタ
	/// 軸の座標3つと角度を設定して初期化する
	/// </summary>
	/// <param name="x">軸X</param>
	/// <param name="y">軸Y</param>
	/// <param name="z">軸Z</param>
	/// <param name="w">角度(ラジアン)</param>
	Quaternion(float inX, float inY, float inZ, float inW);

	virtual ~Quaternion();

	float Length()const;

	void Normalize();

	void ToAxisAngle(Vector3& outAxis, float& outAngle);

	void DrawAxisLine(const Vector3& origin, const Quaternion& q, float len);

	/// <summary>
	/// 単位クオータニオンを返す
	/// </summary>
	/// <returns></returns>
	static Quaternion Identity();

	/// <summary>
	/// オイラー回転のクオータニオンを返す
	/// </summary>
	/// <param name="rotation"></param>
	/// <returns></returns>
	static Quaternion Euler(Vector3 rotation);

	/// <summary>
	/// 角度と軸を設定したクオータニオンを返す
	/// </summary>
	/// <param name="angle">角度(ラジアン)</param>
	/// <param name="axis">回転軸</param>
	static Quaternion AngleAxis(float angle, Vector3 axis);
	

	static Quaternion SetRotation(Vector3 start, Vector3 end);

	/// <summary>
	/// 逆クオータニオンを返す
	/// 注意:実際に返しているのは共役クオータニオンなので、使う次元が3次元の時のみ成立する
	/// </summary>
	/// <param name="val">対象のクオータニオン</param>
	/// <returns>逆クオータニオン</returns>
	static const Quaternion Inverse(Quaternion val);

	Matrix4x4 ToMatrix4x4();

	float x;
	float y;
	float z;
	float w;


	Vector3 operator*(Vector3 val);
	Quaternion operator*(Quaternion val);


};

