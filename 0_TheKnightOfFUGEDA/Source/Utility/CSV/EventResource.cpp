#include "EventResource.h"
#include "../../MyLib/MyString.h"

namespace
{
	// CSVの列番号
	constexpr int kTriggerColumn = 0;	// イベントが始まる条件
	constexpr int kActionColumn = 1;	// 行われるイベント
	constexpr int kParamColumn = 2;	// テキストIDなどのパラメータ
	constexpr int kBoolParamColumn = 3;	// bool型のパラメータ(0以外でtrue)
}

void EventResource::ConvertEventData()
{
	// データの数を取得する
	size_t size = GetDataCount();

	for (size_t i = 0; i < size; ++i)
	{
				// データを読み込む
		Events::TriggerType trigger = ToTriggerType(Read<std::wstring>(static_cast<int>(i), kTriggerColumn));
		Events::ActionType action = ToActionType(Read<std::wstring>(static_cast<int>(i), kActionColumn));
		std::wstring param = Read<std::wstring>(static_cast<int>(i), kParamColumn);
		bool boolParam = Read<int>(static_cast<int>(i), kBoolParamColumn) != 0;
		// イベントデータを作成
		Events::EventData data;
		data.trigger = trigger;
		data.action = action;
		data.param = param;
		data.boolParam = boolParam;
		m_eventDatas.push_back(data);
	}

	ReleaseRawData();
}

Events::TriggerType EventResource::ToTriggerType(std::wstring string)
{
	Events::TriggerType triggerType = Events::TriggerType::NoTrigger;
	if (string == L"GameStart")
	{
		triggerType = Events::TriggerType::GameStart;
	}
	else if (string == L"TextEnd")
	{
		triggerType = Events::TriggerType::TextEnd;
	}
	else if (string == L"CountDownEnd")
	{
		triggerType = Events::TriggerType::CountDownEnd;
	}
	else if (string == L"AllEnemyDead")
	{
		triggerType = Events::TriggerType::AllEnemyDead;
	}
	else if (string == L"AllItemGet")
	{
		triggerType = Events::TriggerType::AllItemGet;
	}
	else
	{
		triggerType = Events::TriggerType::NoTrigger;
	}

	return triggerType;
}

Events::ActionType EventResource::ToActionType(std::wstring string)
{
	Events::ActionType actionType = Events::ActionType::NoAction;

	if(string == L"ShowText")
	{
		actionType = Events::ActionType::ShowText;
	}
	else if (string == L"StartCountDown")
	{
		actionType = Events::ActionType::StartCountDown;
	}
	else if (string == L"FreezePlayer")
	{
		actionType = Events::ActionType::FreezePlayer;
	}
	else if (string == L"FreezeEnemy")
	{
		actionType = Events::ActionType::FreezeEnemy;
	}
	else if (string == L"GoToClearScene")
	{
		actionType = Events::ActionType::GoToClearScene;
	}
	else
	{
		actionType = Events::ActionType::NoAction;
	}

	return actionType;
}
