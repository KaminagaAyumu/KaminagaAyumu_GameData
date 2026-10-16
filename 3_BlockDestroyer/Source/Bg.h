#pragma once
/// <summary>
/// 背景を管理するクラス
/// </summary>
class Bg
{
public:
	Bg();
	~Bg();

	void Init(int handle);
	void End();
	void Update();
	void Draw();

private:
	// メンバ変数
	int m_bgHandle; // 背景ハンドル


};

