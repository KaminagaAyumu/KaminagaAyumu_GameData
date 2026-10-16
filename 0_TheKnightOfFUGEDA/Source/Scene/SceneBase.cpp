#include "SceneBase.h"

SceneBase::SceneBase(SceneController& controller) : 
	m_sceneController(controller),
	m_frameCount(0),
	m_fadeColor(0)
{
}