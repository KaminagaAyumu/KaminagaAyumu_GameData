#pragma once
#include "../TextManager.h"
#include "../../Utility/Geometry.h"
#include <functional>
#include <string>
#include <memory>
#include <vector>

class UITextWindow;

/// <summary>
/// イベントの内容をまとめた構造体
/// </summary>
struct EventControls
{
	// ヒントを表示する関数
	std::function<std::weak_ptr<UITextWindow>(const std::string& id, const Size& size, const Position2& pos, float duration)> showHintFunc;

	// テキストウィンドウを表示する関数
	std::function<std::weak_ptr<UITextWindow>(const std::string& id, const Size& size, const Position2& pos, float duration)> showTextWindowFunc;

	// アイテムをドロップする関数
	std::function<void(int chestNo, const std::string& itemType)> dropItemFunc;

	// 敵を生成する関数
	std::function<void(const std::vector<Position2>& pos, int formNo)> spawnEnemiesFunc;

	// プレイヤーのフリーズ状態を変更する関数
	std::function<void()> changePlayerFreezeFunc;

	// カメラが見るターゲットをセットする関数
	std::function<void(const std::string& key)> lookCameraFunc;

	// カメラが見るターゲットを戻す関数(基本的にプレイヤーに戻す)
	std::function<void()> returnCameraFunc;

	// バリアをアクティブ状態にしてセットする関数
	std::function<void(int barrierNo)> setBarrierActiveFunc;

	// バリアを非アクティブ状態にしてセットする関数
	std::function<void(int barrierNo)> setBarrierInActiveFunc;

	// ゴールを解放する関数
	std::function<void()> activeGoalFunc;

};