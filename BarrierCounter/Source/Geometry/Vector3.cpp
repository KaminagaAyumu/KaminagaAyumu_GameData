#include "Vector3.h"
#include <cmath>
#include <cassert>

Vector3::Vector3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Vector3::Vector3(float inX, float inY, float inZ)
{
	x = inX;
	y = inY;
	z = inZ;
}

VECTOR Vector3::ToDxLibVector() const
{
	return VGet(x, y, z);
}

float Vector3::Length()
{
	return std::hypot(x,y,z);
}

float Vector3::SqrLength()
{
	return (x * x + y * y + z * z);
}

void Vector3::Normalize()
{
	if (Length() == 0.0f)
	{
		return;
	}
	x /= Length();
	y /= Length();
	z /= Length();
}

const Vector3 Vector3::Normalized()
{
	if (Length() == 0.0f)
	{
		return Vector3::Zero();
	}
	return Vector3(x / Length(), y / Length(), z / Length());
}

const float Vector3::Dot(Vector3 left, Vector3 right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

const Vector3 Vector3::Cross(Vector3 left, Vector3 right)
{
	return Vector3(left.y * right.z - left.z * right.y, left.z * right.x - left.x * right.z, left.x * right.y - left.y * right.x);
}

const Vector3 Vector3::LerpVec3(const Vector3& start, const Vector3& end, float t)
{
	// 戻り値にするベクトル
	Vector3 ret;

	// 成分ごとにlerpで補完
	ret.x = std::lerp(start.x, end.x, t);
	ret.y = std::lerp(start.y, end.y, t);
	ret.z = std::lerp(start.z, end.z, t);

	return ret;
}

const float Vector3::GetDistance(const Vector3& from, const Vector3& to)
{
	return std::hypot(to.x - from.x, to.y - from.y, to.z - from.z);
}

Vector3 Vector3::operator+(const Vector3& len) const
{
	return Vector3(x + len.x, y + len.y, z + len.z);
}

Vector3 Vector3::operator-(const Vector3& len) const
{
	return Vector3(x - len.x, y - len.y, z - len.z);
}

Vector3 Vector3::operator*(const Vector3& len) const
{
	return Vector3(x * len.x, y * len.y, z * len.z);
}

Vector3 Vector3::operator*(const float& len) const
{
	return Vector3(x * len, y * len, z * len);
}

Vector3 Vector3::operator/(const float& len) const
{
	// 0以外の時のみ計算する
	assert(len != 0.0f && "0除算を検知しました");
	return Vector3(x / len, y / len, z / len);
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

void Vector3::operator+=(const Vector3& len)
{
	x += len.x;
	y += len.y;
	z += len.z;
}

void Vector3::operator-=(const Vector3& len)
{
	x -= len.x;
	y -= len.y;
	z -= len.z;
}

void Vector3::operator*=(const float& len)
{
	x *= len;
	y *= len;
	z *= len;
}

void Vector3::operator/=(const float& len)
{
	// 0以外の時のみ計算する
	assert(len != 0.0f && "0除算を検知しました");
	x /= len;
	y /= len;
	z /= len;
}

bool Vector3::operator==(const Vector3& len) const
{
	return (x == len.x) && (y == len.y) && (z == len.z);
}