#pragma once
#include <array>
#include "DxLib.h"

namespace
{
	constexpr int kKeyNum = 256; // キーの数
}

class InputManager
{
public:
	/// <summary>
	/// InputManagerのインスタンスを取得する
	/// 複製不可
	/// </summary>
	/// <returns>InputManagerのインスタンス</returns>
	static InputManager& Instance();

	void Update(); // 入力状態を更新する

	// キー入力状態を取得する
	bool GetKeyDown(int keyCode) const; // キーが押された瞬間にtrueを返す
	bool GetKey(int keyCode) const; // キーが押されている間trueを返す
	bool GetKeyUp(int keyCode) const; // キーが離された瞬間にtrueを返す

	// マウス入力状態を取得する
	bool GetMouseDown(int button) const; // マウスボタンが押された瞬間にtrueを返す
	bool GetMouse(int button) const; // マウスボタンが押されている間trueを返す
	bool GetMouseUp(int button) const; // マウスボタンが離された瞬間にtrueを返す

	// パッド入力状態を取得する
	bool GetPadDown(int button) const; // パッドのボタンが押された瞬間にtrueを返す
	bool GetPad(int button) const; // パッドのボタンが押されている間trueを返す
	bool GetPadUp(int button) const; // パッドのボタンが離された瞬間にtrueを返す

private:
	// コンストラクタ
	InputManager();
	// デストラクタ　特に使用しないためdefault
	~InputManager() = default;

	std::array<char, kKeyNum> m_keyNow; // 現在のキー入力状態
	std::array<char, kKeyNum> m_keyPrev; // 前回のキー入力状態

	int m_mouseNow; // 現在のマウス入力状態
	int m_mousePrev; // 前回のマウス入力状態

	int m_padNow; // 現在のパッド入力状態
	int m_padPrev; // 前回のパッド入力状態
};

