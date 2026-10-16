#pragma once
#include "DxLib.h"
#include <vector>
#include <array>
#include <algorithm>

namespace
{
	constexpr int kNumberGraphNum = 10; // 数字の画像の数
	constexpr int kNumberGraphWidth = 75; // 数字の画像の幅
	constexpr int kNumberGraphHeight = 95; // 数字の画像の高さ
}

class UI
{
public:
	UI();
	virtual ~UI() = default;

	void Init();
	void End();
	void Update();
	void Draw();

	/// <summary>
	/// 数字のUIを表示する関数
	/// </summary>
	/// <param name="number">表示したい数字</param>
	/// <param name="x">UIの開始左上座標</param>
	/// <param name="y">UIの開始左上座標</param>
	/// <param name="scale">UIの大きさ</param>
	void DrawNumber(int number, int x, int y, float scale);

	/// <summary>
	/// 数字のUIを表示する関数
	/// </summary>
	/// <param name="number">表示したい数字</param>
	/// <param name="x">UIの開始左上座標</param>
	/// <param name="y">UIの開始左上座標</param>
	/// <param name="width">UIの横幅</param>
	/// <param name="height">UIの縦幅</param>
	void DrawNumber(int number, int x, int y, int width, int height);

	/// <summary>
	/// 数字のUIを表示する関数(サイズ指定版)
	/// </summary>
	/// <param name="number">表示したい数字</param>
	/// <param name="x">UIの開始左上座標</param>
	/// <param name="y">UIの開始左上座標</param>
	/// <param name="width">UI全体の横幅</param>
	/// <param name="height">UI全体の縦幅</param>
	void DrawNumberInSize(int number, int x, int y, int width, int height);

	/// <summary>
	/// 数字のUIを表示する関数(サイズ指定版、中央寄せ)
	/// </summary>
	/// <param name="number">表示したい数字</param>
	/// <param name="x">UIの開始左上座標</param>
	/// <param name="y">UIの開始左上座標</param>
	/// <param name="width">UI全体の横幅</param>
	/// <param name="height">UI全体の縦幅</param>
	void DrawNumberInSizeCenter(int number, int x, int y, int width, int height);

	/// <summary>
	/// ゲージUIを表示する関数
	/// </summary>
	/// <param name="x">UIの開始左上座標</param>
	/// <param name="y">UIの開始左上座標</param>
	/// <param name="width">UIの横幅</param>
	/// <param name="height">UIの縦幅</param>
	/// <param name="rate">ゲージの割合最大から1.0~0.0</param>
	void DrawGauge(int x, int y, int width, int height, float rate);

private:
	std::array<int, kNumberGraphNum> m_numberGraphHandles; // 数字の画像ハンドル

};

