#pragma once
#include "Vector3.h"

/// <summary>
/// AABB矩形を表すクラス
/// </summary>
class BoundingBox
{
public:
	Vector3 min;
	Vector3 max;

	BoundingBox();
	BoundingBox(Vector3 min, Vector3 max);

	/// <summary>
	/// AABB矩形を拡張する
	/// 最大値と最小値を更新
	/// </summary>
	/// <param name="other">指定のAABB矩形</param>
	void Extend(const BoundingBox& other);

};

