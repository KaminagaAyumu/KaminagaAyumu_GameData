#include "ColliderBase.h"

MyLib::ColliderBase::ColliderBase(ColliderShape shape, ObjectTag tag, const std::string& name, bool isTrigger) : 
	m_shape(shape),
	m_tag(tag),
	m_name(name),
	m_isTrigger(isTrigger),
	m_isEnable(true),
	m_localOffset{ Vector3::Zero() },
	m_localRotOffset{ Quaternion::Identity() }
{
}
