#include "UIText.h"
#include "DxLib.h"
#include "../Game.h"

namespace
{
	// フェードの最大値
	constexpr int kMaxFadeRate = 255;

	// デフォルトのテキストの色(白)
	constexpr unsigned int kDefaultTextColor = 0xffffff;

	// テキストに影をつける際のテキストからの補正位置
	constexpr int kTextShadowMargin = 4;
}

UIText::UIText() : 
	m_handle(-1),
	m_graphHandle(-1),
	m_textColor(kDefaultTextColor),
	m_isAlive(true),
	m_pos{},
	m_alignmentType(AlignmentType::Center)
{
}

UIText::~UIText()
{
}

void UIText::Init()
{
	m_isAlive = true;

	// テキストのサイズを取得するための変数
	Vector2Int size;

	// 幅を取得
	size.x = GetDrawStringWidthToHandle(m_text.c_str(), static_cast<int>(m_text.size()), m_handle);
	// 高さを取得
	size.y = GetFontSizeToHandle(m_handle);

	m_graphHandle = MakeScreen(Game::kScreenWidth, Game::kScreenHeight, TRUE);
}

void UIText::End()
{
	m_isAlive = false;

	DeleteGraph(m_graphHandle);
}

void UIText::Update()
{
	//DrawGraphHandle();

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
}

void UIText::Draw() const
{
	// HACK:テキストをそのまま描画しているが、
	// アルファブレンドの際後ろの影が見えてしまうので違和感を感じる

	// テキストのサイズを取得するための変数
	Vector2Int size;

	// 幅を取得
	size.x = GetDrawStringWidthToHandle(m_text.c_str(), static_cast<int>(m_text.size()), m_handle);
	// 高さを取得
	size.y = GetFontSizeToHandle(m_handle);

	// 表示する座標
	Vector2Int pos = m_pos;
	
	// y座標は文字の中心基準にする
	pos.y -= size.y / 2;

	// 文字の配置パターンを確認
	switch (m_alignmentType)
	{
	case UIText::AlignmentType::Left:
		// デフォルトが左上なので変更しない
		break;
	case UIText::AlignmentType::Right:
		// 文字全体の右にする
		pos.x -= size.x;
		break;
	case UIText::AlignmentType::Center:
		// 文字全体の中央にする
		pos.x -= size.x / 2;
		break;
	default:
		break;
	}

	// フェード状態なら
	if (IsFade())
	{
		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
		auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_fadeFrame);
		// フェードアウト中は徐々に透明にする
		if (m_fadeState == FadeState::FadeOut)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate - kMaxFadeRate * rate));
		}
		else // それ以外の場合はフェードイン中のため透明から徐々に不透明にする
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
		}
		// テキストを描画
		DrawStringToHandle(pos.x + kTextShadowMargin, pos.y + kTextShadowMargin, m_text.c_str(), 0x000000, m_handle);
		DrawStringToHandle(pos.x, pos.y, m_text.c_str(), m_textColor, m_handle);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		if (m_uiState == UIState::Blinking)
		{
			// フェード率の計算 開始時: 0.0f  終了時: 1.0f
			auto sinRate = 1.0f - sinf(static_cast<float>(m_uiUpdateFrameCount) / (m_blinkFrame) * DX_PI_F * 2);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * sinRate));
		}
		// 通常のテキストを描画
		DrawStringToHandle(pos.x + kTextShadowMargin, pos.y + kTextShadowMargin, m_text.c_str(), 0x000000, m_handle);
		DrawStringToHandle(pos.x, pos.y, m_text.c_str(), m_textColor, m_handle);

		if (m_uiState == UIState::Blinking)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	//// 画像を使う場合のDraw(αブレンドの違和感はないが画質が悪め)
	//// フェード状態なら
	//if (IsFade())
	//{
	//	// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	//	auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_fadeFrame);
	//	// フェードアウト中は徐々に透明にする
	//	if (m_fadeState == FadeState::FadeOut)
	//	{
	//		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate - kMaxFadeRate * rate));
	//	}
	//	else // それ以外の場合はフェードイン中のため透明から徐々に不透明にする
	//	{
	//		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * rate));
	//	}
	//	// テキストを描画
	//	DrawGraph(0,0, m_graphHandle, true);
	//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//}
	//else
	//{
	//	if (m_uiState == UIState::Blinking)
	//	{
	//		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
	//		auto sinRate = 1.0f - sinf(static_cast<float>(m_uiUpdateFrameCount) / (m_blinkFrame) * DX_PI_F * 2);
	//		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * sinRate));
	//	}
	//	// 通常のテキストを描画
	//	// テキストを描画
	//	DrawGraph(0,0, m_graphHandle, true);

	//	if (m_uiState == UIState::Blinking)
	//	{
	//		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//	}
	//}

}

bool UIText::IsAlive() const
{
	return m_isAlive;
}

void UIText::SetAlignment(AlignmentType type)
{
	m_alignmentType = type;
}

void UIText::DrawGraphHandle()
{
	// 描画対象をミニマップのものにする
	SetDrawScreen(m_graphHandle);

	// 前のフレームに描画した内容をクリアする
	ClearDrawScreen();

	// テキストのサイズを取得するための変数
	Vector2Int size;

	// 幅を取得
	size.x = GetDrawStringWidthToHandle(m_text.c_str(), static_cast<int>(m_text.size()), m_handle);
	// 高さを取得
	size.y = GetFontSizeToHandle(m_handle);

	// 表示する座標
	Vector2Int pos = m_pos;

	// y座標は文字の中心基準にする
	pos.y -= size.y / 2;

	// 文字の配置パターンを確認
	switch (m_alignmentType)
	{
	case UIText::AlignmentType::Left:
		// デフォルトが左上なので変更しない
		break;
	case UIText::AlignmentType::Right:
		// 文字全体の右にする
		pos.x -= size.x;
		break;
	case UIText::AlignmentType::Center:
		// 文字全体の中央にする
		pos.x -= size.x / 2;
		break;
	default:
		break;
	}

	// テキストを描画
	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawStringToHandle(pos.x + kTextShadowMargin, pos.y + kTextShadowMargin, m_text.c_str(), 0x000000, m_handle);
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawStringToHandle(pos.x, pos.y, m_text.c_str(), m_textColor, m_handle);

	// 描画先を元の画面に戻す
	SetDrawScreen(DX_SCREEN_BACK);

}
