#include "Animation.h"

namespace
{
	int kAnimNextCount = 6; // アニメーションの更新間隔（フレーム数）
}

Animation::Animation() :
	m_graphHandle(-1),
	m_frameCount(0),
	m_maxAnimCount(0),
	m_animCount(0),
	m_isPlaying(false),
	m_pos(0, 0),
	m_animSize(0, 0)
{
}

void Animation::Init(int graphHandle, int maxAnimCount, Vector2 pos, Vector2 animSize)
{
	m_graphHandle = graphHandle;
	m_frameCount = 0;
	m_maxAnimCount = maxAnimCount;
	m_pos = pos;
	m_animSize = animSize;
}
void Animation::Update()
{
	if (!m_isPlaying)
	{
		return;
	}

	m_frameCount++;

	if (m_frameCount >= kAnimNextCount)
	{
		m_animCount++;
		m_frameCount = 0; // フレームカウントをリセット
	}

	if (m_animCount > m_maxAnimCount)
	{
		m_animCount = 0; // アニメーションカウントをリセット
		m_isPlaying = false; // アニメーションを停止
	}

}

void Animation::Draw()
{
	if (!m_isPlaying)
	{
		return; // アニメーションが再生されていない場合は描画しない
	}
	DrawRectGraphF(
		m_pos.X, m_pos.Y,// 表示する座標
		m_animSize.X * m_animCount, 0, // 表示する矩形の左上座標
		m_animSize.X, m_animSize.Y, // 表示する矩形の幅と高さ
		m_graphHandle, // グラフィックハンドル
		TRUE);
}

void Animation::PlayOneShot()
{
	m_animCount = 0; // アニメーションカウントをリセット
	m_frameCount = 0; // フレームカウントをリセット
	m_isPlaying = true; // アニメーションを再生
}


