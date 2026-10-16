#include "BoardUI.h"
#include "ParamUI.h"
#include "../../MyLib/ObjectFactory.h"
#include "../../MyLib/Component/Draw/UI/UIImage.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// ボードの上端から1行目までの、見出しを表示する領域の高さ
	// (見出しはこの領域の縦方向の中央に表示する)
	constexpr int kTitleAreaHeight = 50;

	// 行ごとの幅
	constexpr int kRowSpacing = 40;
}

BoardUI::BoardUI() :
	m_pos{},
	m_size{},
	m_fontType(MyLib::Renderer::FontType::Small),
	m_separatorOffsetX(0),
	m_isActive(true)
{
}

BoardUI::~BoardUI()
{
}

void BoardUI::Init(const Vector2Int& pos, const Vector2Int& size, MyLib::Renderer::FontType type)
{
	m_pos = pos;
	m_size = size;
	m_fontType = type;
}

void BoardUI::End()
{
	if (m_pBgObj)
	{
		m_pBgObj->End();
	}

	if (m_pTitleObj)
	{
		m_pTitleObj->End();
	}

	for (auto& pParamUI : m_pParamUIs)
	{
		pParamUI->End();
	}
	m_pParamUIs.clear();
}

void BoardUI::SetBackGround(std::shared_ptr<File> pFile)
{
	// 描画コンポーネントは初期化した順に描画されるため、
	// 背景を見出しや行より後に生成すると文字が背景に隠れてしまう
	if (m_pTitleObj || !m_pParamUIs.empty())
	{
		assert(false && "BoardUI : 背景は見出しや行を追加する前に設定してください");
		return;
	}

	if (m_pBgObj)
	{
		assert(false && "BoardUI : 背景は既に設定されています");
		return;
	}

	if (!pFile) return;

	m_pBgObj = MyLib::ObjectFactory::CreateUIImage(m_pos);

	if (auto pImage = m_pBgObj->GetComponent<MyLib::UIImage>().lock())
	{
		pImage->SetImageFile(pFile);

		// 画像がボード全体のサイズで表示されるように拡大率を設定する
		Vector2Int graphSize;
		GetGraphSize(pFile->GetHandle(), &graphSize.x, &graphSize.y);
		if (graphSize.x > 0 && graphSize.y > 0)
		{
			pImage->SetScale({ static_cast<float>(m_size.x) / static_cast<float>(graphSize.x),
				static_cast<float>(m_size.y) / static_cast<float>(graphSize.y) });
		}

		pImage->SetActive(m_isActive);
	}

	// 初期化時に画像のサイズを取得するため、画像を設定してから初期化する
	m_pBgObj->Init();
}

void BoardUI::SetTitle(const std::wstring& title)
{
	// 初めて設定したときだけ見出しのテキストを生成する
	if (!m_pTitleObj)
	{
		const int top = m_pos.y - m_size.y / 2;
		const Vector2Int titlePos = { m_pos.x, top + kTitleAreaHeight / 2 };

		m_pTitleObj = MyLib::ObjectFactory::CreateUIText(titlePos, m_fontType);
		m_pTitleObj->Init();
		m_pTitle = m_pTitleObj->GetComponent<MyLib::UIText>();
	}

	if (auto pTitle = m_pTitle.lock())
	{
		pTitle->SetText(title);
		pTitle->SetActive(m_isActive);
	}
}

void BoardUI::AddParam(const std::wstring& text, int param)
{
	AddParam(text, std::to_wstring(param));
}

void BoardUI::AddParam(const std::wstring& text, const std::wstring& param)
{
	auto pParamUI = std::make_shared<ParamUI>();
	pParamUI->Init(GetRowPos(static_cast<int>(m_pParamUIs.size())), m_fontType);
	pParamUI->SetText(text);
	pParamUI->SetParam(param);
	pParamUI->SetActive(m_isActive);
	m_pParamUIs.push_back(pParamUI);
}

void BoardUI::SetParam(int index, int param)
{
	if (index < 0 || index >= static_cast<int>(m_pParamUIs.size()))
	{
		assert(false && "BoardUI : 存在しない行の値を変更しようとしました");
		return;
	}

	m_pParamUIs[index]->SetParam(param);
}

void BoardUI::SetActive(bool isActive)
{
	m_isActive = isActive;

	if (m_pBgObj)
	{
		if (auto pImage = m_pBgObj->GetComponent<MyLib::UIImage>().lock())
		{
			pImage->SetActive(isActive);
		}
	}

	if (auto pTitle = m_pTitle.lock())
	{
		pTitle->SetActive(isActive);
	}

	for (auto& pParamUI : m_pParamUIs)
	{
		pParamUI->SetActive(isActive);
	}
}

void BoardUI::SetSeparatorOffsetX(int offsetX)
{
	m_separatorOffsetX = offsetX;

	// 追加済みの行を新しい位置に並べなおす
	for (int i = 0; i < static_cast<int>(m_pParamUIs.size()); ++i)
	{
		m_pParamUIs[i]->SetPos(GetRowPos(i));
	}
}

Vector2Int BoardUI::GetRowPos(int index) const
{
	// 見出しの領域の下から、行の間隔ごとに上から並べる(UIText は縦方向が中心基準)
	// X座標はテキストと値の境目になる
	const int top = m_pos.y - m_size.y / 2;
	return { m_pos.x + m_separatorOffsetX, top + kTitleAreaHeight + kRowSpacing * index + kRowSpacing / 2 };
}
