#pragma once
#include <string>

/// <summary>
/// 攻撃のデータ
/// </summary>
class AttackData
{
public:
	/// <summary>
	/// 攻撃のID
	/// </summary>
	enum class AttackID : uint8_t
	{
		None = 0,
		Attack1 = 1,
		Attack2 = 2,
		Attack3 = 3
	};
public:
	AttackData();
	virtual ~AttackData() = default;

	// データをセットする
	void SetData(AttackID id, int startFrame, int activeFrame, int comboFrame, int animChangeFrame, int cancelFrame, float initSpeed, int moveEndFrame, AttackID nextID, const std::wstring& nextAnim);

	/// <summary>
	/// 攻撃の名前を取得
	/// </summary>
	/// <returns></returns>
	AttackID GetAttackID() const { return m_id; }

	/// <summary>
	/// 次の攻撃の名前を取得
	/// </summary>
	/// <returns></returns>
	AttackID GetNextAttackID() const { return m_nextAttackID; }

	std::wstring GetNextAnimName() const { return m_nextAnimName; }

	int GetAnimChangeFrame() const { return m_animChangeFrame; }

	float GetInitSpeed() const { return m_initSpeed; }

	int GetMoveEndFrame() const { return m_moveEndFrame; }

	// 攻撃を始められるかどうかをチェック
	bool IsStart(int frame) const { return m_startFrame <= frame; }

	// 攻撃が発生しているかどうかをチェック
	bool IsActive(int frame) const { return m_activeFrame >= frame; }

	// コンボが繋がるフレーム内かをチェック
	bool IsCombo(int frame) const { return m_comboFrame >= frame; }

	// キャンセル可能かどうかをチェック
	bool IsCanselable(int frame) const { return m_cancelFrame <= frame; }

private:
	AttackID m_id;					// 攻撃ID
	int m_startFrame;				// 発生フレーム
	int m_activeFrame;				// 持続フレーム
	int m_comboFrame;				// コンボが繋がるフレーム
	int m_animChangeFrame;			// 次のアニメーションに進めるフレーム
	int m_cancelFrame;				// キャンセル可能なフレーム
	float m_initSpeed;				// 攻撃の初速度
	int m_moveEndFrame;				// 移動しなくなるフレーム数
	AttackID m_nextAttackID;		// 次の攻撃のID
	std::wstring m_nextAnimName;	// 次のアニメーションの名前
};

