#include "Vector2.h"
#include <cmath>

float Vector2::Length() const
{
	return sqrtf(X * X + Y * Y);
}

float Vector2::Magnitude() const
{
	return X * X + Y * Y;
}

Vector2 Vector2::Nomalize() const
{
	float len = Length();

	return Vector2(X / len, Y / len);
}

float Vector2::Dot(const Vector2& left, const Vector2& right)
{
	return left.X * right.X + left.Y * right.Y;
}

Vector2 Vector2::Project(const Vector2& left, const Vector2& right)
{
	// 射影軸になるベクトルの単位ベクトルを出す
	Vector2 rightHat = right.Nomalize();

	// 射影軸の単位ベクトルと、射影したいベクトルの内積を計算し、
	// 射影ベクトルの大きさをだす
	float size = Vector2::Dot(rightHat, left);

	// 射影軸の単位ベクトルと、射影ベクトルの大きさをかけて、
	// 射影ベクトルを出す
	Vector2 ans = size * rightHat;

	return ans;
}

float Vector2::CrossZ(const Vector2& left, const Vector2& right)
{
	return left.X * right.Y - left.Y * right.X;
}



Vector2 operator+(const Vector2& left, const Vector2& right)
{
	return Vector2(left.X + right.X, left.Y + right.Y);
}

Vector2 operator-(const Vector2& left, const Vector2& right)
{
	return Vector2(left.X - right.X, left.Y - right.Y);
}

Vector2 operator*(const float& left, const Vector2& right)
{
	return Vector2(left * right.X, left * right.Y);
}

Vector2 operator*(const Vector2& left, const float& right)
{
	return Vector2(left.X * right, left.Y * right);
}

Vector2 operator/(const Vector2& left, const float& right)
{
	return Vector2(left.X / right, left.Y / right);
}
