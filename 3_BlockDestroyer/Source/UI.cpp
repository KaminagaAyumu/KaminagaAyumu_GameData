#include "UI.h"


UI::UI()
{
	m_numberGraphHandles =
	{
		LoadGraph("data/0.png"),
		LoadGraph("data/1.png"),
		LoadGraph("data/2.png"),
		LoadGraph("data/3.png"),
		LoadGraph("data/4.png"),
		LoadGraph("data/5.png"),
		LoadGraph("data/6.png"),
		LoadGraph("data/7.png"),
		LoadGraph("data/8.png"),
		LoadGraph("data/9.png")
	}; // 数字の画像ハンドルを読み込む
}

void UI::Init()
{
	m_numberGraphHandles =
	{
		LoadGraph("data/0.png"),
		LoadGraph("data/1.png"),
		LoadGraph("data/2.png"),
		LoadGraph("data/3.png"),
		LoadGraph("data/4.png"),
		LoadGraph("data/5.png"),
		LoadGraph("data/6.png"),
		LoadGraph("data/7.png"),
		LoadGraph("data/8.png"),
		LoadGraph("data/9.png")
	}; // 数字の画像ハンドルを読み込む
}

void UI::End()
{
	for (int i = 0; i < kNumberGraphNum; i++)
	{
		DeleteGraph(m_numberGraphHandles[i]);
	}
}

void UI::Update()
{
}

void UI::Draw()
{
}

void UI::DrawNumber(int number, int x, int y, float scale)
{
	if (number == 0)
	{
		int w = static_cast<int>(kNumberGraphWidth * scale);
		int h = static_cast<int>(kNumberGraphHeight * scale);
		DrawExtendGraph(x, y, x + w, y + h, m_numberGraphHandles[0], true);
		return;
	}
	std::vector<int> digits;
	while (number > 0)
	{
		digits.push_back(number % 10);
		number /= 10;
	}
	int w = static_cast<int>(kNumberGraphWidth * scale);
	int h = static_cast<int>(kNumberGraphHeight * scale);

	for (int i = static_cast<int>(digits.size()) - 1; i >= 0; i--)
	{
		DrawExtendGraph(x, y, x + w, y + h, m_numberGraphHandles[digits[i]], true);
		x += w; // 次の数字の位置へ
	}
}

void UI::DrawNumber(int number, int x, int y, int width, int height)
{
	if (number == 0)
	{
		DrawExtendGraph(x, y, x + width, y + height, m_numberGraphHandles[0], true);
		return;
	}
	std::vector<int> digits;
	while (number > 0)
	{
		digits.push_back(number % 10);
		number /= 10;
	}
	for (int i = static_cast<int>(digits.size()) - 1; i >= 0; i--)
	{
		DrawExtendGraph(x, y, x + width, y + height, m_numberGraphHandles[digits[i]], true);
		x += width; // 次の数字の位置へ
	}
}

void UI::DrawNumberInSize(int number, int x, int y, int width, int height)
{
	// 数字を桁ごとに分解
	std::vector<int> digits;
	if (number == 0) {
		digits.push_back(0);
	}
	else {
		while (number > 0) {
			digits.push_back(number % 10);
			number /= 10;
		}
	}
	std::reverse(digits.begin(), digits.end()); // 左から右へ並べる

	int numDigits = static_cast<int>(digits.size());
	if (numDigits == 0) return;

	// 1桁あたりのサイズを計算
	int digitWidth = width / numDigits;
	int digitHeight = height;

	// 各桁を描画
	for (int i = 0; i < numDigits; ++i) {
		int drawX = x + digitWidth * i;
		DrawExtendGraph(drawX, y, drawX + digitWidth, y + digitHeight, m_numberGraphHandles[digits[i]], true);
	}
}

void UI::DrawNumberInSizeCenter(int number, int x, int y, int width, int height)
{
	// 数字を桁ごとに分解
	std::vector<int> digits;
	if (number == 0) {
		digits.push_back(0);
	}
	else {
		while (number > 0) {
			digits.push_back(number % 10);
			number /= 10;
		}
	}
	std::reverse(digits.begin(), digits.end()); // 左から右へ並べる
	int numDigits = static_cast<int>(digits.size());
	if (numDigits == 0) return;
	// 1桁あたりのサイズを計算
	int digitWidth = width / numDigits;
	int digitHeight = height;
	// 全体の幅を計算して中央寄せの開始位置を決定
	int totalWidth = digitWidth * numDigits;
	int startX = x + (width - totalWidth) / 2;
	// 各桁を描画
	for (int i = 0; i < numDigits; ++i) {
		int drawX = startX + digitWidth * i;
		DrawExtendGraph(drawX, y, drawX + digitWidth, y + digitHeight, m_numberGraphHandles[digits[i]], true);
	}
}

void UI::DrawGauge(int x, int y, int width, int height, float rate)
{
	int gaugeWidth = static_cast<int>(width * rate); // ゲージの割合を計算

	DrawBox(x, y, x + width, y + height, GetColor(50, 50, 50), true); // 背景
	DrawBox(x, y, x + gaugeWidth, y + height, GetColor(0, 255, 0), true); // ゲージ
}
