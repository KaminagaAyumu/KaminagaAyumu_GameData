#include "Matrix4x4.h"
#include <cmath>
#include "DxLib.h"

Matrix4x4::Matrix4x4()
{
	x0 = 0.0f;
	y0 = 0.0f;
	z0 = 0.0f;
	w0 = 0.0f;

	x1 = 0.0f;
	y1 = 0.0f;
	z1 = 0.0f;
	w1 = 0.0f;

	x2 = 0.0f;
	y2 = 0.0f;
	z2 = 0.0f;
	w2 = 0.0f;
	
	x3 = 0.0f;
	y3 = 0.0f;
	z3 = 0.0f;
	w3 = 0.0f;
}

Matrix4x4::Matrix4x4(float inX0, float inY0, float inZ0, float inW0,
	float inX1, float inY1, float inZ1, float inW1,
	float inX2, float inY2, float inZ2, float inW2,
	float inX3, float inY3, float inZ3, float inW3)
{
	x0 = inX0;
	y0 = inY0;
	z0 = inZ0;
	w0 = inW0;

	x1 = inX1;
	y1 = inY1;
	z1 = inZ1;
	w1 = inW1;
	
	x2 = inX2;
	y2 = inY2;
	z2 = inZ2;
	w2 = inW2;
	
	x3 = inX3;
	y3 = inY3;
	z3 = inZ3;
	w3 = inW3;
}

void Matrix4x4::DrawAll()
{
	DrawFormatString(0, 32, 0xffffff, L"x0:%f, y0:%f, z0:%f, w0:%f", x0, y0, z0, w0);
	DrawFormatString(0, 48, 0xffffff, L"x1:%f, y1:%f, z1:%f, w1:%f", x1, y1, z1, w1);
	DrawFormatString(0, 64, 0xffffff, L"x2:%f, y2:%f, z2:%f, w2:%f", x2, y2, z2, w2);
	DrawFormatString(0, 80, 0xffffff, L"x3:%f, y3:%f, z3:%f, w3:%f", x3, y3, z3, w3);
}

void Matrix4x4::RotateX(float angle)
{
	x0 = 1.0f;
	y1 = cos(angle);
	z1 = -sin(angle);
	y2 = sin(angle);
	z2 = cos(angle);
	w3 = 1.0f;
}

void Matrix4x4::RotateY(float angle)
{
	x0 = cos(angle);
	z0 = sin(angle);
	y1 = 1.0f;
	x2 = -sin(angle);
	z2 = cos(angle);
	w3 = 1.0f;
}

void Matrix4x4::RotateZ(float angle)
{
	x0 = cos(angle);
	y0 = -sin(angle);
	x1 = sin(angle);
	y1 = cos(angle);
	z2 = 1.0f;
	w3 = 1.0f;
}

void Matrix4x4::Translate(float x, float y, float z)
{
	x0 = 1.0f;
	y1 = 1.0f;
	z2 = 1.0f;
	w3 = 1.0f;
	w0 = x;
	w1 = y;
	w2 = z;
}

void Matrix4x4::Scale(float x, float y, float z)
{
	x0 = x;
	y1 = y;
	z2 = z;
	w3 = 1.0f;
}

MATRIX Matrix4x4::ToDxLibMatrix()
{
	MATRIX mat;

	mat.m[0][0] = x0;
	mat.m[0][1] = y0;
	mat.m[0][2] = z0;
	mat.m[0][3] = w0;
	
	mat.m[1][0] = x1;
	mat.m[1][1] = y1;
	mat.m[1][2] = z1;
	mat.m[1][3] = w1;

	mat.m[2][0] = x2;
	mat.m[2][1] = y2;
	mat.m[2][2] = z2;
	mat.m[2][3] = w2;
	
	mat.m[3][0] = x3;
	mat.m[3][1] = y3;
	mat.m[3][2] = z3;
	mat.m[3][3] = w3;

	return mat;
}

Vector3 Matrix4x4::operator*(Vector3 val)
{
	Vector3 ret{};

	ret.x = x0 * val.x + y0 * val.y + z0 * val.z + w0 * 1.0f;
	ret.y = x1 * val.x + y1 * val.y + z1 * val.z + w1 * 1.0f;
	ret.z = x2 * val.x + y2 * val.y + z2 * val.z + w2 * 1.0f;

	return ret;
}

Matrix4x4 Matrix4x4::operator+(Matrix4x4 val)
{
	Matrix4x4 ret{};

	ret.x0 = x0 + val.x0;
	ret.y0 = y0 + val.y0;
	ret.z0 = z0 + val.z0;
	ret.w0 = w0 + val.w0;
	
	ret.x1 = x1 + val.x1;
	ret.y1 = y1 + val.y1;
	ret.z1 = z1 + val.z1;
	ret.w1 = w1 + val.w1;
	
	ret.x2 = x2 + val.x2;
	ret.y2 = y2 + val.y2;
	ret.z2 = z2 + val.z2;
	ret.w2 = w2 + val.w2;
	
	ret.x3 = x3 + val.x3;
	ret.y3 = y3 + val.y3;
	ret.z3 = z3 + val.z3;
	ret.w3 = w3 + val.w3;

	return ret;
}

Matrix4x4 Matrix4x4::operator-(Matrix4x4 val)
{
	Matrix4x4 ret{};

	ret.x0 = x0 - val.x0;
	ret.y0 = y0 - val.y0;
	ret.z0 = z0 - val.z0;
	ret.w0 = w0 - val.w0;

	ret.x1 = x1 - val.x1;
	ret.y1 = y1 - val.y1;
	ret.z1 = z1 - val.z1;
	ret.w1 = w1 - val.w1;

	ret.x2 = x2 - val.x2;
	ret.y2 = y2 - val.y2;
	ret.z2 = z2 - val.z2;
	ret.w2 = w2 - val.w2;

	ret.x3 = x3 - val.x3;
	ret.y3 = y3 - val.y3;
	ret.z3 = z3 - val.z3;
	ret.w3 = w3 - val.w3;

	return ret;
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4 val)
{	
	Matrix4x4 ret{};

	// 0行目
	ret.x0 = x0 * val.x0 + y0 * val.x1 + z0 * val.x2 + w0 * val.x3;
	ret.y0 = x0 * val.y0 + y0 * val.y1 + z0 * val.y2 + w0 * val.y3;
	ret.z0 = x0 * val.z0 + y0 * val.z1 + z0 * val.z2 + w0 * val.z3;
	ret.w0 = x0 * val.w0 + y0 * val.w1 + z0 * val.w2 + w0 * val.w3;

	// 1行目
	ret.x1 = x1 * val.x0 + y1 * val.x1 + z1 * val.x2 + w1 * val.x3;
	ret.y1 = x1 * val.y0 + y1 * val.y1 + z1 * val.y2 + w1 * val.y3;
	ret.z1 = x1 * val.z0 + y1 * val.z1 + z1 * val.z2 + w1 * val.z3;
	ret.w1 = x1 * val.w0 + y1 * val.w1 + z1 * val.w2 + w1 * val.w3;

	// 2行目
	ret.x2 = x2 * val.x0 + y2 * val.x1 + z2 * val.x2 + w2 * val.x3;
	ret.y2 = x2 * val.y0 + y2 * val.y1 + z2 * val.y2 + w2 * val.y3;
	ret.z2 = x2 * val.z0 + y2 * val.z1 + z2 * val.z2 + w2 * val.z3;
	ret.w2 = x2 * val.w0 + y2 * val.w1 + z2 * val.w2 + w2 * val.w3;

	// 3行目
	ret.x3 = x3 * val.x0 + y3 * val.x1 + z3 * val.x2 + w3 * val.x3;
	ret.y3 = x3 * val.y0 + y3 * val.y1 + z3 * val.y2 + w3 * val.y3;
	ret.z3 = x3 * val.z0 + y3 * val.z1 + z3 * val.z2 + w3 * val.z3;
	ret.w3 = x3 * val.w0 + y3 * val.w1 + z3 * val.w2 + w3 * val.w3;

	return ret;
}

