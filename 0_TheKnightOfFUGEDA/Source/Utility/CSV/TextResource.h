#pragma once
#include "CSVResource.h"
#include "TextData.h"
#include <string>
#include <memory>
#include <unordered_map>

/// <summary>
/// テキストファイルのデータを管理するCSVデータクラス
/// </summary>
class TextResource : public CSVResource
{
public:

	TextResource() = default;
	virtual ~TextResource() = default;

	void ConvertTextData();

	std::wstring GetText(std::wstring id);

private:
	// テキストのデータ
	std::unordered_map<std::wstring, std::shared_ptr<TextData>> m_pTextDatas;

};

