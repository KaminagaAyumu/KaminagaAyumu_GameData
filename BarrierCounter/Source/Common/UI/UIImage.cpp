#include "UIImage.h"
#include "DxLib.h"
#include "../../MyLib/MyMath.h"
#include "../Game.h"
#include <cmath>

namespace
{
	// アニメーションの最少枚数
	constexpr int kMinAnimNum = 1;
	// フェードの最大値
	constexpr int kMaxFadeRate = 255;
}

UIImage::UIImage() :
	m_handle(-1),
	m_frameHandle(-1),
	m_isAlive(true),
	m_pos{},
	m_scrollPos{},
	m_graphSize{},
	m_scale{ 1.0f, 1.0f },
	m_type(ImageType::Normal),
	m_animGraphSize{},
	m_animNum(0),
	m_animFrame(0),
	m_loopFrame(0),
	m_isLoop(true),
	m_animFrameCount(0),
	m_animCount(0)
{
}

UIImage::~UIImage()
{
}

void UIImage::Init()
{
	// 変数に画像のサイズを取得
	GetGraphSize(m_handle, &m_graphSize.x, &m_graphSize.y);
	// スケールは1がデフォルト
	m_scale = { 1.0f,1.0f };
}

void UIImage::End()
{
	m_isAlive = false;
}

void UIImage::Update()
{
	// UIの更新フレームを増加
	m_uiUpdateFrameCount++;

	// フェード中の場合
	if (IsFade())
	{
		// フレームカウンタを増加
		m_frameCount++;
		// フレームカウンタが設定のフレーム数に達したら
		if (m_frameCount >= m_fadeFrame)
		{
			// フェードアウトの場合
			if (m_fadeState == FadeState::FadeOut)
			{
				// 非アクティブに変更
				m_isActive = false;
				// 消去するかどうかを判別
				m_isAlive = !m_isAfterDelete;
				// これ以下の処理はフェードイン中のみ行う
				return;
			}

			// 通常の描画処理に変更
			m_fadeState = FadeState::Normal;
		}
	}

	// アニメーションする画像の場合の処理
	if (m_type == ImageType::Animation)
	{
		// アニメーションが1枚以下なら更新しない
		if (m_animNum <= kMinAnimNum){ return; }
		m_animFrameCount++;

		// アニメーションを進めるフレームになった時
		if (m_animFrameCount >= m_animFrame)
		{
			// カウンタをリセット
			m_animFrameCount = 0;
			// アニメーションのカウントを1つ進める
			m_animCount++;
			// 最後のアニメーションの時
			if (m_animCount >= m_animNum)
			{
				// ループする場合は最初のアニメーションに戻す
				if (m_isLoop)
				{
					m_animCount = 0;
				}
				else
				{
					// 最後のアニメーションで止める
					m_animCount = m_animNum - kMinAnimNum;
				}
			}

		}
	}
	

	
	// 画像が中央から出てくる際の更新処理
	if (m_uiState == UIState::AppearCenter)
	{
		// 画像のサイズを取得するための変数
		Vector2Int size;
		// 変数に画像のサイズを取得
		GetGraphSize(m_handle, &m_graphSize.x, &size.y);

		// 出現割合の計算 開始時: 0.0f  終了時: 1.0f
		auto appearRate = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_appearFrame);

		m_graphSize.y = static_cast<int>(std::lerp(m_graphSize.y, size.y, appearRate));

		if (appearRate >= 1.0f)
		{
			m_uiState = UIState::Normal;
		}
	}
	else if (m_uiState == UIState::CloseCenter)
	{
		// 画像のサイズを取得するための変数
		Vector2Int size;
		// 変数に画像のサイズを取得
		GetGraphSize(m_handle, &m_graphSize.x, &size.y);

		// 出現割合の計算 開始時: 0.0f  終了時: 1.0f
		auto appearRate = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_appearFrame);

		m_graphSize.y = static_cast<int>(std::lerp(m_graphSize.y, 0, appearRate));

		// 画像のサイズが0以下になった場合
		if (m_graphSize.y <= 0)
		{
			// 非アクティブに変更
			m_isActive = false;
			// 消去するかどうかを判別
			m_isAlive = !m_isAfterDelete;
		}
	}
	else
	{
		// 変数に画像のサイズを取得
		GetGraphSize(m_handle, &m_graphSize.x, &m_graphSize.y);
	}

	// ループさせる背景を描画する際の処理
	if (m_type == ImageType::LoopBackGround)
	{
		// 表示する座標を加算
		m_pos.x += m_loopFrame;
		m_pos.y += m_loopFrame;

		// スクロールした上での座標を計算
		// 基準からどれだけスクロールしたかどうか
		// 例:画像サイズが1280*720でm_posが{1400,1400}の時スクロール座標は{120,680}
		m_scrollPos = { -MyLib::RemainderToNaturalNumber(m_pos.x, m_graphSize.x),
			-MyLib::RemainderToNaturalNumber(m_pos.y, m_graphSize.y) };
	}

}

void UIImage::Draw() const
{
	// フェード状態ならばアルファブレンドに設定
	if (IsFade())
	{
		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
		auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_fadeFrame);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate - kMaxFadeRate * rate));
	}
	// タイプによって描画方法が変わる
	switch (m_type)
	{
	case UIImage::ImageType::Normal:
	{
		// 画像を描画
		DrawExtendGraph(
			m_pos.x - (m_graphSize.x * m_scale.x) / 2,
			m_pos.y - (m_graphSize.y * m_scale.y) / 2,
			m_pos.x + (m_graphSize.x * m_scale.x) / 2,
			m_pos.y + (m_graphSize.y * m_scale.y) / 2,
			m_handle, true);
	}
		break;
	case UIImage::ImageType::Animation:
		// 画像を描画
		DrawRectRotaGraph3(
			m_pos.x, m_pos.y, // 表示座標
			m_animCount * m_animGraphSize.x, 0, // 画像の切り取り座標
			m_animGraphSize.x, m_animGraphSize.y, // 描画するサイズ
			m_animGraphSize.x / 2, m_animGraphSize.y / 2, // 画像の回転の中心
			m_scale.x,m_scale.y, // 画像のスケール
			0.0f, // 回転率
			m_handle,
			true
		);
		break;
	case UIImage::ImageType::LoopBackGround:
		LoopBackGroundDraw();
		break;
	default:
		break;
	}

	// 枠のハンドルが存在する場合
	if (m_frameHandle != -1)
	{
		// 枠の画像を描画
		DrawExtendGraph(
			m_pos.x - (m_graphSize.x * m_scale.x) / 2,
			m_pos.y - (m_graphSize.y * m_scale.y) / 2,
			m_pos.x + (m_graphSize.x * m_scale.x) / 2,
			m_pos.y + (m_graphSize.y * m_scale.y) / 2,
			m_frameHandle, true);
	}

	// フェード状態で設定したブレンド描画を戻す
	if (IsFade())
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

bool UIImage::IsAlive() const
{
	return m_isAlive;
}

void UIImage::SetAnimation(const Vector2Int& animGraphSize, int animNum, int animFrame, bool isLoop)
{
	// アニメーションで使用する変数をセット
	m_type = ImageType::Animation;
	m_animGraphSize = animGraphSize;
	m_animNum = animNum;
	m_animFrame = animFrame;
	m_isLoop = isLoop;
	m_animFrameCount = 0;
	m_animCount = 0;
}

void UIImage::SetLoopBackGround(int loopFrame)
{
	m_type = ImageType::LoopBackGround;
	m_loopFrame = loopFrame;
}

void UIImage::StartAppearCenter(int appearFrame)
{
	m_uiState = UIState::AppearCenter;
	m_appearFrame = appearFrame;
	m_uiUpdateFrameCount = 0;
	m_graphSize.y = 0;
}

void UIImage::LoopBackGroundDraw() const
{
	// 左上
	DrawGraph(m_scrollPos.x, m_scrollPos.y, m_handle, true);

	// 画像のはみ出している部分を補う描画をするかどうか
	const bool needX = (m_scrollPos.x + m_graphSize.x < Game::kScreenWidth);
	const bool needY = (m_scrollPos.y + m_graphSize.y < Game::kScreenHeight);

	// X座標がはみ出している場合右側に新たに画像を描画
	if (needX)
	{
		// 右上
		DrawGraph(m_scrollPos.x + m_graphSize.x, m_scrollPos.y, m_handle, true);
	}

	// Y座標がはみ出している場合下側に新たに画像を描画
	if (needY)
	{
		// 左下
		DrawGraph(m_scrollPos.x, m_scrollPos.y + m_graphSize.y, m_handle, true);
	}

	// 両方の座標がはみ出している場合右下側に新たに画像を描画
	if (needX && needY)
	{
		// 右下
		DrawGraph(m_scrollPos.x + m_graphSize.x, m_scrollPos.y + m_graphSize.y, m_handle, true);
	}
}
