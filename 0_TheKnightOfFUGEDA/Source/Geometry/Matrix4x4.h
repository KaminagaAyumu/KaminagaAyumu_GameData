#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include "DxLib.h"

/// <summary>
/// 4*4の行列クラス
/// 行はx,y,z,w、列は0,1,2,3で表している
/// 例:最初の行はx0,y0,z0,w0、次の行はx1,y1,z1,w1となる
/// </summary>
class Matrix4x4
{
public:

	// 行と列の配列で表す
	// 例:最初の行はm[0][0],m[0][1],m[0][2],m[0][3]、次の行はm[1][0],m[1][1],m[1][2],m[1][3]となる
	float m[4][4];

	Matrix4x4();
	Matrix4x4(float inX0, float inY0, float inZ0, float inW0,
		float inX1, float inY1, float inZ1, float inW1,
		float inX2, float inY2, float inZ2, float inW2,
		float inX3, float inY3, float inZ3, float inW3);

	/// <summary>
	/// 転置行列を得る
	/// </summary>
	/// <returns></returns>
	Matrix4x4 Transpose() const;

	/// <summary>
	/// 単位行列を返す
	/// </summary>
	/// <returns></returns>
	static const Matrix4x4 Identity()
	{
		return Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	static const Matrix4x4 GetTranslate(const Vector3& vec)
	{
		Matrix4x4 ret = Matrix4x4::Identity();

		ret.m[0][3] = vec.x;
		ret.m[1][3] = vec.y;
		ret.m[2][3] = vec.z;

		return ret;
	}

	static const Matrix4x4 GetScale(const Vector3& vec)
	{
		Matrix4x4 ret = Matrix4x4::Identity();

		ret.m[0][0] = vec.x;
		ret.m[1][1] = vec.y;
		ret.m[2][2] = vec.z;

		return ret;
	}

	/// <summary>
	/// 逆行列を作る
	/// </summary>
	/// <param name="mat"></param>
	/// <returns></returns>
	static const Matrix4x4 Inverse(Matrix4x4 mat);

	const Position3 GetPosition();

	const Vector3 GetScale();

	const Quaternion GetRotation() const;


	Vector3 operator*(const Vector3& val);


	Matrix4x4 operator+(const Matrix4x4& val)const;
	Matrix4x4 operator-(const Matrix4x4& val)const;
	Matrix4x4 operator*(const Matrix4x4& val)const;

	static const Matrix4x4 ToMatrix(const MATRIX& val);

	operator MATRIX() const;
	Matrix4x4& operator=(const MATRIX& val);

};