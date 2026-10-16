#include "UICombo.h"
#include "../../../GameObject.h"
#include "../../../Renderer.h"
#include "../../../../Utility/Game.h"
#include <cassert>

namespace
{
	constexpr unsigned int kFillColor = 0xffd700;
	constexpr unsigned int kDefaultTextColor = 0xffffff;	// 文字色の初期値
}

MyLib::UICombo::UICombo(DrawLayer layer) :
	Drawable2D(layer),
	m_text(L""),
	m_fontHandle(-1),
	m_textColor(kDefaultTextColor),
	m_isAlive(true),
	m_count(0),
	m_remainRate(0.0f)
{
}

void MyLib::UICombo::Init(std::weak_ptr<GameObject> parent)
{
	std::shared_ptr<GameObject> pParent = parent.lock();
	m_pTransform = pParent->GetComponent<Transform>();
	if (!m_pTransform.lock())
	{
		assert(false && "UIText : Transformコンポーネントがありません");
	}
	Renderer::GetInstance().Entry(shared_from_this());
}

void MyLib::UICombo::Start()
{
}

void MyLib::UICombo::Update()
{
}

void MyLib::UICombo::End()
{
	Renderer::GetInstance().Exit(shared_from_this());
}

void MyLib::UICombo::Draw() const
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

	const int x = pos.x - textW / 2;
	const int y = pos.y - fontSize / 2;

	DrawStringToHandle(x, y, m_text.c_str(), m_textColor, m_fontHandle);

	// フォントの高さを取得
	const int height = GetFontSizeToHandle(m_fontHandle);
	// 現在の色付き文字の最高位置を計算
	const int top = y + static_cast<int>(height * (1.0f - m_remainRate));

	SetDrawArea(x, top, pos.x + textW / 2, pos.y + fontSize / 2);
	DrawStringToHandle(x, y, m_text.c_str(), kFillColor, m_fontHandle);
	SetDrawArea(0, 0, Game::kScreenWidth, Game::kScreenHeight);
}

void MyLib::UICombo::SetParam(int count, float remainRate)
{
	m_count = count;
	m_remainRate = remainRate;
}
