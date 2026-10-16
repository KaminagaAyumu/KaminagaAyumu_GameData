#pragma once
#include "UIBase.h"
#include "../../Geometry/Vector2Int.h"
#include "../../Geometry/Vector2.h"

class UIImage : public UIBase
{
public:
	UIImage();
	virtual ~UIImage();

	void Init()override;
	void End()override;
	void Update()override;
	void Draw()const override;

	bool IsAlive()const override;

	/// <summary>
	/// 画像ハンドルをセットする
	/// </summary>
	/// <param name="handle">画像のハンドル</param>
	void SetHandle(int handle) { m_handle = handle; };

	/// <summary>
	/// 枠画像ハンドルをセットする
	/// </summary>
	/// <param name="handle">枠画像のハンドル</param>
	void SetFrameHandle(int handle) { m_frameHandle = handle; }

	/// <summary>
	/// 表示する座標をセット
	/// </summary>
	/// <param name="pos">2D座標</param>
	void SetPos(const Vector2Int& pos) { m_pos = pos; }

	/// <summary>
	/// 表示する拡大率をセット
	/// </summary>
	/// <param name="scale">拡大率</param>
	void SetScale(const Vector2& scale) { m_scale = scale; }

	/// <summary>
	/// アニメーションをセットする
	/// </summary>
	/// <param name="animGraphSize">アニメーション1枚の画像サイズ</param>
	/// <param name="animNum">アニメーションの数</param>
	/// <param name="animFrame">アニメーションを更新するフレーム数</param>
	/// <param name="isLoop">ループするかどうか</param>
	void SetAnimation(const Vector2Int& animGraphSize, int animNum, int animFrame, bool isLoop);

	/// <summary>
	/// ループする背景をセットする
	/// </summary>
	/// <param name="loopFrame">ループするフレーム数</param>
	void SetLoopBackGround(int loopFrame);

	void StartAppearCenter(int appearFrame) override;

	/// <summary>
	/// 画像のタイプを設定
	/// </summary>
	enum class ImageType
	{
		Normal, // 通常描画
		Animation, // アニメーション描画
		LoopBackGround, // ループする背景画像
	};

private:
	// 画像ハンドル
	int m_handle;
	// 枠を使用する際の画像ハンドル
	int m_frameHandle;
	// 存在フラグ
	bool m_isAlive;
	// 表示座標(画像の中心)
	Vector2Int m_pos;
	// スクロールする際の表示座標
	Vector2Int m_scrollPos;
	// 表示サイズ
	Vector2Int m_graphSize;
	// 表示スケール
	Vector2 m_scale;
	// 画像タイプ
	ImageType m_type;
	// アニメーション1枚のサイズ
	Vector2Int m_animGraphSize;
	// アニメーションの数
	int m_animNum;
	// アニメーションのフレーム
	int m_animFrame;
	// 背景をループさせる際のフレーム数
	int m_loopFrame;
	// アニメーションのループを行うかのフラグ
	bool m_isLoop;
	// アニメーション用のフレームカウンタ
	int m_animFrameCount;
	// アニメーションの進行率カウンタ
	int m_animCount;

private:

	/// <summary>
	/// ループする背景を描画する際の処理
	/// </summary>
	void LoopBackGroundDraw() const;
};

