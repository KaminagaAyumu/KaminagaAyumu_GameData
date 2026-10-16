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

	void Update();
	void Draw() const;

	/// <summary>
	/// イベントをセットする
	/// </summary>
	/// <param name="controls"></param>
	/// <param name="sensors"></param>
	void SetEvents(const std::shared_ptr<EventControls>& controls, const std::shared_ptr<EventSensors>& sensors);

	/// <summary>
	/// 指定されたステージのイベントデータをロードする
	/// </summary>
	/// <param name="stageNo">ステージ番号</param>
	/// <param name="isMiniGame">ミニゲームかどうか</param>
	bool LoadEventData(int stageNo, bool isMiniGame);

	/// <summary>
	/// 指定されたステージの共通イベントデータをロードする
	/// </summary>
	/// <param name="stageNo">ステージ番号</param>
	/// <param name="isMiniGame">ミニゲームかどうか</param>
	bool LoadCommonEventData(int stageNo, bool isMiniGame);

	/// <summary>
	/// ゲームが止まっているかどうかを返す
	/// </summary>
	/// <returns>true : 止まっている false : 止まっていない</returns>
	bool IsFreezeGame()const { return m_isFreezeGame; };

private:

	int m_eventIndex; // イベントの進行状況
	int m_timeCount; // 経過時間を測るイベントを行うときに使う
	bool m_isWaitingInput; // テキストウィンドウのページをめくるときに使う
	bool m_isActionHold; // イベントの更新を止めるフラグ
	bool m_isFreezeGame; // ゲームを止めるフラグ

	std::weak_ptr<EventControls> m_pControls;
	std::weak_ptr<EventSensors> m_pSensors;

	std::weak_ptr<UITextWindow> m_currentTextWindow; // ウィンドウを表示するイベントで使う

	std::weak_ptr<UITextWindow> m_currentHintWindow; // ヒントを表示するイベントで使う

	std::vector<EventData> m_eventData; // イベントデータ群
	std::vector<CommonEventData> m_commonEventData; // 共通イベントデータ群

	/// <summary>
	/// パラメータの数値を整数型に変換する関数
	/// </summary>
	/// <param name="param">パラメータの数値文字列</param>
	/// <returns>整数化した数</returns>
	int GetParamNum(const std::string& param);

	/// <summary>
	/// イベントが終了したかどうかを判定する
	/// </summary>
	/// <param name="eventNo">イベントの番号</param>
	/// <returns>true : イベントが終わっている false : イベントが終わっていない</returns>
	bool IsEventEnd(int eventNo) const;

	/// <summary>
	/// イベント条件を文字列から変換する用の関数
	/// </summary>
	/// <param name="strData">文字列データ</param>
	/// <returns>変換後のイベント条件</returns>
	TriggerType ToTriggerType(const std::string& strData);

	/// <summary>
	/// イベントで行う内容を文字列から変換する用の関数
	/// </summary>
	/// <param name="strData">文字列データ</param>
	/// <returns>変換後のイベント内容</returns>
	ActionType ToActionType(const std::string& strData);

	/// <summary>
	/// イベントトリガーを判定
	/// </summary>
	/// <param name="data">イベントデータ</param>
	/// <returns>true : トリガー発火 false : トリガーが発火していない</returns>
	bool CheckTrigger(const EventData& data);

	/// <summary>
	/// 指定されたイベントを行う
	/// </summary>
	/// <param name="data">イベントデータ</param>
	void RunAction(const EventData& data);

	/// <summary>
	/// 指定された共通イベントを行う
	/// </summary>
	/// <param name="data">イベントデータ</param>
	void RunCommonAction(const EventData& data);

};

