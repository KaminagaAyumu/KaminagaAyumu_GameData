#pragma once
#include <string>

// イベント関連
namespace Events
{
	// イベントが始まる条件
	enum class TriggerType
	{
		GameStart, // ゲームが始まった
		TextEnd, // テキスト表示が終わった
		CountDownEnd, // カウントダウンが終わった
		AllEnemyDead, // 敵を全滅させた
		AllItemGet, // アイテムをすべて取った
		NoTrigger, // 条件なし
	};

	// イベントで何を行うか
	enum class ActionType
	{
		ShowText, // テキストを表示する
		StartCountDown, // カウントダウンを開始する
		FreezePlayer, // プレイヤーを止める/戻す
		FreezeEnemy, // 敵を止める/戻す
		GoToClearScene, // クリアシーンへ遷移する
		NoAction, // 何もしない
	};

	/// <summary>
	/// ゲーム内で順番に行われるイベントをまとめる構造体
	/// </summary>
	struct EventData
	{
		TriggerType trigger = TriggerType::NoTrigger; // イベントが始まる条件
		ActionType action = ActionType::NoAction; // 行われるイベント
		std::wstring param; // テキストIDなど
		bool boolParam = false;
	};

	/// <summary>
	/// ゲーム内で条件を満たしたら行われるイベントをまとめる構造体
	/// </summary>
	struct CommonEventData : EventData
	{
		bool isOnce = false; // 一度しか行わないかどうか
		bool isInvoked = false; // 行われたかどうか(onceの際使う)
	};
}