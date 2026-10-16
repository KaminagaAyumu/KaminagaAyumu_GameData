#pragma once
#include <functional>
#include <string>

/// <summary>
/// イベントを起こす条件をまとめた構造体
/// </summary>
struct EventSensors
{
	// プレイヤーが指定されたエリアに入ったかどうかの関数
	std::function<bool(int areaNo)> isEnterAreaFunc;

	// 指定された宝箱を開けたかどうかの関数
	std::function<bool(int chestNo)> isOpenChestFunc;

	// 指定されたアイテムを取得したかどうかの関数
	std::function<bool(const std::string& id)> isGetItemFunc;

	// プレイヤーが強化されているかどうかの関数
	std::function<bool()> isPowerUpFunc;

	// ボタンが押されたかどうかの関数
	std::function<bool()> isPressedButtonFunc;

	// カメラの補間が終わったかどうかの関数
	std::function<bool()> isCameraLerpEndFunc;

	// 風船の取得状態の関数
	std::function<bool(int balloonNum, bool isAll)> isGetBalloonFunc;

	// 敵のスポーン位置すべてを取得する関数 
	std::function<const std::vector<Position2>&()> getSpawnPositionsFunc;

	// 風船の総数を取得する関数
	std::function<int()> getTotalBalloonNumFunc;
};