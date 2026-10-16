#pragma once
#include "../Utility/Geometry.h"
#include <vector>
#include <memory>

class Actor;

/// <summary>
/// 当たり判定管理用クラス
/// </summary>
class CollisionManager
{
public:
	/// <summary>
	/// ゲーム内すべてのアクターと当たり判定を行う
	/// </summary>
	/// <param name="actors">ゲーム内のアクター</param>
	void CheckCollision(const std::vector<std::weak_ptr<Actor>>& actors);

private:

	/// <summary>
	/// 2つの円の当たり判定
	/// </summary>
	/// <param name="first">円1</param>
	/// <param name="second">円2</param>
	/// <returns>true : 当たっている false : 当たっていない</returns>
	bool IsCollisionCircleToCircle(const Circle2D& first, const Circle2D& second);

	/// <summary>
	/// 2つの矩形の当たり判定(回転を考慮しない)
	/// </summary>
	/// <param name="first">矩形1</param>
	/// <param name="second">矩形2</param>
	/// <returns>true : 当たっている false : 当たっていない</returns>
	bool IsCollisionRectToRect(const Rect2D& first, const Rect2D& second);

	/// <summary>
	/// 円と矩形の当たり判定
	/// </summary>
	/// <param name="circle">円</param>
	/// <param name="rect">矩形</param>
	/// <returns>true : 当たっている false : 当たっていない</returns>
	bool IsCollisionCircleToRect(const Circle2D& circle,const Rect2D& rect);

};

