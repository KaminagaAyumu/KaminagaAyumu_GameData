#include "Quaternion.h"
#include "Matrix4x4.h"
#include "../MyLib/MyMath.h"
#include <cmath>
#include <algorithm>

namespace
{
	constexpr float kDotThreshold	= 0.9999f;	// 内積の閾値
	constexpr float kCrossThreshold	= 0.001f;	// 外積の閾値

	// 指定された軸の色
	constexpr unsigned int kAxisLineColor = 0x11ff33;

	// 正面(z軸)の色
	constexpr unsigned int kForwardLineColor = 0x00ff00;
	// 右方向(x軸)の色
	constexpr unsigned int kRightLineColor = 0xff0000;
	// 上方向(y軸)の色
	constexpr unsigned int kUpLineColor = 0x0000ff;
}

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

	DrawLine3D(start, end, kAxisLineColor);
}

void Quaternion::DrawLocalAxis(const Vector3& centerPos, const Quaternion& q, float len)
{
	// クオータニオンを取得
	Quaternion unit = q;
	unit.Normalize();

	// 軸のベクトル
	Vector3 forward = unit * Vector3::Forward();
	Vector3 right = unit * Vector3::Right();
	Vector3 up = unit * Vector3::Up();

	// z軸
	DrawLine3D(centerPos, centerPos + forward * len, kForwardLineColor);
	// x軸
	DrawLine3D(centerPos, centerPos + right * len, kRightLineColor);
	// y軸
	DrawLine3D(centerPos, centerPos + up * len, kUpLineColor);
}

Quaternion Quaternion::Identity()
{
	return Quaternion(0.0f,0.0f,0.0f,1.0f);
}

Quaternion Quaternion::Euler(Vector3 rotation)
{
	// 回転角度をラジアンに変換
	Vector3 radian = { MyLib::ToRadian(rotation.x), MyLib::ToRadian(rotation.y),MyLib::ToRadian(rotation.z) };

	Quaternion xQ = AngleAxis(radian.x, Vector3::Right());
	Quaternion yQ = AngleAxis(radian.y, Vector3::Up());
	Quaternion zQ = AngleAxis(radian.z, Vector3::Back());

	Quaternion ret;

	ret = yQ * xQ * zQ;
	ret.Normalize();

	return ret;
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
	// まず2つの座標を正規化し、方向ベクトルとして分かりやすくする
	Vector3 startNorm = start.Normalized();
	Vector3 endNorm = end.Normalized();

	// 二つの座標の内積を求める
	float dot = Vector3::Dot(startNorm, endNorm);

	// 内積が1に近い(ほぼ同じ方向)の時は単位クオータニオンを返す
	if(dot > kDotThreshold)
	{
		return Quaternion::Identity();
	}

	// 内積が-1に近い(ほぼ逆方向)の時は、外積が0になるのを避ける処理をする
	if(dot < -kDotThreshold)
	{
		// 逆方向の時は、適当な軸を選んで180度回転させるクオータニオンを返す
		Vector3 axis = Vector3::Cross(Vector3::Right(), startNorm);

		// もし上方向と平行だった場合は、右方向を使う
		if (axis.Length() < kCrossThreshold)
		{
			axis = Vector3::Cross(Vector3::Up(), startNorm);
		}
		axis.Normalize();
		return Quaternion::AngleAxis(DX_PI_F, axis);
	}

	// 二つの座標の外積を求め、それを軸とする
	Vector3 axis = Vector3::Cross(startNorm, endNorm);

	axis.Normalize();

	// 外積が0の時は単位クオータニオンを返す
	if (axis == Vector3::Zero())
	{
		return Quaternion::Identity();
	}

	// 二つの座標の内積を求め、そこから回転角度を得る
	// a ・ b = |a| |b| cosθ
	// cosθ = a ・ b / |a| |b|
	float theta = Vector3::Dot(start,end) / (start.Length() * end.Length());

	theta = std::clamp(theta, -1.0f, 1.0f);

	float angle = std::acos(theta);

	return Quaternion::AngleAxis(angle, axis);
}

Quaternion Quaternion::LookRotation(Vector3 forward, Vector3 upward)
{
	if (forward.SqrLength() < 1e-6f)
	{
		return Quaternion::Identity();
	}

	Vector3 forwardDir = forward.Normalized();


	Vector3 r = Vector3::Cross(upward, forwardDir);
	if (r.Length() < kCrossThreshold)
	{
		Vector3 altAxis = (fabsf(forwardDir.x) < 0.9f) ? Vector3::Right() : Vector3::Forward();
		r = Vector3::Cross(altAxis, forwardDir);
	}
	r.Normalize();
	Vector3 u = Vector3::Cross(forwardDir, r);

	Matrix4x4 rotMat = Matrix4x4::Identity();
	rotMat.m[0][0] = r.x;
	rotMat.m[1][0] = r.y;
	rotMat.m[2][0] = r.z;
	rotMat.m[0][1] = u.x;
	rotMat.m[1][1] = u.y;
	rotMat.m[2][1] = u.z;
	rotMat.m[0][2] = forwardDir.x;
	rotMat.m[1][2] = forwardDir.y;
	rotMat.m[2][2] = forwardDir.z;

	return Quaternion::FromMatrix4x4(rotMat);
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

const float Quaternion::Dot(Quaternion left, Quaternion right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

Quaternion Quaternion::Slerp(const Quaternion& start, const Quaternion& end, float t)
{
	// クオータニオン同士の内積を出す
	float dot = Dot(start, end);
	// 反転用に変数を取っておく
	Quaternion target = end;

	// 内積が0以下の場合、最短経路でたどり着くために反転する
	if (dot < 0.0f)
	{
		dot = -dot;
		target.x = -end.x;
		target.y = -end.y;
		target.z = -end.z;
		target.w = -end.w;
	}

	// 内積が1に近い場合、角度が小さいため線形補間を行う
	if (dot >= kDotThreshold)
	{
		Quaternion ret;
		ret.x = start.x + (target.x - start.x) * t;
		ret.y = start.y + (target.y - start.y) * t;
		ret.z = start.z + (target.z - start.z) * t;
		ret.w = start.w + (target.w - start.w) * t;
		ret.Normalize();
		return ret;
	}

	float theta = std::acos(dot);
	Quaternion sNorm = start;
	sNorm.Normalize();
	target.Normalize();
	Quaternion ret = sNorm * (std::sin((1 - t) * theta) / std::sin(theta)) + target * (std::sin(t * theta) / std::sin(theta));

	return ret;
}

Matrix4x4 Quaternion::ToMatrix4x4()
{
	Matrix4x4 ret = Matrix4x4::Identity();

	ret.m[0][0] = (2.0f * w * w) + (2.0f * x * x) - 1;
	ret.m[0][1] = (2.0f * x * y) - (2.0f * z * w);
	ret.m[0][2] = (2.0f * x * z) + (2.0f * y * w);
	ret.m[1][0] = (2.0f * x * y) + (2.0f * z * w);
	ret.m[1][1] = (2.0f * w * w) + (2.0f * y * y) - 1;
	ret.m[1][2] = (2.0f * y * z) - (2.0f * x * w);
	ret.m[2][0] = (2.0f * x * z) - (2.0f * y * w);
	ret.m[2][1] = (2.0f * y * z) + (2.0f * x * w);
	ret.m[2][2] = (2.0f * w * w) + (2.0f * z * z) - 1;

	return ret;
}

Vector3 Quaternion::ToEuler()
{
	float sy = 2.0f * x * z + 2.0f * y * w;
	bool unlocked = std::abs(sy) < 0.99999f;

	return Vector3{
		unlocked ? std::atan2(-(2.0f * y * z -2.0f * x*w), 2.0f * w * w + 2.0f * z * z - 1.0f)
		: std::atan2(2.0f * y * z + 2.0f * x * w, 2.0f * w * w + 2.0f * y * y - 1.0f),
		std::asin(sy),
		unlocked ? std::atan2(-(2.0f * x * y - 2.0f * z * w), 2.0f * w * w + 2.0f * x * x - 1.0f) : 0.0f
	};
}

const Quaternion Quaternion::FromMatrix4x4(const Matrix4x4 mat)
{
	Quaternion ret;
	float x = mat.m[0][0];
	float y = mat.m[1][1];
	float z = mat.m[2][2];

	float trace = x + y + z;

	if (trace > 0.0f)
	{
		// Sを計算
		float s = sqrtf(std::max(0.0f, trace + 1.0f)) * 2.0f;
		ret.w = s * 0.25f;
		ret.x = (mat.m[2][1] - mat.m[1][2]) / s;
		ret.y = (mat.m[0][2] - mat.m[2][0]) / s;
		ret.z = (mat.m[1][0] - mat.m[0][1]) / s;
	}
	else if (x > y && x > z) // x成分が一番大きい場合
	{
		float s = sqrtf(std::max(0.0f, 1.0f + x - y - z)) * 2.0f;
		ret.w = (mat.m[2][1] - mat.m[1][2]) / s;
		ret.x = s * 0.25f;
		ret.y = (mat.m[0][1] + mat.m[1][0]) / s;
		ret.z = (mat.m[0][2] + mat.m[2][0]) / s;
	}
	else if (y > z) // y成分が一番大きい場合
	{
		float s = sqrtf(std::max(0.0f, 1.0f + y - x - z)) * 2.0f;
		ret.w = (mat.m[0][2] - mat.m[2][0]) / s;
		ret.x = (mat.m[0][1] + mat.m[1][0]) / s;
		ret.y = s * 0.25f;
		ret.z = (mat.m[1][2] + mat.m[2][1]) / s;
	}
	else // z成分が一番大きい場合
	{
		float s = sqrtf(std::max(0.0f, 1.0f + z - x - y)) * 2.0f;
		ret.w = (mat.m[1][0] - mat.m[0][1]) / s;
		ret.x = (mat.m[0][2] + mat.m[2][0]) / s;
		ret.y = (mat.m[1][2] + mat.m[2][1]) / s;
		ret.z = s * 0.25f;
	}
	ret.Normalize();
	return ret;
}

Quaternion Quaternion::operator*(const float val) const
{
	Quaternion ret;
	ret.x = x * val;
	ret.y = y * val;
	ret.z = z * val;
	ret.w = w * val;
	return ret;
}

Vector3 Quaternion::operator*(const Vector3& val) const
{
	// ベクトルから単位クオータニオンを作成
	Quaternion vecQ(val.x, val.y, val.z, 0.0f);
	
	// 自分自身のコピーを作成(後で逆クオータニオンに変換する)
	Quaternion invQ(x, y, z, w);

	invQ.Normalize();

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

Quaternion Quaternion::operator+(const Quaternion& val) const
{
	Quaternion ret;

	ret.x = x + val.x;
	ret.y = y + val.y;
	ret.z = z + val.z;
	ret.w = w + val.w;

	return ret;
}

Quaternion Quaternion::operator*(const Quaternion& val) const
{
	Quaternion ret;

	ret.x = w * val.x + x * val.w + y * val.z - z * val.y;
	ret.y = w * val.y + y * val.w + z * val.x - x * val.z;
	ret.z = w * val.z + z * val.w + x * val.y - y * val.x;
	ret.w = w * val.w - x * val.x - y * val.y - z * val.z;

	return ret;
}
