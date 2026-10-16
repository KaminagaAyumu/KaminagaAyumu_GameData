#pragma once
#include <string>

/// <summary>
/// テキストのデータを管理するCSVデータクラス
/// </summary>
class TextData
{
public:
	TextData();
	virtual ~TextData();

	void SetData(std::wstring id, std::wstring text);

	std::wstring GetText() { return m_text; }

private:

	std::wstring m_id;
	std::wstring m_text;

};

