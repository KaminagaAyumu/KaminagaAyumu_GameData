#include "CSVResource.h"
#include "../../MyLib/MyString.h"
#include <fstream>
#include <sstream>

CSVResource::CSVResource()
{
}

bool CSVResource::Load(const std::wstring& path)
{
	// 非同期読み込みをしているか確認
	// ファイルが完全に開かれていない状態で読み込むのを防ぐ
	bool isASyncLoading = GetUseASyncLoadFlag();
	SetUseASyncLoadFlag(false);

	std::ifstream file(path);
	if (!file) // ファイルの読み込みに失敗した場合
	{
		return false; // ロード失敗とする
	}

	// UTF8のCSVファイル全体のサイズを取得
	std::string utf8((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	// UTF16に変換する
	std::wstring utf16 = MyLib::GetWStringFromString(utf8);

	std::wstringstream wideString(utf16);
	std::wstring line;
	bool isHeader = true;

	while (std::getline(wideString, line))
	{
		// 最初の一行は読み込まない
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		std::wstringstream stream(line);
		std::wstring field;
		std::vector<std::wstring> cell;	// 列のデータ

		while (getline(stream, field, L','))
		{
			cell.push_back(field);
		}
		// 列のデータを行に格納
		m_data.push_back(cell);
	}

	// 元の非同期読み込みフラグに戻す
	SetUseASyncLoadFlag(isASyncLoading);

	return true;
}

void CSVResource::ReleaseRawData()
{
	m_data.clear();
	// 余分なメモリを確保している部分をなくす
	m_data.shrink_to_fit();
}