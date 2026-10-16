#include "BoundingBox.h"
#include <algorithm>

BoundingBox::BoundingBox() : 
	min{},
	max{}
{
}

BoundingBox::BoundingBox(Vector3 min, Vector3 max) : 
	min{min},
	max{max}
{
}

void BoundingBox::Extend(const BoundingBox& other)
{
	// AABB矩形の最小値と最大値を更新する
	min.x = std::min(min.x, other.min.x);
	min.y = std::min(min.y, other.min.y);
	min.z = std::min(min.z, other.min.z);
	max.x = std::max(max.x, other.max.x);
	max.y = std::max(max.y, other.max.y);
	max.z = std::max(max.z, other.max.z);
}
