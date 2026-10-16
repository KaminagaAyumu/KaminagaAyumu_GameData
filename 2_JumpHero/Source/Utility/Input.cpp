#include "Input.h"
#include "DxLib.h"

namespace
{
	constexpr int kKeyNum = 256;
}

Input::Input()
{
	// ボタンのイベント名と入力の内容をここで初期化する
	m_inputTable["OK"] = { { InputType::Keyboard, KEY_INPUT_RETURN }, { InputType::Pad, PAD_INPUT_R | PAD_INPUT_A } }; // Enterキー、STARTボタンもしくはAボタン
	m_inputTable["Up"] = { { InputType::Keyboard, KEY_INPUT_UP }, { InputType::Pad, PAD_INPUT_UP } };
	m_inputTable["Down"] = { { InputType::Keyboard, KEY_INPUT_DOWN }, { InputType::Pad, PAD_INPUT_DOWN } };
	m_inputTable["Left"] = { { InputType::Keyboard, KEY_INPUT_LEFT }, { InputType::Pad, PAD_INPUT_LEFT } };
	m_inputTable["Right"] = { { InputType::Keyboard, KEY_INPUT_RIGHT }, { InputType::Pad, PAD_INPUT_RIGHT } };
	m_inputTable["Jump"] = { { InputType::Keyboard, KEY_INPUT_Z }, { InputType::Pad, PAD_INPUT_A } }; // Zキー、Aボタン
	m_inputTable["LShift"] = { { InputType::Keyboard, KEY_INPUT_LSHIFT }, { InputType::Pad, PAD_INPUT_Y } }; // Lシフトキー、Lボタン
	m_inputTable["RShift"] = { { InputType::Keyboard, KEY_INPUT_RSHIFT }, { InputType::Pad, PAD_INPUT_Z } }; // Rシフトキー、Rボタン
	m_inputTable["PowerUp"] = { { InputType::Keyboard, KEY_INPUT_X }, { InputType::Pad, PAD_INPUT_B } }; // Xキー、Bボタン
	m_inputTable["Pause"] = { { InputType::Keyboard, KEY_INPUT_P }, { InputType::Pad, PAD_INPUT_R } }; // Pキー、STARTボタン

	// 入力データの初期化
	// 入力種別ごとにすべてのボタンが押されていないとする
	for (const auto& inputInfo : m_inputTable)
	{
		m_currentInputState[inputInfo.first] = false;
		m_lastInputState[inputInfo.first] = false;
	}
}

void Input::Update()
{
	// 現在の入力情報を取得
	char keyState[kKeyNum]; // キーボードの入力状態保存用配列
	GetHitKeyStateAll(keyState); // キーボードの入力状態を取得
	// イベント名に対応する入力情報を取得
	int padState = GetJoypadInputState(DX_INPUT_PAD1); // パッドの入力状態を取得
	m_lastInputState = m_currentInputState; // 前のフレームの入力状態を更新

	// 各イベントの入力をチェックする
	for (const auto& inputInfo : m_inputTable) // すべての入力分のループ
	{
		auto& input = m_currentInputState[inputInfo.first]; // イベント名に対応する現在の入力状態
		for (const auto& state : inputInfo.second) // イベント名に対応するすべての入力種別分のループ
		{
			// 入力種別ごとに処理を分ける
			switch (state.type)
			{
			case InputType::Keyboard:
				input = keyState[state.inputId];
				break;
			case InputType::Pad:
				input = (padState & state.inputId);
				break;
			case InputType::Mouse:
				// マウスの入力処理はここに追加
				break;
			}
			if (input)
			{
				break; // いずれかの入力が検出されたらループを抜ける
			}
		}
	}
}

bool Input::IsTriggered(const std::string& name) const
{
	// イベント名に対応する入力情報を取得
	auto keyInfo = m_inputTable.find(name);
	if (keyInfo == m_inputTable.end())
	{
		return false; // イベント名が存在しない場合、falseを返す
	}
	// イベント名が存在する場合、現在の状態と前の状態を比較して押された瞬間かどうかを判定
	return m_currentInputState.at(name) && !m_lastInputState.at(name);
}

bool Input::IsPressed(const std::string& name) const
{
	auto keyInfo = m_inputTable.find(name);
	if (keyInfo == m_inputTable.end())
	{
		return false; // イベント名が存在しない場合、falseを返す
	}
	// イベント名が存在する場合、現在の状態を返す
	return m_currentInputState.at(name);

}
