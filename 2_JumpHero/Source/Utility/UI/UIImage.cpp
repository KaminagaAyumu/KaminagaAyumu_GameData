#include "../Animation.h"
#include <memory>
#include "UIImage.h"
#include "DxLib.h"

UIImage::UIImage() : 
	m_handle(-1),
	m_size{},
	m_pos{},
	m_isAlive(true)
{

}

UIImage::~UIImage()
{
}

void UIImage::Init(int handle, const Size& size, const Position2& pos)
{
	m_handle = handle;
	m_size = size;
	m_pos = pos;
}

void UIImage::Update()
{
	// アニメーションを使用する場合
	if (m_pAnimation)
	{
		// アニメーションを更新する
		m_pAnimation->Update();
	}
}

void UIImage::Draw() const
{
	// アニメーションを使用する場合
	if (m_pAnimation)
	{
		Position2Int pos = { static_cast<int>(m_pos.x),static_cast<int>(m_pos.y) };
		m_pAnimation->Draw(pos, false);
	}
	else // 使用しない場合
	{
		// 画像を描画
		DrawExtendGraph(static_cast<int>(m_pos.x),
			static_cast<int>(m_pos.y),
			static_cast<int>(m_pos.x) + m_size.width,
			static_cast<int>(m_pos.y) + m_size.height,
			m_handle, true);
	}
}

bool UIImage::IsAlive()const
{
	return m_isAlive;
}

void UIImage::SetAnimation(int animNum, int animFrame, bool isLoop)
{
	// アニメーションを生成
	m_pAnimation = std::make_shared<Animation>();
	
	// 出来れば画像サイズが違うときに対応するようにする
	/*Size gSize;
	GetGraphSize(m_handle, &gSize.width, &gSize.height);
	gSize.width = gSize.width / animNum;

	m_pAnimation->SetScale(m_size.width / gSize.width);*/

	m_pAnimation->SetAnimation(m_handle, m_size, animNum, animFrame, isLoop);
}

void UIImage::Close()
{
	m_isAlive = false;
}
