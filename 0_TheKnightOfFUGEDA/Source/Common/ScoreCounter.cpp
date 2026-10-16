#include "ScoreCounter.h"
#include "../MyLib/MyMath.h"

ScoreCounter::ScoreCounter() : 
	m_score(0),
	m_displayedScore(0)
{
}

ScoreCounter::~ScoreCounter()
{
}

void ScoreCounter::AddScore(int score)
{
	m_score += score;
}

void ScoreCounter::Update()
{
	// 表示用のスコアの加算処理
	if (m_displayedScore < m_score)
	{
		MyLib::UpdateParam(m_displayedScore, m_score);
	}
}
