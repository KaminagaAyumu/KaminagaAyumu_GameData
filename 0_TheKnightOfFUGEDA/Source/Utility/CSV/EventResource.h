#pragma once
#include "CSVResource.h"
#include "../../Common/Event/EventStructs.h"
#include <vector>
#include <string>

struct EventData;

class EventResource : public CSVResource
{
public:

	EventResource() = default;
	virtual ~EventResource() = default;

	void ConvertEventData();

	std::vector<Events::EventData>& GetEventDatas() { return m_eventDatas; }

private:

	// イベントのデータ
	std::vector<Events::EventData> m_eventDatas;

private:

	Events::TriggerType ToTriggerType(std::wstring string);
	Events::ActionType ToActionType(std::wstring string);

};

