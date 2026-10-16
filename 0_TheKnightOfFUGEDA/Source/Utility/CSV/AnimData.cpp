#include "AnimData.h"

AnimData::AnimData() : 
	m_animName(L""),
	m_animSpeed(0.0f),
	m_blendFrame(-1),
	m_isLoop(true)
{
}

AnimData::~AnimData()
{
}

void AnimData::SetData(const std::wstring& animName, float animSpeed, int blendFrame, bool isLoop)
{
	m_animName = animName;
	m_animSpeed = animSpeed;
	m_blendFrame = blendFrame;
	m_isLoop = isLoop;
}
