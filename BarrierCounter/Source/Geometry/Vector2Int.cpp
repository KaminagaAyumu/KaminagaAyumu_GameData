#include "Vector2Int.h"
#include <cmath>
#include <cassert>

Vector2Int::Vector2Int()
{
    x = 0;
    y = 0;
}

Vector2Int::Vector2Int(int inX, int inY)
{
    x = inX;
    y = inY;
}

void Vector2Int::Set(int newX, int newY)
{
    x = newX;
    y = newY;
}

Vector2Int Vector2Int::operator+() const
{
    return Vector2Int(x, y);
}

Vector2Int Vector2Int::operator+(const Vector2Int& val) const
{
    return Vector2Int(x + val.x, y + val.y);
}

Vector2Int Vector2Int::operator-() const
{
    return Vector2Int(-x, -y);
}

Vector2Int Vector2Int::operator-(const Vector2Int& val) const
{
    return Vector2Int(x - val.x, y - val.y);
}

Vector2Int Vector2Int::operator*(const int& len) const
{
    return Vector2Int(x * len, y * len);
}

void Vector2Int::operator+=(const Vector2Int& val)
{
    x += val.x;
    y += val.y;
}

void Vector2Int::operator-=(const Vector2Int& val)
{
    x -= val.x;
    y -= val.y;
}

void Vector2Int::operator*=(const int& len)
{
    x *= len;
    y *= len;
}

bool Vector2Int::operator==(const Vector2Int& val) const
{
    return (x == val.x) && (y == val.y);
}