#include <vector>
#include "TextManager.h"
#include "../Utility/StringFunction.h"
#include <fstream>
#include "DxLib.h"

namespace
{
	// データを格納する際に使用する定数
	constexpr int kIdNo = 0; // IDのデータの場所
	constexpr int kPageNo = 1; // ページのデータの場所
	constexpr int kTextDataNo = 2; // テキストのデータの場所
	constexpr int kDataSize = 3; // データサイズの大きさ
}

TextManager::TextManager()
{
	LoadTextData();
}

TextManager::~TextManager()
{
}

void TextManager::Init()
{

}

void TextManager::Update()
{

}

void TextManager::Draw()
{
	
}

void TextManager::LoadTextData()
{
	std::ifstream file("data/textData.csv");
	std::string line;

	// ヘッダかどうかを判別する
	bool isHeader = true;

	while (std::getline(file, line))
	{

		if (isHeader) // 最初の一行は読み込まない
		{
			isHeader = false;
			continue;
		}
		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> row;

		while (getline(stream, field, ','))
		{
			row.push_back(field);
		}
		if (row.size() >= kDataSize)
		{
			TextData data;
			data.id = row[kIdNo];
			data.page = std::stoi(row[kPageNo]);
			data.textData = row[kTextDataNo];
			m_textData[data.id].push_back(data);
		}
	}

}

std::string TextManager::GetFirstPageText(const std::string& id)
{
	auto it = m_textData.find(id);
	// idが存在しなかった場合
	if (it == m_textData.end() || it->second.empty())
	{
		return "エラーメッセージ : textIdが存在しません"; // エラーメッセージを返す
	}
	return it->second.front().textData;
}

std::vector<TextData> TextManager::GetAllPageText(const std::string& id)
{
	auto it = m_textData.find(id);
	// idが存在しなかった場合
	if (it == m_textData.end())
	{
		return {}; // 空白の配列を返す
	}
	return it->second;
}
