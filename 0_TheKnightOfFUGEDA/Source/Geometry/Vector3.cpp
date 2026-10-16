#include "Vector3.h"
#include <cmath>
#include <cassert>

namespace
{
	constexpr float kClosestPointThreshold = 0.00001f;
}

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

Vector3::Vector3(const VECTOR& val)
{
	x = val.x;
	y = val.y;
	z = val.z;
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
	const float len = Length();
	if (len == 0.0f)
	{
		return;
	}
	x /= len;
	y /= len;
	z /= len;
}

const Vector3 Vector3::Normalized()
{
	const float len = Length();
	if (len == 0.0f)
	{
		return Vector3::Zero();
	}
	return Vector3(x / len, y / len, z / len);
}

const Vector3 Vector3::SetAll(float val)
{
	return Vector3(val, val, val);
}

const float Vector3::Dot(Vector3 left, Vector3 right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

const Vector3 Vector3::Cross(Vector3 left, Vector3 right)
{
	return Vector3(left.y * right.z - left.z * right.y, left.z * right.x - left.x * right.z, left.x * right.y - left.y * right.x);
}

const Vector3 Vector3::Min(Vector3 left, Vector3 right)
{
	// 戻り値にするベクトル
	Vector3 ret;

	// 成分ごとの最小値を計算
	ret.x = std::fmin(left.x, right.x);
	ret.y = std::fmin(left.y, right.y);
	ret.z = std::fmin(left.z, right.z);

	return ret;
}

const Vector3 Vector3::Max(Vector3 left, Vector3 right)
{
	// 戻り値にするベクトル
	Vector3 ret;

	// 成分ごとの最大値を計算
	ret.x = std::fmax(left.x, right.x);
	ret.y = std::fmax(left.y, right.y);
	ret.z = std::fmax(left.z, right.z);

	return ret;
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

const Vector3 Vector3::GetClosestPositionOnLineSegment(const Vector3& pos, const Vector3& lineA, const Vector3& lineB)
{
	// 線分のA地点からの2点に向かうベクトルを求める
	Vector3 AB = lineB - lineA;
	Vector3 AP = pos - lineA;
	
	// 線分を正規化
	Vector3 normAB = AB.Normalized();

	// 線分のサイズを計算
	float abLength = AB.Length();

	// AとBが重なっている場合最近点はAとする
	if (abLength < kClosestPointThreshold) return lineA;

	// 内積からの割合でABの位置のどこにいるのかを取得
	float dist = Dot(normAB, AP) / AB.Length();

	// 当たった位置が0未満ならばAが最近点になる
	if (dist < 0.0f) return lineA;
	// 当たった位置が1以上ならばBが最近点になる
	if (dist >= 1.0f) return lineB;

	// ABの線分上の位置に変換
	// A地点からABの距離分足す
	Vector3 ret = lineA + AB * dist;

	return ret;
}

const Position3 Vector3::GetClosestPositionOnTriangle(const Vector3& pos, const Vector3& triA, const Vector3& triB, const Vector3& triC)
{
	// 三角形平面上の法線を求める
	Vector3 AB = triB - triA;
	Vector3 AC = triC - triA;
	// 法線
	Vector3 norm = Cross(AB, AC);
	norm.Normalize();

	// 点Aから点Pに向かうベクトルを定義
	Vector3 AP = pos - triA;

	// 三角形平面からの距離を取得
	float planeDist = Dot(AP, norm);

	// 三角形平面上の位置を計算
	// 現在の座標から平面方向に距離の分動かすことで平面上の位置がわかる
	Vector3 planePos = pos - norm * planeDist;

	// 平面上の最近点が三角形の中にあるかどうかを判定
	// 計算するための線分を定義
	Vector3 BC = triC - triB;
	Vector3 CA = triA - triC;
	// まず、線分AB,BC,CAと最近点に向かうベクトルの外積を計算
	Vector3 crossAB = Cross(AB, planePos - triA);
	Vector3 crossBC = Cross(BC, planePos - triB);
	Vector3 crossCA = Cross(CA, planePos - triC);

	// 法線との内積を取り、0以上=内側となるため、
	// 3つの線分すべての内側に入っている場合は最近点は平面上の位置になる
	if (Dot(crossAB, norm) >= 0 && Dot(crossBC, norm) >= 0 && Dot(crossCA, norm) >= 0)
	{
		return planePos;
	}

	// ここまで来たら三角形の平面上の位置にいないため線分ごとの最近点を求める
	Vector3 posAB = GetClosestPositionOnLineSegment(pos, triA, triB);
	Vector3 posBC = GetClosestPositionOnLineSegment(pos, triB, triC);
	Vector3 posCA = GetClosestPositionOnLineSegment(pos, triC, triA);

	// 線分ごとの最近点との距離の大きさを計算する
	float distAB = (pos - posAB).SqrLength();
	float distBC = (pos - posBC).SqrLength();
	float distCA = (pos - posCA).SqrLength();

	// ABの距離が最近点の場合
	if (distAB <= distBC && distAB <= distCA)
	{
		return posAB;
	}

	// BCの距離が最近点の場合(ABは上の条件で除外)
	if (distBC <= distCA)
	{
		return posBC;
	}

	// AB,BCが除外されたため、最も近いのはCAになる
	return posCA;
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

Vector3::operator VECTOR() const
{
	return VGet(x, y, z);
}

Vector3& Vector3::operator=(const VECTOR& val)
{
	x = val.x;
	y = val.y;
	z = val.z;
	return *this;
}
