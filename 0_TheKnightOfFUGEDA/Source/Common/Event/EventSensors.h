#pragma once
#include <functional>
#include <string>

/// <summary>
/// イベントを起こす条件をまとめた構造体
/// </summary>
struct EventSensors
{
	// テキストの表示が終わったかどうかの関数
	std::function<bool()> isTextFinishedFunc;

	// カウントダウンが終了したかどうかの関数
	std::function<bool()> isCountDownFinishedFunc;

	// 敵をすべて倒したかどうかの関数
	std::function<bool()> isAllEnemyDeadFunc;

	// アイテムをすべて取ったかどうかの関数
	std::function<bool()> isAllItemGetFunc;
};