#include "Quaternion.h"
#include <cmath>

Quaternion::Quaternion():
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(0.0f)
{
}

Quaternion::Quaternion(float inX, float inY, float inZ, float inW):
	x(inX),
	y(inY),
	z(inZ),
	w(inW)
{
}

Quaternion::~Quaternion()
{
}

float Quaternion::Length() const
{
	// hypotの引数が3つまでしか対応していないようなので直で2乗する
	return std::sqrt(x * x + y * y + z * z + w * w);
}

void Quaternion::Normalize()
{
	float len = Length();
	if (len == 0.0f)
	{
		return;
	}
	x /= len;
	y /= len;
	z /= len;
	w /= len;

}

void Quaternion::ToAxisAngle(Vector3& outAxis, float& outAngle)
{
	float vLen = std::sqrt(x * x + y * y + z * z);

	if (vLen < 1e-8f)
	{
		outAxis = Vector3(0.0f, 1.0f, 0.0f);
		outAngle = 0.0f;
		return;
	}

	outAngle = 2.0f * std::atan2(vLen, w);
	outAxis = Vector3(x / vLen, y / vLen, z / vLen);

}

void Quaternion::DrawAxisLine(const Vector3& origin, const Quaternion& q, float len)
{
	Vector3 axis;
	float angle;
	Quaternion unit = q;
	unit.Normalize();
	unit.ToAxisAngle(axis, angle);

	Vector3 start = Vector3(
		origin.x - axis.x * (len * 0.5f),
		origin.y - axis.y * (len * 0.5f),
		origin.z - axis.z * (len * 0.5f)
	);

	Vector3 end = Vector3(
		origin.x + axis.x * (len * 0.5f),
		origin.y + axis.y * (len * 0.5f),
		origin.z + axis.z * (len * 0.5f)
	);

	DrawLine3D(start.ToDxLibVector(), end.ToDxLibVector(), 0x11ff33);

	DrawFormatString(0, 100, 0xffffff, L"Quaternion : %f,%f,%f\n", q.x, q.y, q.z);

}

Quaternion Quaternion::Identity()
{
	return Quaternion(0.0f,0.0f,0.0f,1.0f);
}

Quaternion Quaternion::Euler(Vector3 rotation)
{
	return Quaternion();
}

Quaternion Quaternion::AngleAxis(float angle, Vector3 axis)
{
	Quaternion ret;

	// 軸と角度を設定する
	ret.w = cos(angle / 2.0f);
	ret.x = axis.x * sin(angle / 2.0f);
	ret.y = axis.y * sin(angle / 2.0f);
	ret.z = axis.z * sin(angle / 2.0f);

	return ret;
}

Quaternion Quaternion::SetRotation(Vector3 start, Vector3 end)
{
	// 二つの座標の外積を求め、それを軸とする
	Vector3 axis = Vector3::Cross(start, end);

	// 外積が0の時は単位クオータニオンを返す
	if (axis == Vector3::Zero())
	{
		return Quaternion::Identity();
	}

	// 二つの座標の内積を求め、そこから回転角度を得る
	// a ・ b = |a| |b| cosθ
	// cosθ = a ・ b / |a| |b|
	float angle = Vector3::Dot(start,end) / (start.Length() * end.Length());

	return Quaternion::AngleAxis(angle, axis);
}

const Quaternion Quaternion::Inverse(Quaternion val)
{
	Quaternion ret;

	// 共役クオータニオンを作成
	ret.w = val.w;
	ret.x = -val.x;
	ret.y = -val.y;
	ret.z = -val.z;

	return ret;
}

Matrix4x4 Quaternion::ToMatrix4x4()
{
	Matrix4x4 ret;
	ret.x0 = (2.0f * w * w) + (2.0f * x * x) - 1;
	ret.y0 = (2.0f * x * y) - (2.0f * z * w);
	ret.z0 = (2.0f * x * z) + (2.0f * y * w);
	ret.x1 = (2.0f * x * y) + (2.0f * z * w);
	ret.y1 = (2.0f * w * w) + (2.0f * y * y) - 1;
	ret.z1 = (2.0f * y * z) - (2.0f * x * w);
	ret.x2 = (2.0f * x * z) - (2.0f * y * w);
	ret.y2 = (2.0f * y * z) + (2.0f * x * w);
	ret.z2 = (2.0f * w * w) + (2.0f * z * z) - 1;

	return ret;
}

Vector3 Quaternion::operator*(Vector3 val)
{
	// ベクトルから単位クオータニオンを作成
	Quaternion vecQ(val.x, val.y, val.z, 0.0f);
	
	// 自分自身のコピーを作成(後で逆クオータニオンに変換する)
	Quaternion invQ(x, y, z, w);

	//invQ.Normalize();

	// 逆クオータニオンに変換
	invQ = Quaternion::Inverse(invQ);

	// 回転させたクォータニオンを作成
	vecQ = *this * vecQ * invQ;

	// 値をVector3に戻す
	Vector3 ret;
	ret.x = vecQ.x;
	ret.y = vecQ.y;
	ret.z = vecQ.z;

	return ret;
}

Quaternion Quaternion::operator*(Quaternion val)
{
	Quaternion ret;

	ret.w = w * val.w - x * val.x - y * val.y - z * val.z;
	ret.x = w * val.x + x * val.w + y * val.z - z * val.y;
	ret.y = w * val.y + y * val.w + z * val.x - x * val.z;
	ret.z = w * val.z + z * val.w + x * val.y - y * val.x;

	return ret;
}
