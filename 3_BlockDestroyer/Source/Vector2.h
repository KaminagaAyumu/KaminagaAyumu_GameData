#pragma once
struct Vector2
{

public:
	/*メンバ変数*/
	float X;
	float Y;

public:
	/*コンストラクタ(とデストラクタ)*/
	//デフォルトコンストラクタ
	Vector2() :X(0.0f),Y(0.0f){}
	//引数付きコンストラクタ
	Vector2(float posX, float posY) : X(posX),Y(posY){}
	//デストラクタ
	~Vector2() {}

	/*メンバ関数*/
	//ベクトルの大きさを返す
	float Length() const;
	//ベクトルの大きさの2乗を返す
	float Magnitude() const;
	//正規化されたベクトルを返す（単位ベクトルを返す）
	Vector2 Nomalize() const;

	/*staticメンバ変数*/
	// 内積を返す
	static float Dot(const Vector2& left, const Vector2& right);
	//射影ベクトル(leftからrightに射影)
	static Vector2 Project(const Vector2& left, const Vector2& right);
	// 疑似外積を返す
	// 正なら、AとBのなす角は左回(負の値なら右）
	//(※なす角とは0<θ<180)
	static float CrossZ(const Vector2& left, const Vector2& right);
};

/*演算子オーバーロード*/
//ベクトル同士の足し算
Vector2 operator+(const Vector2& left,const Vector2& right);
//ベクトル同士の引き算
Vector2 operator-(const Vector2& left,const Vector2& right);
// 実数*ベクトル
Vector2 operator*(const float& left,const Vector2& right);
// ベクトル*実数
Vector2 operator*(const Vector2& left, const float& right);
//ベクトル/実数
Vector2 operator/(const Vector2& left,const float& right);



