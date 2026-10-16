#include "Block.h"

namespace
{
	int kMaxEffectCount = 7; // 最大エフェクト数
}

Vector2 Block::GetLeftTop() const
{
    return Vector2(Pos.X - Rect.X, Pos.Y - Rect.Y);
}

Vector2 Block::GetRightTop() const
{
    return Vector2(Pos.X + Rect.X, Pos.Y - Rect.Y);
}

Vector2 Block::GetLeftBottom() const
{
    return Vector2(Pos.X - Rect.X, Pos.Y + Rect.Y);
}

Vector2 Block::GetRightBottom() const
{
    return Vector2(Pos.X + Rect.X, Pos.Y + Rect.Y);
}
Vector2 Block::LeftVec() const
{
	// 右回りなので左上 - 左下
	return Vector2(GetLeftTop() - GetLeftBottom());
}
Vector2 Block::RightVec() const
{
	// 右回りなので右下 - 右上
	return Vector2(GetRightBottom() - GetRightTop());
}
Vector2 Block::TopVec() const
{
	// 右回りなので右上 - 左上
	return Vector2(GetRightTop() - GetLeftTop());
}
Vector2 Block::BottomVec() const
{
	// 右回りなので左下 - 右下 
	return Vector2(GetLeftBottom() - GetRightBottom());
}
void Block::Init(Vector2 pos, Vector2 rect,Vector2 effectAnimSize, int drawHandle, int seHandle, int effectHandle)
{
	Pos = pos;
	Rect = rect;
	EffectAnimSize = effectAnimSize;
	EffectPos = Vector2(Pos.X - EffectAnimSize.X / 2, Pos.Y - EffectAnimSize.Y / 2);
	DrawHandle = drawHandle;
	SeHandle = seHandle;
	EffectHandle = effectHandle;
	IsExist = true;
	Animation.Init(EffectHandle, kMaxEffectCount, EffectPos, EffectAnimSize);
	Sound.Init(SeHandle);
}

void Block::Draw()
{
	if (IsExist)
	{
		DrawRotaGraph(static_cast<int>(Pos.X), static_cast<int>(Pos.Y), 1.0f, 0, DrawHandle, false);
		//DrawBox(Pos.X - Rect.X, Pos.Y - Rect.Y, Pos.X + Rect.X, Pos.Y + Rect.Y, GetColor(200, 200, 0), false);
	}
	Animation.Draw();
}

void Block::ChangeHandle(int newHandle)
{
	DrawHandle = newHandle;
}
