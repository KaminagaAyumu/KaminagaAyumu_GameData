#include "Input.h"
#include "DxLib.h"


namespace
{
	constexpr int kKeyNum = 256;

	// スティックのデッドゾーン
	constexpr int kMinStickValue = 3000;
	constexpr int kMaxStickValue = 28000;
	// トリガーのデッドゾーン
	constexpr int kMinTriggerValue = 50;
	constexpr int kMaxTriggerValue = 200;

	// トリガーの最大値
	constexpr int kTriggerMax = 255;
}

Input::~Input()
{
}

Input& Input::GetInstance()
{
	static Input instance;
	return instance;
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
	m_inputTable["CameraChange"] = { { InputType::Keyboard, KEY_INPUT_Y }, { InputType::Pad, PAD_INPUT_X } }; // Yキー、Yボタン
	m_inputTable["Pause"] = { { InputType::Keyboard, KEY_INPUT_P }, { InputType::Pad, PAD_INPUT_R } }; // Pキー、STARTボタン
	m_inputTable["Avoid"] = { { InputType::Keyboard, KEY_INPUT_Z }, { InputType::Pad, PAD_INPUT_A } }; // Zキー、Aボタン
	m_inputTable["OpenMap"] = { { InputType::Keyboard, KEY_INPUT_C }, { InputType::Pad, PAD_INPUT_C } }; // Cキー、Xボタン

	// 入力データの初期化
	// 入力種別ごとにすべてのボタンが押されていないとする
	for (const auto& inputInfo : m_inputTable)
	{
		m_currentInputState[inputInfo.first] = false;
		m_lastInputState[inputInfo.first] = false;
	}
}

void Input::UpdateStickData()
{
	// XInputの入力値を保存できる構造体(DXLib内構造体)
	XINPUT_STATE stickData;
	// パッドの入力値を取得する
	int state = GetJoypadXInputState(DX_INPUT_PAD1, &stickData);

	// パッドの取得ができなかったらすべての値を0にする
	if (state == -1)
	{
		m_stickTable.leftTrigger = 0.0f;
		m_stickTable.rightTrigger = 0.0f;
		m_stickTable.leftStick.x = 0.0f;
		m_stickTable.leftStick.y = 0.0f;
		m_stickTable.rightStick.x = 0.0f;
		m_stickTable.rightStick.y = 0.0f;
		// ここで処理を終わる
		return;
	}

	// 取得した入力値をスティックテーブル内に保存
	m_stickTable.leftTrigger = static_cast<float>(stickData.LeftTrigger);
	m_stickTable.rightTrigger = static_cast<float>(stickData.RightTrigger);
	m_stickTable.leftStick.x = static_cast<float>(stickData.ThumbLX);
	m_stickTable.leftStick.y = static_cast<float>(stickData.ThumbLY);
	m_stickTable.rightStick.x = static_cast<float>(stickData.ThumbRX);
	m_stickTable.rightStick.y = static_cast<float>(stickData.ThumbRY);

	// スティック、トリガーのデッドゾーンを設定
	ApplyDeadZone(true, false);
	ApplyDeadZone(false, false);
	ApplyDeadZone(false, true);
	ApplyDeadZone(true, true);
}

void Input::ApplyDeadZone(bool isRight, bool isTrigger)
{
	// 右側のスティックかトリガーの場合
	if (isRight)
	{
		// 右側のトリガーの場合
		if (isTrigger)
		{
			// トリガーの入力値を取得
			float len = m_stickTable.rightTrigger;

			// トリガーの最少入力値と最大入力値までの割合を求める
			float lenRate = (len - kMinTriggerValue) / (kMaxTriggerValue - kMinTriggerValue);
			// 0~1までの値に収める
			if (lenRate < 0.0f) lenRate = 0.0f;
			if (lenRate > 1.0f) lenRate = 1.0f;

			// 0~1までの割合にしたトリガーの値を設定
			m_stickTable.rightTrigger = m_stickTable.rightTrigger * lenRate / kTriggerMax;
		}
		// 右側のスティックの場合
		else
		{
			// スティックの入力値を取得
			float len = m_stickTable.rightStick.Length();

			// スティックの最少入力値と最大入力値までの割合を求める
			float lenRate = (len - kMinStickValue) / (kMaxStickValue - kMinStickValue);
			// 0~1までの値に収める
			if (lenRate < 0.0f) lenRate = 0.0f;
			if (lenRate > 1.0f) lenRate = 1.0f;

			// 0~1までの割合にしたスティックの値を設定
			m_stickTable.rightStick.x = m_stickTable.rightStick.x / len * lenRate;
			m_stickTable.rightStick.y = m_stickTable.rightStick.y / len * lenRate;
		}
	}
	// 左側のスティックかトリガーの場合
	else
	{
		// 左側のトリガーの場合
		if (isTrigger)
		{
			float len = m_stickTable.leftTrigger;

			float lenRate = (len - kMinTriggerValue) / (kMaxTriggerValue - kMinTriggerValue);
			if (lenRate < 0.0f) lenRate = 0.0f;
			if (lenRate > 1.0f) lenRate = 1.0f;

			m_stickTable.leftTrigger = m_stickTable.leftTrigger * lenRate / kTriggerMax;
		}
		// 左側のスティックの場合
		else
		{
			float len = m_stickTable.leftStick.Length();

			float lenRate = (len - kMinStickValue) / (kMaxStickValue - kMinStickValue);
			if (lenRate < 0.0f) lenRate = 0.0f;
			if (lenRate > 1.0f) lenRate = 1.0f;

			m_stickTable.leftStick.x = m_stickTable.leftStick.x / len * lenRate;
			m_stickTable.leftStick.y = m_stickTable.leftStick.y / len * lenRate;
		}
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

	// スティックの情報を更新する
	UpdateStickData();

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

const StickData& Input::GetStickData() const
{
	return m_stickTable;
}
