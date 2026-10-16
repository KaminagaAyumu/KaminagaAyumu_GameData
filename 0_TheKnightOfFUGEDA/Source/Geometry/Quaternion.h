#pragma once
#include "Vector3.h"

class Matrix4x4;

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
	/// ローカル座標での軸を取得する
	/// x軸=赤 y軸=緑 z軸=青
	/// </summary>
	/// <param name="centerPos">中心</param>
	/// <param name="q">クオータニオン</param>
	/// <param name="len">軸の長さ</param>
	void DrawLocalAxis(const Vector3& centerPos, const Quaternion& q, float len);

	/// <summary>
	/// 単位クオータニオンを返す
	/// </summary>
	/// <returns></returns>
	static Quaternion Identity();

	/// <summary>
	/// オイラー回転のクオータニオンを返す
	/// </summary>
	/// <param name="rotation">回転角度(度数法)</param>
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
	/// 指定した正面の向きと上方向の基準に回転をするクオータニオンを返す
	/// </summary>
	/// <param name="forward">指定正面方向</param>
	/// <param name="upward">上方向</param>
	/// <returns></returns>
	static Quaternion LookRotation(Vector3 forward, Vector3 upward);

	/// <summary>
	/// 逆クオータニオンを返す
	/// 注意:実際に返しているのは共役クオータニオンなので、使う次元が3次元の時のみ成立する
	/// </summary>
	/// <param name="val">対象のクオータニオン</param>
	/// <returns>逆クオータニオン</returns>
	static const Quaternion Inverse(Quaternion val);

	/// <summary>
	/// クオータニオンの内積を計算する
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>2つのクオータニオンの内積</returns>
	static const float Dot(Quaternion left, Quaternion right);

	/// <summary>
	/// 2つのクオータニオンの値から球の線形補間を行う
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="t">割合</param>
	/// <returns>割合で補完したクオータニオン</returns>
	static Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);

	Matrix4x4 ToMatrix4x4();

	Vector3 ToEuler();

	/// <summary>
	/// 行列からクオータニオンに変換する
	/// </summary>
	/// <param name="mat"></param>
	/// <returns></returns>
	static const Quaternion FromMatrix4x4(const Matrix4x4 mat);

	float x;
	float y;
	float z;
	float w;

	Quaternion operator*(const float val)const;
	Vector3 operator*(const Vector3& val) const;
	Quaternion operator+(const Quaternion& val) const;
	Quaternion operator*(const Quaternion& val) const;


};

