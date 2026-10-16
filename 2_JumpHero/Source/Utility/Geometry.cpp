#include "Geometry.h"
#include <cmath>
#include <cassert>
#include "DxLib.h"

float Vector2::Magnitude()
{
    return hypotf(x, y);
}

float Vector2::SqrMagnitude()
{
    return (x * x + y * y);
}

void Vector2::Set(float newX, float newY)
{
    x = newX;
    y = newY;
}

void Vector2::Normalize()
{
    if (Magnitude() == 0.0f)
    {
		return; // 0の時は正規化しない
    }
    x /= Magnitude();
    y /= Magnitude();
}

float Vector2::Dot(const Vector2& right) const
{
	return x * right.x + y * right.y;
}

float Vector2::Dot(const Vector2& left, const Vector2& right) const
{
	return left.x * right.x + left.y * right.y;
}

float Vector2::Cross(const Vector2& right) const
{
	return x * right.y - y * right.x;
}

float Vector2::Cross(const Vector2& left, const Vector2& right) const
{
	return left.x * right.y - left.y * right.x;
}

Vector2 Vector2::operator+() const
{
    return Vector2(x, y);
}

Vector2 Vector2::operator+(const Vector2& val) const
{
    return Vector2(x + val.x, y + val.y);
}

Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y); 
}

Vector2 Vector2::operator-(const Vector2& val) const
{
    return Vector2(x - val.x, y - val.y);
}

Vector2 Vector2::operator*(const Vector2& val) const
{
    return Vector2(x * val.x, y * val.y);
}

Vector2 Vector2::operator*(const float& len) const
{
    return Vector2(x * len, y * len);
}

Vector2 Vector2::operator/(const float& len) const
{
    // 0以外の時のみ計算する
    assert(len != 0.0f);
    return Vector2(x / len, y / len);
}

void Vector2::operator+=(const Vector2& val)
{
    x += val.x; 
    y += val.y; 
}

void Vector2::operator-=(const Vector2& val)
{
    x -= val.x;
    y -= val.y;
}

void Vector2::operator*=(const float& len)
{
    x *= len;
    y *= len; 
}

void Vector2::operator/=(const float& len)
{
    // 0以外の時のみ計算する
	assert(len != 0.0f);
	x /= len;
	y /= len;
}

bool Vector2::operator==(const Vector2& val) const
{
    return (x == val.x) && (y == val.y);
}

void Circle2D::Draw()
{
    DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(radius), 0xff0000, false);
}

void Circle2D::Draw(int posX, int posY)
{
    DrawCircle(posX, posY, static_cast<int>(radius), 0xff0000, false);
}

Rect2D::Rect2D():
    pos{},
    width(0),
    height(0)
{

}

Rect2D::Rect2D(Position2 pos_, float width_, float height_)
{
    pos = pos_;
    width = width_;
    height = height_;
}

Rect2D::Rect2D(float left, float right, float top, float bottom)
{
    pos.x = (left + right) * 0.5f;
    pos.y = (top + bottom) * 0.5f;
    width = right - left;
    height = bottom - top;
}

void Rect2D::Draw()
{
	DrawBox(static_cast<int>(GetLeft()), static_cast<int>(GetTop()),
		static_cast<int>(GetRight()), static_cast<int>(GetBottom()),
		0xff0000, false);
}

void Rect2D::Draw(unsigned int color)
{
    DrawBox(static_cast<int>(GetLeft()), static_cast<int>(GetTop()),
        static_cast<int>(GetRight()), static_cast<int>(GetBottom()),
        color, false);
}

void Rect2D::Draw(int posX, int posY)
{
    DrawBox(posX - static_cast<int>(width) / 2, posY - static_cast<int>(height) / 2, posX + static_cast<int>(width) / 2, posY + static_cast<int>(height) / 2, 0xff0000, false);
}

float Rect2D::GetLeft() const
{
    return (pos.x - width / 2);
}

float Rect2D::GetRight() const
{
    return (pos.x + width / 2);
}

float Rect2D::GetTop() const
{
    return (pos.y - height / 2);
}

float Rect2D::GetBottom() const
{
    return (pos.y + height / 2);
}

Position2 Rect2D::GetLeftTop() const
{
    return { GetLeft(), GetTop() };
}

Position2 Rect2D::GetLeftBottom() const
{
    return { GetLeft(), GetBottom() };
}

Position2 Rect2D::GetRightTop() const
{
    return { GetRight(), GetTop() };
}

Position2 Rect2D::GetRightBottom() const
{
    return { GetRight(), GetBottom() };
}

Vector2 Geometry::LerpVec2(const Vector2& start, const Vector2& end, float t)
{
    Vector2 ret;

    ret.x = std::lerp(start.x, end.x, t);
    ret.y = std::lerp(start.y, end.y, t);

    return ret;
}

float Geometry::GetDistance(const Position2& from, const Position2& to)
{
	return hypotf(to.x - from.x, to.y - from.y);
}

int Geometry::RemainderToNaturalNumber(int left, int right)
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

float Geometry::RemainderToNaturalNumberF(float left, float right)
{
    if(right <= 0.0f) return 0.0f;
    
    // 左辺と右辺の余りを求める
    float ret = std::fmod(left, right);

    // 余りが負の数だったら右辺の値を足した数を返す
    if (ret < 0)
    {
        return ret + right;
    }

    // ここを通ったら正の数の場合なのでそのまま返す
    return ret;
}
