#pragma once
#include "UIBase.h"
#include "../Geometry.h"


class Animation;

/// <summary>
/// 画像を表示するクラス
/// </summary>
class UIImage : public UIBase
{
public:
	UIImage();
	virtual ~UIImage();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="handle">画像のハンドル</param>
	/// <param name="size">表示したいサイズ</param>
	/// <param name="pos">表示左上座標</param>
	void Init(int handle, const Size& size, const Position2& pos);

	void Update()override;

	void Draw()const override;

	bool IsAlive()const override;

	/// <summary>
	/// アニメーションをセットする
	/// </summary>
	/// <param name="animNum">アニメーション枚数</param>
	/// <param name="animFrame">アニメーションの更新を行うフレーム</param>
	/// <param name="isLoop">ループするかどうか(基本true)</param>
	void SetAnimation(int animNum, int animFrame, bool isLoop);

	/// <summary>
	/// 画像を閉じる
	/// </summary>
	void Close();

private:
	int m_handle; // 画像ハンドル
	Size m_size; // 画像のサイズ
	Position2 m_pos; // 画像を表示する座標

	bool m_isAlive; // 画像が表示されているかどうか

	std::shared_ptr<Animation> m_pAnimation; // アニメーションがある際に使用する

};

