#include "EventManager.h"

EventManager::EventManager() : 
	m_eventIndex(0)
{
}

EventManager::~EventManager()
{
}

void EventManager::Init(std::vector<Events::EventData> events, const std::shared_ptr<EventControls>& controls, const std::shared_ptr<EventSensors>& sensors)
{
	m_eventData = std::move(events);
	m_pControls = controls;
	m_pSensors = sensors;
}

void EventManager::Update()
{
	if (m_eventIndex >= static_cast<int>(m_eventData.size())) return;

	if (CheckTrigger(m_eventData[m_eventIndex]))
	{
		RunAction(m_eventData[m_eventIndex]);
		m_eventIndex++;
	}
}

bool EventManager::CheckTrigger(const Events::EventData& data)
{
	auto sensors = m_pSensors.lock();

	if (!sensors) return false;

	switch (data.trigger)
	{
	case TriggerType::GameStart:
		return true;
		break;
	case TriggerType::TextEnd:
		return sensors->isTextFinishedFunc();
		break;
	case TriggerType::CountDownEnd:
		return sensors->isCountDownFinishedFunc();
		break;
	case TriggerType::AllEnemyDead:
		return sensors->isAllEnemyDeadFunc();
		break;
	case TriggerType::AllItemGet:
		return sensors->isAllItemGetFunc();
	default:
		return false;
		break;
	}

	return false;
}

void EventManager::RunAction(const Events::EventData& data)
{
	auto controls = m_pControls.lock();
	if (!controls) return;

	switch (data.action)
	{
	case ActionType::ShowText:
		controls->showTextFunc(data.param);
		break;
	case ActionType::StartCountDown:
		controls->startCountDownFunc();
		break;
	case ActionType::FreezePlayer:
		controls->setPlayerCanMoveFunc(data.boolParam);
		break;
	case ActionType::FreezeEnemy:
		controls->setEnemyCanActFunc(data.boolParam);
		break;
	case ActionType::GoToClearScene:
		controls->goToClearSceneFunc();
		break;
	default:
		break;
	}

}
