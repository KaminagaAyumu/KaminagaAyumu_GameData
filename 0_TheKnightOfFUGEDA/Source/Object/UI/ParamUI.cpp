#include "ParamUI.h"
#include "../../MyLib/ObjectFactory.h"

ParamUI::ParamUI() : 
	m_betweenSymbol(L"")
{
}

void ParamUI::Init(const Vector2Int& pos, MyLib::Renderer::FontType type)
{
	m_pTextObj = MyLib::ObjectFactory::CreateUIText(pos, type);
	m_pTextObj->Init();
	m_pText = m_pTextObj->GetComponent<MyLib::UIText>();

	if (auto pText = m_pText.lock())
	{
		pText->SetAlignment(MyLib::UIText::AlignmentType::Left);
	}

	m_pParamObj = MyLib::ObjectFactory::CreateUIText(pos, type);
	m_pParamObj->Init();
	m_pParam = m_pParamObj->GetComponent<MyLib::UIText>();

	if (auto pParam = m_pParam.lock())
	{
		pParam->SetAlignment(MyLib::UIText::AlignmentType::Right);
	}
	m_betweenSymbol = L":";
}

void ParamUI::End()
{
	m_pTextObj->End();
	m_pParamObj->End();
}

void ParamUI::SetText(const std::wstring& text)
{
	if (auto pText = m_pText.lock())
	{
		pText->SetText(text + m_betweenSymbol);
	}
}

void ParamUI::SetParam(int param)
{
	if (auto pParam = m_pParam.lock())
	{
		pParam->SetText(std::to_wstring(param));
	}
}

void ParamUI::SetParam(const std::wstring& text)
{
	if (auto pParam = m_pParam.lock())
	{
		pParam->SetText(text);
	}
}

void ParamUI::SetActive(bool isActive)
{
	if (auto pText = m_pText.lock())
	{
		pText->SetActive(isActive);
	}

	if (auto pParam = m_pParam.lock())
	{
		pParam->SetActive(isActive);
	}
}

void ParamUI::SetPos(const Vector2Int& pos)
{
	// テキストとパラメータは同じ座標を基準に左右へ寄せて表示している
	if (auto pTransform = m_pTextObj->GetComponent<MyLib::Transform>().lock())
	{
		pTransform->SetScreenPos(pos);
	}

	if (auto pTransform = m_pParamObj->GetComponent<MyLib::Transform>().lock())
	{
		pTransform->SetScreenPos(pos);
	}
}

void ParamUI::SetBetweenSymbol(std::wstring symbol)
{
	m_betweenSymbol = symbol;
}
