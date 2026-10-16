#pragma once
#include "EventStructs.h"
#include "EventControls.h"
#include "EventSensors.h"

using namespace Events;

class Input;

/// <summary>
/// ゲーム中のイベントを管理するクラス
/// </summary>
class EventManager
{
public:
	EventManager();
	virtual ~EventManager();

	void Init(std::vector<Events::EventData> events, const std::shared_ptr<EventControls>& controls, const std::shared_ptr<EventSensors>& sensors);

	void Update();

private:

	int m_eventIndex; // イベントの進行状況
	std::vector<Events::EventData> m_eventData;
	std::weak_ptr<EventControls> m_pControls;
	std::weak_ptr<EventSensors> m_pSensors;

	/// <summary>
	/// イベントトリガーを判定
	/// </summary>
	/// <param name="data">イベントデータ</param>
	/// <returns>true : トリガー発火 false : トリガーが発火していない</returns>
	bool CheckTrigger(const Events::EventData& data);

	/// <summary>
	/// 指定されたイベントを行う
	/// </summary>
	/// <param name="data">イベントデータ</param>
	void RunAction(const Events::EventData& data);
};

