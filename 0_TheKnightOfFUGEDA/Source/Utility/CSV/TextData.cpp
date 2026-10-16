#include "TextData.h"

TextData::TextData() : 
	m_id(L""),
	m_text(L"")
{
}

TextData::~TextData()
{
}

void TextData::SetData(std::wstring id, std::wstring text)
{
	m_id = id;
	m_text = text;
}
