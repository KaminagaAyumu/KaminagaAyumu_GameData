#pragma once
#include "Vector2.h"
#include "DxLib.h"
#include "Animation.h"
#include "Sound.h"

/// <summary>
/// ブロックを構成する線分ベクトルは右回りとする
/// </summary>
struct Block
{ 
public:
	Vector2 Pos;// 中心座標	
	Vector2 Rect;// x:横幅の半分　y:縦幅の半分
	Vector2 EffectPos; // エフェクトの位置 （左上座標）
	Vector2 EffectAnimSize; // エフェクトのアニメーション1つのサイズ
	int DrawHandle;// 画像ハンドル
	int SeHandle; // SEハンドル
	int EffectHandle; // エフェクト画像ハンドル
	bool IsExist; // 存在フラグ
	Animation Animation; // アニメーション用
	Sound Sound; // サウンド用

	/*メンバ関数*/
	// ブロックの四隅の位置を取得
	// 左上の座標
	Vector2 GetLeftTop() const;
	// 右上の座標
	Vector2 GetRightTop() const;
	// 左下の座標
	Vector2 GetLeftBottom() const;
	// 右下の座標
	Vector2 GetRightBottom() const;
	// 左辺のベクトル
	Vector2 LeftVec() const;
	// 右辺のベクトル
	Vector2 RightVec() const;
	// 上辺のベクトル
	Vector2 TopVec() const;
	// 下辺のベクトル
	Vector2 BottomVec() const;

	// 第1引数に位置、第2引数に幅（x:横幅の半分　y:縦幅の半分）、第3引数にエフェクトアニメーション1つのサイズ
	void Init(Vector2 pos, Vector2 rect,Vector2 effectAnimSize, int drawHandle, int seHandle, int effectHandle);
	void Draw();

	void ChangeHandle(int newHandle);

};

