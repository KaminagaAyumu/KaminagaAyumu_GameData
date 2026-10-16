#pragma once

/// <summary>
/// ゲーム内の敵を倒した際のコンボをカウントするクラス
/// </summary>
class ComboCounter
{
public:

	ComboCounter();
	virtual ~ComboCounter() = default;

	void Update();

	void AddKill();

	const int GetCount() const { return m_comboCount; };

	const int GetMaxCount() const { return m_maxComboCount; }

	const float GetRemainRate();

private:

	int m_comboCount;

	int m_maxComboCount;

	int m_comboTimer;

};

