#pragma once
#include "../Geometry/Vector2Int.h"
#include "../Geometry/Vector3.h"
#include "DxLib.h"
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace MyLib
{
	// 半円の角度
	inline constexpr float kHalfCircleDegree = 180.0f;

	// 値を加算する割合
	inline constexpr float kParamAddRate = 0.2f;

	// 表示用の値を現在の値に完全に合わせる際の差分の閾値
	inline constexpr float kParamThreshold = 0.9f;

	/// <summary>
	/// 度数法を弧度法(ラジアン)に変換する
	/// </summary>
	/// <param name="degree">度数法の角度</param>
	/// <returns>弧度法の角度(ラジアン)</returns>
	inline constexpr float ToRadian(float degree) noexcept
	{
		return degree * DX_PI_F / kHalfCircleDegree;
	}

	/// <summary>
	/// 弧度法(ラジアン)を度数法に変換する
	/// </summary>
	/// <param name="radian">弧度法の角度</param>
	/// <returns>度数法の角度</returns>
	inline constexpr float ToDegree(float radian) noexcept
	{
		return radian * (kHalfCircleDegree / DX_PI_F);
	}

	inline float NormalizeAngle(float angle) noexcept
	{
		angle = fmodf(angle + DX_PI_F, DX_TWO_PI_F);

		if (angle < 0.0f)
		{
			angle += DX_TWO_PI_F;
		}

		return angle - DX_PI_F;
	}

	inline float LerpAngle(float from, float to, float t) noexcept
	{
		float diff = NormalizeAngle(to - from);
		return from + diff * t;
	}

	/// <summary>
	/// 指定の値に値をゆっくり近づける
	/// </summary>
	/// <param name="display">変数格納用の値</param>
	/// <param name="target">指定の値</param>
	inline void UpdateParam(int& display, int target)
	{
		// 加算する値を取得
		int add = static_cast<int>((target - display) * kParamAddRate);

		// 加算する値の割合が閾値を超えたら表示用の値を指定の値に合わせる
		if (add <= kParamThreshold)
		{
			display = target;
		}
		else // 値を加算していく
		{
			display += add;
		}
		// 表示用の値が現在の値を超えないようにする
		if (display > target)
		{
			display = target;
		}
	}

	/// <summary>
	/// 指定の値に値をゆっくり近づける(加算対象と減算対象を指定する)
	/// </summary>
	/// <param name="increase"></param>
	/// <param name="decrease"></param>
	/// <param name="target"></param>
	inline void UpdateParamAdjustment(int& increase, int& decrease, int target)
	{
		// 加算する値を取得
		int add = static_cast<int>((target - increase) * kParamAddRate);

		// 加算する値の割合が閾値を超えたら表示用の値を指定の値に合わせる
		if (add <= kParamThreshold)
		{
			increase = target;
			decrease = 0;
			return;
		}

		add = std::min(add, decrease);
		increase += add;
		decrease -= add;
	}

	/// <summary>
	/// ワールド座標からスクリーン座標に変換する
	/// </summary>
	/// <param name="worldPos"></param>
	/// <returns></returns>
	inline Vector2Int WorldPosToScreenPos(const Vector3& worldPos)
	{
		VECTOR screenPos = ConvWorldPosToScreenPos(worldPos);

		return Vector2Int(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y));
	}

	/// <summary>
	/// 余りを正の数にして返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>余りの数(正の数)</returns>
	inline int RemainderToNaturalNumber(int left, int right)
	{
		if (right <= 0) return 0;
		// 左辺と右辺の余りを求める
		int ret = left % right;

		// 余りが負の数だったら右辺の値を足した数を返す
		if (ret < 0)
		{
			return ret + right;
		}

		// ここを通ったら正の数の場合なのでそのまま返す
		return ret;
	}

	/// <summary>
	/// ビットを3D空間で分割するための関数
	/// 最初の10ビットが3ビットおきに分割されます
	/// </summary>
	/// <param name="n">分割するビット列</param>
	/// <returns>分割後のビット列</returns>
	inline constexpr uint32_t BitSeparateFor3D(uint32_t n) noexcept
	{
		// 1024までの値を扱うため、10ビット分のマスクを作成
		// 0000 0000 0000 0000 0000 0011 1111 1111
		n &= 0x000003FF; // 最初の10ビットのみを残す

		// まず上位2ビットを16ビット左にシフトして、下位8ビットと上位8ビットを分離
		// 0000 0011 0000 0000 0000 0000 1111 1111
		n = (n | (n << 16)) & 0x030000FF;
		// 次に上位8ビットを8ビット左にシフトして、4ビットの塊に分離
		// 0000 0011 0000 0000 1111 0000 0000 1111
		n = (n | (n << 8)) & 0x0300F00F;
		// 次に上位4ビットを4ビット左にシフトして、2ビットの塊に分離
		// 0000 0011 0000 1100 0011 0000 1100 0011
		n = (n | (n << 4)) & 0x030C30C3;
		// 最後に上位2ビットを2ビット左にシフトして、3ビットおきにビットが配置されるようにする
		// 0000 1001 0010 0100 1001 0010 0100 1001
		n = (n | (n << 2)) & 0x09249249;
		return n;
	}

	/// <summary>
	/// 3D空間でのモートン番号を取得する関数
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	/// <returns>モートン番号</returns>
	inline constexpr uint32_t GetMortonNumber3D(uint32_t x, uint32_t y, uint32_t z) noexcept
	{
		// 各座標のビットを3ビットおきに分割し、最終的に1つの整数にまとめる
		return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
	}

	/// <summary>
	/// 数値の8乗を計算する
	/// </summary>
	/// <param name="exp"></param>
	/// <returns></returns>
	inline constexpr uint32_t Pow8(uint32_t exp) noexcept
	{
		uint32_t result = 1;
		for (uint32_t i = 0; i < exp; ++i)
		{
			result *= 8;
		}
		return result;
	}
}