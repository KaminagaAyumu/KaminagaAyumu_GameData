#pragma once

enum class FadeState
{
	None,       // フェードしていない
	FadeIn,    // フェードイン中
	FadeOut   // フェードアウト中
};

class FadeManager
{
public:
	// コンストラクタ
	FadeManager();
	// デストラクタ
	~FadeManager();

	// 初期化処理
	void Init();
	void Update();
	void Draw();
	
	/*メンバ関数*/

	// フェードインの開始
	void StartFadeIn();
	// フェードアウトの開始
	void StartFadeOut();

	// フェード処理を行っているかどうかを返す
	bool IsFading() const { return m_fadeState != FadeState::None; }
	bool IsFadeIn() const { return m_fadeState == FadeState::FadeIn; }
	bool IsFadeOut() const { return m_fadeState == FadeState::FadeOut; }


private:

	// メンバ変数

	FadeState m_fadeState; // フェードの状態

	int m_fadeAlpha; // フェードのアルファ値 0で透明 255で完全に不透明

	int m_fadeCount; // フェードのカウント

};

