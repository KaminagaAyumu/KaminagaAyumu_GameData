#pragma once
#include "Vector3.h"
#include "DxLib.h"

/// <summary>
/// 4*4の行列クラス
/// 行はx,y,z,w、列は0,1,2,3で表している
/// 例:最初の行はx0,y0,z0,w0、次の行はx1,y1,z1,w1となる
/// </summary>
class Matrix4x4
{
public:

	float x0;
	float x1;
	float x2;
	float x3;
	float y0;
	float y1;
	float y2;
	float y3;
	float z0;
	float z1;
	float z2;
	float z3;
	float w0;
	float w1;
	float w2;
	float w3;

	Matrix4x4();
	Matrix4x4(float inX0, float inY0, float inZ0, float inW0,
		float inX1, float inY1, float inZ1, float inW1,
		float inX2, float inY2, float inZ2, float inW2,
		float inX3, float inY3, float inZ3, float inW3);

	/// <summary>
	/// 行列の値をすべて表示する
	/// </summary>
	void DrawAll();

	void RotateX(float angle);

	void RotateY(float angle);

	void RotateZ(float angle);

	void Translate(float x, float y, float z);

	void Scale(float x, float y, float z);

	MATRIX ToDxLibMatrix();

	Vector3 operator*(Vector3 val);


	Matrix4x4 operator+(Matrix4x4 val);
	Matrix4x4 operator-(Matrix4x4 val);
	Matrix4x4 operator*(Matrix4x4 val);


};

