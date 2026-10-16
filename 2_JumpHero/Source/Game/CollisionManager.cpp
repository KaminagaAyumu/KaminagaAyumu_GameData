#include "CollisionManager.h"
#include "Actor.h"
#include "../Utility/GameType.h"

void CollisionManager::CheckCollision(const std::vector<std::weak_ptr<Actor>>& actors)
{
	for (size_t i = 0; i < actors.size(); ++i)
	{
		for (size_t j = i + 1; j < actors.size(); ++j)
		{
			// 当たり判定関数を呼び出すために共有ポインタに変換
			auto a = actors[i].lock();
			auto b = actors[j].lock();

			// 当たり判定のパターン
			// 現在一回何かしら当たったら終わるようになっているが、すべての場合使えるようにしたい
			if (IsCollisionRectToRect(a->GetColRect(), b->GetColRect()))
			{
				a->IsCollision({ b->GetType(),Types::ColliderType::Rect,Types::ColliderType::Rect });
				b->IsCollision({ a->GetType(),Types::ColliderType::Rect,Types::ColliderType::Rect });
			}
			else if (IsCollisionCircleToRect(a->GetColCircle(), b->GetColRect()))
			{
				a->IsCollision({ b->GetType(),Types::ColliderType::Circle,Types::ColliderType::Rect });
				b->IsCollision({ a->GetType(),Types::ColliderType::Rect,Types::ColliderType::Circle });
			}
			else if (IsCollisionCircleToCircle(a->GetColCircle(), b->GetColCircle()))
			{
				a->IsCollision({ b->GetType(),Types::ColliderType::Circle,Types::ColliderType::Circle });
				b->IsCollision({ a->GetType(),Types::ColliderType::Circle,Types::ColliderType::Circle });
			}
		}
	}
}

bool CollisionManager::IsCollisionCircleToCircle(const Circle2D& first, const Circle2D& second)
{
	// 2つの円の距離を測る
	Vector2 delta = first.pos - second.pos;

	// 対象の円と自身の円の半径の和を測る
	float distance = first.radius + second.radius;

	// 距離の二乗が半径の和の二乗以下なら当たっている
	return delta.SqrMagnitude() <= distance * distance;
}

bool CollisionManager::IsCollisionRectToRect(const Rect2D& first, const Rect2D& second)
{
	// 確実に当たっていない状況を判定する
	if (first.GetLeft() > second.GetRight())	return false;
	if (first.GetTop() > second.GetBottom())	return false;
	if (first.GetRight() < second.GetLeft())	return false;
	if (first.GetBottom() < second.GetTop())	return false;

	// 条件をすべて満たしていなければ当たっている
	return true;
}

bool CollisionManager::IsCollisionCircleToRect(const Circle2D& circle, const Rect2D& rect)
{
	// 円の中心座標
	float circleX = circle.pos.x;
	float circleY = circle.pos.y;

	// 矩形の四隅の情報
	float left = rect.GetLeft();
	float right = rect.GetRight();
	float top = rect.GetTop();
	float bottom = rect.GetBottom();

	// 円と矩形の最近点
	float nearX;
	float nearY;

	if (circleX > right)
	{
		nearX = right;
	}
	else if (circleX < left)
	{
		nearX = left;
	}
	else
	{
		nearX = circleX;
	}

	if (circleY < top)
	{
		nearY = top;
	}
	else if (circleY > bottom)
	{
		nearY = bottom;
	}
	else
	{
		nearY = circleY;
	}

	// 最近点と円の中心の距離
	float distanceX = circleX - nearX;
	float distanceY = circleY - nearY;

	// 距離の二乗が半径の二乗以下なら当たっている
	return (distanceX * distanceX) + (distanceY * distanceY) <= (circle.radius * circle.radius);
}
