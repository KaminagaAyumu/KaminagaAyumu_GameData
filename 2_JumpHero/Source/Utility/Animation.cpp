#include "Geometry.h"
#include "Animation.h"
#include "DxLib.h"

namespace
{
	constexpr int kAnimNumMin = 1; // アニメーションの最小枚数
}

Animation::Animation() : 
	m_handle(-1),
	m_animSize({ 0,0 }),
	m_animNum(0),
	m_animFrame(0),
	m_animIndex(0),
	m_isLoop(false),
	m_isEnd(false),
	m_animAngle(0.0f),
	m_animScale(1.0f),
	m_frameCount(0),
	m_animCount(0)
{
}

void Animation::SetAnimation(int handle, const Size& animSize, int animNum, int animFrame, bool isLoop)
{
	m_handle = handle;
	m_animSize = animSize;
	m_animNum = animNum;
	m_animFrame = animFrame;
	m_isLoop = isLoop;
	m_animIndex = 0;
	m_frameCount = 0;
}

void Animation::SetAnimation(int handle, const Size& animSize, int animIndex, int animNum, int animFrame, bool isLoop)
{
	m_handle = handle;
	m_animSize = animSize;
	m_animNum = animNum;
	m_animFrame = animFrame;
	m_isLoop = isLoop;
	m_animIndex = animIndex;
	m_frameCount = 0;
}

void Animation::Update()
{
	if (m_animNum <= kAnimNumMin)
	{
		return; // アニメーションが1枚以下なら更新しない
	}
	m_frameCount++;
	if (m_frameCount >= m_animFrame)
	{
		m_frameCount = 0;
		m_animCount++;
		if (m_animCount >= m_animNum)
		{
			if (m_isLoop)
			{
				m_animCount = 0; // ループするなら最初に戻る
			}
			else
			{
				m_isEnd = true;
				m_animCount = m_animNum - kAnimNumMin; // ループしないなら最後のフレームで止める(カウントが0から始まるため-1)
			}
		}
	}
}

void Animation::Draw(Position2Int pos, bool isTurn) const
{
	if (m_handle == -1)
	{
		return; // アニメーションが設定されていないなら描画しない
	}
	
	DrawRectRotaGraph(
		pos.x, pos.y, // 表示座標(中央)
		m_animCount * m_animSize.width, m_animIndex * m_animSize.height, // 画像の切り取り座標(左上)
		m_animSize.width, m_animSize.height, // 切り取るサイズ
		m_animScale, // 拡大率
		m_animAngle, // 回転角度
		m_handle, // 画像ハンドル
		true,
		isTurn); // 反転するかどうか
}

bool Animation::operator!=(const Animation& anim) const
{
	// 設定したアニメーションが異なっていたらtrueを返す
	if (m_handle != anim.m_handle) return true;
	if (m_animSize.width != anim.m_animSize.width) return true;
	if (m_animSize.height != anim.m_animSize.height) return true;
	if (m_animNum != anim.m_animNum) return true;
	if (m_animFrame != anim.m_animFrame) return true;
	// それ以外はfalseを返す
	return false;
}

