#pragma once
#include "UIBase.h"
#include "../Geometry.h"

/// <summary>
/// ゲージを表示するUIクラス
/// </summary>
class UIGauge : public UIBase
{
public:
	UIGauge();
	virtual ~UIGauge();
	

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="frameHandle">枠画像ハンドル</param>
	/// <param name="fillHandle">ゲージの中身のハンドル</param>
	void SetHandle(int frameHandle, int fillHandle);

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="size">ゲージのサイズ</param>
	/// <param name="pos">ゲージを表示する中心座標</param>
	void Init(const Size& size, const Position2& pos);
	
	void Update()override;
	
	void Draw()const override;
	
	bool IsAlive()const override;

	/// <summary>
	/// ゲージの割合を設定
	/// </summary>
	/// <param name="value">ゲージの割合</param>
	void SetValue(float value);

	/// <summary>
	/// 枠画像のアクティブ状態を変更する
	/// </summary>
	/// <param name="isActive">true : 表示 false : 非表示</param>
	void SetActive(bool isActive) { m_isActive = isActive; }

private:

	int m_frameHandle; // 枠画像ハンドル
	int m_fillHandle; // ゲージの中身のハンドル

	float m_value; // ゲージの量(0.0~1.0)
	float m_visualValue; // 表示するゲージの量

	int m_fillSrcW;
	int m_fillSrcH;

	bool m_isActive;

	Position2 m_pos; // 表示座標(左上座標)
	Size m_size; // ゲージの大きさ

};

