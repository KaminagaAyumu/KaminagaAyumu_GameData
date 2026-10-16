#pragma once
#include <string>

// イベント関連
namespace Events
{
	// イベントが始まる条件
	enum class TriggerType
	{
		GameStart, // ゲームが始まった
		EnterArea, // エリアに入った
		TimeElapsed, // 時間が経過した
		EventEnd, // イベントが終わった
		OpenChest, // 宝箱を開けた
		GetItem, // アイテムを取得した
		EnemySpawn, // 敵がスポーンした
		PlayerPowerUp, // プレイヤーがパワーアップ状態になった
		GetBalloon, // 風船をすべて取得した
		NoTrigger, // 条件なし
	};

	// イベントで何を行うか
	enum class ActionType
	{
		ShowHint, // ヒントを表示する
		ShowText, // テキストを表示する
		DropItem, // アイテムを生成する
		SpawnEnemy, // 敵をスポーンさせる
		FreezeGame, // ゲームを一時停止する
		UnFreezeGame, // ゲームを再開する
		FreezePlayer, // プレイヤーを止める
		UnFreezePlayer, // プレイヤーを止める
		LookCamera, // カメラが特定の場所を見る
		ReturnCamera, // カメラが戻ってくる
		PowerUp, // プレイヤーの強化
		WaitInput, // ボタン入力待ち
		SetBarrier, // 移動制限バリアを設定
		UnlockBarrier, // 移動制限バリアを開放
		ActiveGoal, // ゴールを開く
		NoAction, // 何もしない
	};

	/// <summary>
	/// ゲーム内で順番に行われるイベントをまとめる構造体
	/// </summary>
	struct EventData
	{
		int id = 0; // 識別ID
		TriggerType triggerType = TriggerType::NoTrigger; // イベントが始まる条件
		std::string triggerParam; // イベントの場合分けで必要なデータ
		ActionType actionType = ActionType::NoAction; // 行われるイベント
		std::string actionParam; // イベントで必要なデータ
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