#pragma once
class Vector2Int
{
public:
	int x;
	int y;

	Vector2Int();
	Vector2Int(int inX,int inY);

	/// <summary>
	/// ベクトルの値を設定する
	/// </summary>
	/// <param name="newX">設定するX成分</param>
	/// <param name="newY">設定するY成分</param>
	void Set(int newX, int newY);

	/// <summary>
	/// すべての値が0のベクトルを返す
	/// </summary>
	/// <returns></returns>
	static const Vector2Int Zero() { return Vector2Int{ 0,0 }; }

	// 演算子オーバーロード
	Vector2Int operator+() const;
	Vector2Int operator+(const Vector2Int& val) const;
	Vector2Int operator-() const;
	Vector2Int operator-(const Vector2Int& val) const;
	Vector2Int operator*(const int& len) const;
	void operator+=(const Vector2Int& val);
	void operator-=(const Vector2Int& val);
	void operator*=(const int& len);
	bool operator==(const Vector2Int& val) const;
};

