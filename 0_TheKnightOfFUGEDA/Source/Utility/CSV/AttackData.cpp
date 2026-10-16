#include "AttackData.h"
#include "DxLib.h"

AttackData::AttackData() : 
	m_id(AttackID::None),
	m_startFrame(0),
	m_activeFrame(0),
	m_comboFrame(0),
	m_animChangeFrame(0),
	m_cancelFrame(0),
	m_initSpeed(0.0f),
	m_moveEndFrame(0),
	m_nextAttackID(AttackID::None),
	m_nextAnimName(L"")
{
}

void AttackData::SetData(AttackID id, int startFrame, int activeFrame, int comboFrame, int animChangeFrame, int cancelFrame, float initSpeed, int moveEndFrame, AttackID nextID, const std::wstring& nextAnim)
{
	m_id = id;
	m_startFrame = startFrame;
	m_activeFrame = activeFrame;
	m_comboFrame = comboFrame;
	m_animChangeFrame = animChangeFrame;
	m_cancelFrame = cancelFrame;
	m_initSpeed = initSpeed;
	m_moveEndFrame = moveEndFrame;
	m_nextAttackID = nextID;
	m_nextAnimName = nextAnim;
}