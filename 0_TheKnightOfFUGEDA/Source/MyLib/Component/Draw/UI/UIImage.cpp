#include "UIImage.h"
#include "../../../GameObject.h"
#include "../../../Renderer.h"
#include "../../../../Geometry/Vector2Int.h"
#include "../../../../Utility/File/File.h"
#include "../../../../Utility/File/FileManager.h"
#include "../../../../Utility/Game.h"
#include "../../../MyMath.h"
#include <cassert>

namespace
{
	// アニメーションの最少枚数
	constexpr int kMinAnimNum = 1;
	// フェードの最大値
	constexpr int kMaxFadeRate = 255;
}

MyLib::UIImage::UIImage(DrawLayer layer) : 
	Drawable2D(layer),
	m_isAlive(true),
	m_scrollPos{},
	m_graphSize{},
	m_scale{ 1.0f, 1.0f },
	m_type(ImageType::Normal),
	m_animGraphSize{},
	m_rotation(0.0f),
	m_animNum(0),
	m_animFrame(0),
	m_loopFrame(0),
	m_isLoop(true),
	m_animFrameCount(0),
	m_animCount(0),
	m_isPulsing(false),
	m_pulseFrame(-1),
	m_pulseRange(0.0f)
{
}

void MyLib::UIImage::Init(std::weak_ptr<GameObject> parent)
{
	std::shared_ptr<GameObject> pParent = parent.lock();
	m_pTransform = pParent->GetComponent<Transform>();
	if (!m_pTransform.lock())
	{
		assert(false && "UIImage : Transformコンポーネントがありません");
	}
	Renderer::GetInstance().Entry(shared_from_this());
}

void MyLib::UIImage::Start()
{
	// 変数に画像のサイズを取得
	GetGraphSize(m_pImageFile->GetHandle(), &m_graphSize.x, &m_graphSize.y);
}

void MyLib::UIImage::Update()
{
	// UIの更新フレームを増加
	m_uiUpdateFrameCount++;

	// フェード中の場合
	if (UpdateFade())
	{
		// 消去するかどうかを判別
		m_isAlive = !m_isAfterDelete;
		return;
	}

	// アニメーションする画像の場合の処理
	if (m_type == ImageType::Animation)
	{
		// アニメーションが1枚以下なら更新しない
		if (m_animNum <= kMinAnimNum) { return; }
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

	if (UpdateAppearState())
	{
		m_isAlive = m_isAfterDelete;
	}

	// ループさせる背景を描画する際の処理
	if (m_type == ImageType::LoopBackGround)
	{
		Vector2Int pos = m_pTransform.lock()->GetScreenPos();

		// 表示する座標を加算
		pos.x += m_loopFrame;
		pos.y += m_loopFrame;

		m_pTransform.lock()->SetScreenPos(pos);

		// スクロールした上での座標を計算
		// 基準からどれだけスクロールしたかどうか
		// 例:画像サイズが1280*720でm_posが{1400,1400}の時スクロール座標は{120,680}
		m_scrollPos = { -MyLib::RemainderToNaturalNumber(pos.x, m_graphSize.x),
			-MyLib::RemainderToNaturalNumber(pos.y, m_graphSize.y) };
	}
}

void MyLib::UIImage::End()
{
	m_pFrameFile.reset();
	m_pImageFile.reset();

	Renderer::GetInstance().Exit(shared_from_this());
}

void MyLib::UIImage::Draw() const
{
	if (!IsActive()) return;
	if (!Renderer::GetInstance().IsUILayerActive(m_uiLayer)) return;
	if (!m_pImageFile) return;

	const float appearScale = GetAppearRate();
	
	// 拡縮用のスケール値
	float pulseScale = 1.0f;

	if (m_isPulsing && m_pulseFrame > 0)
	{
		float t = static_cast<float>(m_uiUpdateFrameCount) / static_cast<float>(m_pulseFrame);
		float wave = sinf(t * DX_TWO_PI_F);
		pulseScale = 1.0f + wave * m_pulseRange;
	}

	const Vector2 finalScale = m_scale * appearScale * pulseScale;

	const int layerAlpha = Renderer::GetInstance().GetUILayerAlpha(m_uiLayer);
	Vector2Int pos = Vector2Int{ 0,0 };

	if (auto pTransform = m_pTransform.lock())
	{
		pos = pTransform->GetScreenPos();
	}

	float fadeOpacity = 1.0f;
	// フェード状態ならばアルファブレンドに設定
	if (IsFade())
	{
		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
		auto rate = static_cast<float>(m_frameCount) / static_cast<float>(m_fadeFrame);
		//fadeOpacity = 1.0f - rate;
		fadeOpacity = (m_fadeState == FadeState::FadeIn) ? rate : (1.0f - rate);
		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate - kMaxFadeRate * rate));
	}

	const float blinkOpacity = GetBlinkAlphaRate();

	const int finalAlpha = static_cast<int>(layerAlpha * fadeOpacity * blinkOpacity);
	const bool needBlend = finalAlpha != kMaxFadeRate;

	if (needBlend)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, finalAlpha);
	}

	// タイプによって描画方法が変わる
	switch (m_type)
	{
	case UIImage::ImageType::Normal:
	{
		// 画像を描画
		DrawRotaGraph3(
			pos.x,
			pos.y,
			m_graphSize.x / 2,
			m_graphSize.y / 2,
			finalScale.x, finalScale.y,
			m_rotation,
			m_pImageFile->GetHandle(), true);
	}
	break;
	case UIImage::ImageType::Animation:
		// 画像を描画
		DrawRectRotaGraph3(
			pos.x, pos.y, // 表示座標
			m_animCount * m_animGraphSize.x, 0, // 画像の切り取り座標
			m_animGraphSize.x, m_animGraphSize.y, // 描画するサイズ
			m_animGraphSize.x / 2, m_animGraphSize.y / 2, // 画像の回転の中心
			m_scale.x, m_scale.y, // 画像のスケール
			m_rotation, // 回転率
			m_pImageFile->GetHandle(),
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
	if (m_pFrameFile)
	{
		// 枠の画像を描画
		DrawExtendGraph(
			pos.x - (m_graphSize.x * static_cast<int>(m_scale.x)) / 2,
			pos.y - (m_graphSize.y * static_cast<int>(m_scale.y)) / 2,
			pos.x + (m_graphSize.x * static_cast<int>(m_scale.x)) / 2,
			pos.y + (m_graphSize.y * static_cast<int>(m_scale.y)) / 2,
			m_pFrameFile->GetHandle(), true);
	}

	// フェード状態で設定したブレンド描画を戻す
	if (needBlend)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void MyLib::UIImage::SetAnimation(const Vector2Int& animGraphSize, int animNum, int animFrame, bool isLoop)
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

void MyLib::UIImage::SetLoopBackGround(int loopFrame)
{
	m_type = ImageType::LoopBackGround;
	m_loopFrame = loopFrame;
}

void MyLib::UIImage::StartAppearCenter(int appearFrame)
{
	m_uiState = UIState::AppearCenter;
	m_appearFrame = appearFrame;
	m_uiUpdateFrameCount = 0;
	m_graphSize.y = 0;
}

void MyLib::UIImage::StartPulse(int pulseFrame, float pulseRange)
{
	m_isPulsing = true;
	m_pulseFrame = pulseFrame;
	m_pulseRange = pulseRange;
}

void MyLib::UIImage::LoopBackGroundDraw() const
{
	// 左上
	DrawGraph(m_scrollPos.x, m_scrollPos.y, m_pImageFile->GetHandle(), true);

	// 画像のはみ出している部分を補う描画をするかどうか
	const bool needX = (m_scrollPos.x + m_graphSize.x < Game::kScreenWidth);
	const bool needY = (m_scrollPos.y + m_graphSize.y < Game::kScreenHeight);

	// X座標がはみ出している場合右側に新たに画像を描画
	if (needX)
	{
		// 右上
		DrawGraph(m_scrollPos.x + m_graphSize.x, m_scrollPos.y, m_pImageFile->GetHandle(), true);
	}

	// Y座標がはみ出している場合下側に新たに画像を描画
	if (needY)
	{
		// 左下
		DrawGraph(m_scrollPos.x, m_scrollPos.y + m_graphSize.y, m_pImageFile->GetHandle(), true);
	}

	// 両方の座標がはみ出している場合右下側に新たに画像を描画
	if (needX && needY)
	{
		// 右下
		DrawGraph(m_scrollPos.x + m_graphSize.x, m_scrollPos.y + m_graphSize.y, m_pImageFile->GetHandle(), true);
	}
}