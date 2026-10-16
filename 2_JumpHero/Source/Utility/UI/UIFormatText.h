#pragma once
#include "UIBase.h"
#include "../Geometry.h"
#include <string>
#include <functional>


class UIFormatText : public UIBase
{
public:
	UIFormatText();
	virtual ~UIFormatText();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="handle">テキストのハンドル</param>
	/// <param name="text">表示するテキストの内容</param>
	/// <param name="pos">表示する座標</param>
	void Init(int handle, std::string text, const Position2& pos);
	void Update()override;
	void Draw()const override;
	bool IsAlive()const override;
	
	/// <summary>
	/// テキストを閉じる(消去)
	/// </summary>
	void Close();

	/// <summary>
	/// 表示するテキストを返すプロバイダをセットする
	/// </summary>
	/// <param name="provider">プロバイダ</param>
	void SetProvider(std::function<std::string()> provider);

	/// <summary>
	/// 中央ぞろえでテキストを表示するようにする
	/// </summary>
	void SetCenter();

	/// <summary>
	/// 文字列を1文字ずつ表示する
	/// </summary>
	/// <param name="typeFrame">文字が増えるフレーム</param>
	/// <param name="isFadeOut">終わった後にフェードアウトするか</param>
	void ShowTypewriter(int typeFrame, bool isFadeOut);

	/// <summary>
	/// 文字の表示が終わった後にフェードアウトする
	/// </summary>
	/// <param name="fadeWaitFrame">フェードアウトを行うまでのフレーム数</param>
	/// <param name="fadeDuration">フェードアウトを行うフレーム数</param>
	void SetFadeOut(int fadeWaitFrame, int fadeDuration);

private:
	Position2 m_pos; // 表示座標
	std::string m_text; // 表示するテキスト
	std::function<std::string()> m_provider; // テキストの内容を取得するプロバイダ
	int m_fontHandle; // フォントのハンドル
	int m_color; // 文字の色
	int m_frameCount; // 表示してからのフレーム数
	bool m_isAlive; // 存在フラグ
	bool m_isCenter; // 中央ぞろえでテキストを表示するかどうか
	bool m_isFadeOut; // フェードアウトを行うかどうか

	std::wstring m_wText; // 文字数を数えやすいようにワイド文字列にする
	int m_visibleTextNum; // 1文字ずつ表示する際に使う(現在の文字数)
	int m_typeFrame; // タイプ表示のフレーム数

	int m_fadeWaitFrame; // フェードアウトを行うまでの時間
	int m_fadeDuration; // フェードアウトの時間
	int m_fadeAlpha; // フェードアウトを行う際のアルファ値
	int m_fadeCount; // フェードをカウントするフレーム

	enum class FadeState
	{
		None,
		Waiting,
		Fading,
		Done
	};

	FadeState m_fadeState; // フェードの状態

	/// <summary>
	/// プロバイダからテキストデータを変更する
	/// </summary>
	void CheckProvider();

	/// <summary>
	/// 1文字ずつ出す文字を更新する
	/// </summary>
	void UpdateTypewriter();

	/// <summary>
	/// フェード関連の処理を更新する
	/// </summary>
	void UpdateFade();
};

