#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2
{
	float x;
	float y;

	/// <summary>
	/// ベクトルの長さを返す
	/// </summary>
	/// <returns>ベクトルの長さ</returns>
	float Magnitude();
	/// <summary>
	/// ベクトルの2乗の長さを返す
	/// </summary>
	/// <returns>ベクトルの2乗の長さ</returns>
	float SqrMagnitude();

	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	void Normalize();

	/// <summary>
	/// ベクトルの値を設定する
	/// </summary>
	/// <param name="newX">設定するX成分</param>
	/// <param name="newY">設定するY成分</param>
	void Set(float newX, float newY);

	/// <summary>
	/// ベクトルの内積を返す
	/// </summary>
	/// <param name="right"></param>
	/// <returns>内積</returns>
	float Dot(const Vector2& right) const;

	/// <summary>
	/// 2つのベクトルの内積を返す
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <returns>内積</returns>
	float Dot(const Vector2& left, const Vector2& right) const;

	/// <summary>
	/// ベクトルの外積を返す
	/// </summary>
	/// <param name="right"></param>
	/// <returns>外積</returns>
	float Cross(const Vector2& right) const;

	/// <summary>
	/// 2つのベクトルの外積を返す
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <returns>外積</returns>
	float Cross(const Vector2& left, const Vector2& right) const;

	// 演算子オーバーロード
	Vector2 operator+() const;
	Vector2 operator+(const Vector2& val) const;
	Vector2 operator-() const;
	Vector2 operator-(const Vector2& val) const;
	Vector2 operator*(const Vector2& val) const;
	Vector2 operator*(const float& len) const;
	Vector2 operator/(const float& len) const;
	void operator+=(const Vector2& val);
	void operator-=(const Vector2& val);
	void operator*=(const float& len);
	void operator/=(const float& len);
	bool operator==(const Vector2& val) const;

};

// 座標情報で使用するために別名をつける
using Position2 = Vector2;
//typedef Vector2 Position2;

struct Position2Int
{
	int x;
	int y;
};

/// <summary>
/// サイズを格納する構造体
/// </summary>
struct Size
{
	int width;
	int height;
};


/// <summary>
/// 2次元の円を表す構造体
/// </summary>
struct Circle2D
{
	Position2 pos; // 中心座標
	float radius; // 半径
	void Draw();
	/// <summary>
	/// 座標指定で表示する
	/// </summary>
	/// <param name="posX">X座標</param>
	/// <param name="posY">Y座標</param>
	void Draw(int posX, int posY);
};

/// <summary>
/// 2次元の矩形を表す構造体
/// </summary>
struct Rect2D
{
	Rect2D();

	Rect2D(Position2 pos_, float width_, float height_);
	/// <summary>
	/// 四辺を受け取って初期化する
	/// </summary>
	/// <param name="left">左端</param>
	/// <param name="right">右端</param>
	/// <param name="top">上端</param>
	/// <param name="bottom">下端</param>
	Rect2D(float left, float right, float top, float bottom);
	Position2 pos; // 中心座標
	float width; // 幅
	float height; // 高さ
	void Draw();
	void Draw(unsigned int color);
	/// <summary>
	/// 座標指定で表示する
	/// </summary>
	/// <param name="posX">X座標</param>
	/// <param name="posY">Y座標</param>
	void Draw(int posX, int posY);

	// 上下左右の座標を取得する関数
	float GetLeft()const;
	float GetRight()const;
	float GetTop()const;
	float GetBottom()const;

	// 矩形の角の座標を取得する関数
	Position2 GetLeftTop()const;
	Position2 GetLeftBottom()const;
	Position2 GetRightTop()const;
	Position2 GetRightBottom()const;
};

/// <summary>
/// 座標をアクティブかどうかの状態付きで指定できる構造体
/// </summary>
struct ActivePosition2
{
	Position2 pos = {};
	bool isActive = false;
};


namespace Geometry
{
	/// <summary>
	/// ベクトルを2点の間で線形補完する
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="t">割合</param>
	/// <returns>補完したベクトル</returns>
	Vector2 LerpVec2(const Vector2& start, const Vector2& end, float t);

	/// <summary>
	/// 2つの位置間の距離を計算する
	/// </summary>
	/// <param name="from">始点の位置</param>
	/// <param name="to">終点の位置</param>
	/// <returns>2つの位置間の距離の大きさ</returns>
	float GetDistance(const Position2& from, const Position2& to);

	/// <summary>
	/// 余りを正の数にして返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>余りの数(正の数)</returns>
	int RemainderToNaturalNumber(int left, int right);

	/// <summary>
	/// 余りを正の数にして返す(浮動小数点版)
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>余りの数(正の数)</returns>
	float RemainderToNaturalNumberF(float left, float right);
};