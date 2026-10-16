#include "ComboCounter.h"
#include "DxLib.h"
#include <algorithm>

namespace
{
	// コンボがつながる時間
	constexpr int kComboChainTime = 180;
}

ComboCounter::ComboCounter() : 
	m_comboCount(0),
	m_maxComboCount(0),
	m_comboTimer(0)
{
}

void ComboCounter::Update()
{
	m_comboTimer--;

	if (m_comboTimer <= 0)
	{
		m_comboTimer = 0;
		m_comboCount = 0;
	}
}

void ComboCounter::AddKill()
{
	m_comboCount++;
	m_comboTimer = kComboChainTime;
	m_maxComboCount = std::max(m_comboCount, m_maxComboCount);
}

const float ComboCounter::GetRemainRate()
{
	return static_cast<float>(m_comboTimer) / kComboChainTime;
}
