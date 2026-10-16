#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "../Geometry/Vector2.h"

struct StickData
{
	float leftTrigger;
	float rightTrigger;
	Vector2 leftStick;
	Vector2 rightStick;
};

/// <summary>
/// 入力管理用クラス
/// </summary>
class Input
{
public:
	virtual ~Input(); // デストラクタ

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static Input& GetInstance();

	/// <summary>
	/// 入力情報を更新する
	/// </summary>
	void Update();

	/// <summary>
	/// イベント名に対応するボタンが今押されたかどうか(押された瞬間のみ反応する)
	/// </summary>
	/// <param name="name">イベント名</param>
	/// <returns>true : 押されている false : 押されていない</returns>
	bool IsTriggered(const std::string& name)const;

	/// <summary>
	/// イベント名に対応するボタンが押されているかどうか
	/// </summary>
	/// <param name="name">イベント名</param>
	/// <returns>true : 押されている false : 押されていない</returns>
	bool IsPressed(const std::string& name)const;


	const StickData& GetStickData()const;

private:
	/// <summary>
	/// 入力種別
	/// </summary>
	enum class InputType
	{
		Keyboard,
		Pad,
		Mouse
	};

	/// <summary>
	/// すべての入力状態の管理用
	/// </summary>
	struct InputInfo
	{
		InputType type; // 入力種別
		int inputId; // 入力情報(種別ごと)
	};

	std::map<std::string, std::vector<InputInfo>> m_inputTable; // イベント名と入力情報のマップ
	std::map<std::string, bool> m_currentInputState; // 現在のフレームの入力状態
	std::map<std::string, bool> m_lastInputState; // 前のフレームの入力状態
	StickData m_stickTable; // スティックの入力状態のマップ

private:
	/// <summary>
	/// コンストラクタ
	/// 入力状態やキーやボタン等の割り振りを行う
	/// </summary>
	Input();

	Input(const Input&) = delete;
	void operator=(const Input&) = delete;

	/// <summary>
	/// XInputの入力データを更新する
	/// </summary>
	void UpdateStickData();

	/// <summary>
	/// デッドゾーンの設定をする
	/// </summary>
	/// <param name="isRight">右スティックの場合trueにする</param>
	/// <param name="isTrigger">トリガーボタンの場合trueにする</param>
	void ApplyDeadZone(bool isRight, bool isTrigger);

};
