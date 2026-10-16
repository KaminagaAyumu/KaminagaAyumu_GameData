#include "Matrix4x4.h"
#include <cmath>

namespace
{
	constexpr int kRowNum = 4;
}

Matrix4x4::Matrix4x4() : 
	m{0.0f}
{
}

Matrix4x4::Matrix4x4(float inX0, float inY0, float inZ0, float inW0,
	float inX1, float inY1, float inZ1, float inW1,
	float inX2, float inY2, float inZ2, float inW2,
	float inX3, float inY3, float inZ3, float inW3)
{
	m[0][0] = inX0;
	m[0][1] = inY0;
	m[0][2] = inZ0;
	m[0][3] = inW0;

	m[1][0] = inX1;
	m[1][1] = inY1;
	m[1][2] = inZ1;
	m[1][3] = inW1;

	m[2][0] = inX2;
	m[2][1] = inY2;
	m[2][2] = inZ2;
	m[2][3] = inW2;

	m[3][0] = inX3;
	m[3][1] = inY3;
	m[3][2] = inZ3;
	m[3][3] = inW3;
}

Matrix4x4 Matrix4x4::Transpose() const
{
	Matrix4x4 ret{};

	for(int i = 0; i < kRowNum; ++i)
	{
		for(int j = 0; j < kRowNum; ++j)
		{
			ret.m[i][j] = m[j][i];
		}
	}

	return ret;
}

const Matrix4x4 Matrix4x4::Inverse(Matrix4x4 mat)
{
	Matrix4x4 ret;

	// DXライブラリの逆行列を求める関数を使う
	ret = MInverse(mat);

	return ret;
}

const Position3 Matrix4x4::GetPosition()
{
	Position3 ret;

	// 座標情報は行の3列目に入っている
	ret.x = m[0][3];
	ret.y = m[1][3];
	ret.z = m[2][3];

	return ret;
}

const Vector3 Matrix4x4::GetScale()
{
	Vector3 ret;

	// 軸毎の値を見る
	Vector3 xAxis{ m[0][0], m[0][1], m[0][2] };
	Vector3 yAxis{ m[1][0], m[1][1], m[1][2] };
	Vector3 zAxis{ m[2][0], m[2][1], m[2][2] };

	// 軸毎の大きさが拡大率になる
	ret.x = xAxis.Length();
	ret.y = yAxis.Length();
	ret.z = zAxis.Length();

	return ret;
}

const Quaternion Matrix4x4::GetRotation() const
{
	Vector3 scale;

	// 軸毎の値を見る
	Vector3 xAxis{ m[0][0], m[0][1], m[0][2] };
	Vector3 yAxis{ m[1][0], m[1][1], m[1][2] };
	Vector3 zAxis{ m[2][0], m[2][1], m[2][2] };

	// 軸毎の大きさが拡大率になる
	scale.x = xAxis.Length();
	scale.y = yAxis.Length();
	scale.z = zAxis.Length();

	// 各軸をスケールで割ることで正規化する
	xAxis /= scale.x;
	yAxis /= scale.y;
	zAxis /= scale.z;

	// 回転行列を作る
	Matrix4x4 rotMat;
	rotMat.m[0][0] = xAxis.x;
	rotMat.m[0][1] = xAxis.y;
	rotMat.m[0][2] = xAxis.z;
	
	rotMat.m[1][0] = yAxis.x;
	rotMat.m[1][1] = yAxis.y;
	rotMat.m[1][2] = yAxis.z;
	
	rotMat.m[2][0] = zAxis.x;
	rotMat.m[2][1] = zAxis.y;
	rotMat.m[2][2] = zAxis.z;

	Quaternion q = q.FromMatrix4x4(rotMat);

	return q;
}

Vector3 Matrix4x4::operator*(const Vector3& val)
{
	Vector3 ret{};

	ret.x = m[0][0] * val.x + m[0][1] * val.y + m[0][2] * val.z + m[0][3] * 1.0f;
	ret.y = m[1][0] * val.x + m[1][1] * val.y + m[1][2] * val.z + m[1][3] * 1.0f;
	ret.z = m[2][0] * val.x + m[2][1] * val.y + m[2][2] * val.z + m[2][3] * 1.0f;

	return ret;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& val) const
{
	Matrix4x4 ret{};

	for(int i = 0; i < kRowNum; ++i)
	{
		for(int j = 0; j < kRowNum; ++j)
		{
			ret.m[i][j] = m[i][j] + val.m[i][j];
		}
	}

	return ret;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& val) const
{
	Matrix4x4 ret{};

	for(int i = 0; i < kRowNum; ++i)
	{
		for(int j = 0; j < kRowNum; ++j)
		{
			ret.m[i][j] = m[i][j] - val.m[i][j];
		}
	}

	return ret;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& val) const
{
	Matrix4x4 ret{};

	for(int i = 0; i < kRowNum; ++i)
	{
		for(int j = 0; j < kRowNum; ++j)
		{
			for(int k = 0; k < kRowNum; ++k)
			{
				ret.m[i][j] += m[i][k] * val.m[k][j];
			}
		}
	}

	return ret;
}

const Matrix4x4 Matrix4x4::ToMatrix(const MATRIX& val)
{
	Matrix4x4 ret{};

	for (int i = 0; i < kRowNum; ++i)
	{
		for (int j = 0; j < kRowNum; ++j)
		{
			// DXライブラリのMATRIX型は行列の要素が転置されているので、転置して代入する
			ret.m[i][j] = val.m[j][i];
		}
	}

	return ret;
}

Matrix4x4::operator MATRIX() const
{
	MATRIX ret{};
	for(int i = 0; i < kRowNum; ++i)
	{
		for(int j = 0; j < kRowNum; ++j)
		{
			// DXライブラリのMATRIX型は行列の要素が転置されているので、転置して代入する
			ret.m[j][i] = m[i][j];
		}
	}

	return ret;
}

Matrix4x4& Matrix4x4::operator=(const MATRIX& val)
{
	Matrix4x4 ret{};

	for(int i = 0; i < kRowNum; ++i)
	{
		for(int j = 0; j < kRowNum; ++j)
		{
			// DXライブラリのMATRIX型は行列の要素が転置されているので、転置して代入する
			ret.m[i][j] = val.m[j][i];
		}
	}

	return *this = ret;
}

