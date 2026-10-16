#include "UIText.h"
#include "../../../GameObject.h"
#include "../../../Renderer.h"
#include "../../../../Utility/Game.h"
#include <cassert>

namespace
{
	constexpr unsigned int kDefaultTextColor = 0xffffff;	// 文字色の初期値
}

MyLib::UIText::UIText(DrawLayer layer) :
	Drawable2D(layer),
	m_text(L""),
	m_fontHandle(-1),
	m_textColor(kDefaultTextColor),
	m_isAlive(true),
	m_alignmentType(AlignmentType::Center)
{
}

void MyLib::UIText::Init(std::weak_ptr<GameObject> parent)
{
	std::shared_ptr<GameObject> pParent = parent.lock();
	m_pTransform = pParent->GetComponent<Transform>();
	if (!m_pTransform.lock())
	{
		assert(false && "UIText : Transformコンポーネントがありません");
	}
	Renderer::GetInstance().Entry(shared_from_this());
}

void MyLib::UIText::Start()
{
}

void MyLib::UIText::Update()
{
}

void MyLib::UIText::End()
{
	Renderer::GetInstance().Exit(shared_from_this());
}

void MyLib::UIText::Draw() const
{
	if (!IsActive())return;
	if (m_pTransform.expired()) return;
	Vector2Int pos = Vector2Int::Zero();

	if (auto pTransform = m_pTransform.lock())
	{
		pos = pTransform->GetScreenPos();
	}
	const int textW = GetDrawStringWidthToHandle(m_text.c_str(), static_cast<int>(m_text.size()), m_fontHandle);
	const int fontSize = GetFontSizeToHandle(m_fontHandle);

	// 表示するx座標
	int x = pos.x - textW / 2;

	switch (m_alignmentType)
	{
	case UIText::AlignmentType::Left:
		// 文字全体の右端にする
		x -= textW / 2;
		break;
	case UIText::AlignmentType::Right:
		// 文字全体の左端にする
		x += textW / 2;
		break;
	case UIText::AlignmentType::Center:
		// デフォルトが中央なので変更しない
		break;
	}

	const int y = pos.y - fontSize / 2;

	DrawStringToHandle(x, y, m_text.c_str(), m_textColor, m_fontHandle);
}
