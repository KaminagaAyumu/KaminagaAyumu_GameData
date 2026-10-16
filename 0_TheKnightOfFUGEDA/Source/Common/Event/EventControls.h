#pragma once
//#include "../TextManager.h"
//#include "../../Utility/Geometry.h"
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
	// テキストを表示する関数
	std::function<void(const std::wstring& id)> showTextFunc;

	// カウントダウンを始める関数
	std::function<void()> startCountDownFunc;

	// プレイヤーが動けるかどうかを設定する関数
	std::function<void(bool canMove)> setPlayerCanMoveFunc;
	
	// 敵が動けるかどうかを設定する関数
	std::function<void(bool canAct)> setEnemyCanActFunc;

	// クリアシーンに移行する関数
	std::function<void()> goToClearSceneFunc;

};