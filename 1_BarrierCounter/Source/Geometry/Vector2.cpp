#include "Vector2.h"
#include <cmath>
#include <cassert>

Vector2::Vector2()
{
    x = 0.0f;
    y = 0.0f;
}

Vector2::Vector2(float inX, float inY)
{
    x = inX;
    y = inY;
}

float Vector2::Length()
{
    return std::hypotf(x, y);
}

float Vector2::SqrLength()
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
    if (Length() == 0.0f)
    {
        return; // 0の時は正規化しない
    }
    x /= Length();
    y /= Length();
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
    assert(len != 0.0f && "0除算を検知しました");
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
    assert(len != 0.0f && "0除算を検知しました");
    x /= len;
    y /= len;
}

bool Vector2::operator==(const Vector2& val) const
{
    return (x == val.x) && (y == val.y);
}