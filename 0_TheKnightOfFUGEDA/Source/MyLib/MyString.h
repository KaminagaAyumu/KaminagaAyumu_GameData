#pragma once
#include "DxLib.h"
#include <string>

namespace MyLib
{
	inline std::wstring GetWStringFromString(const std::string& str)
	{
		std::wstring wstr;

		// マルチバイト文字からワイド文字列に変換した際のサイズを取得
		auto size = MultiByteToWideChar(CP_ACP,
										MB_COMPOSITE | MB_ERR_INVALID_CHARS,
										str.data(), static_cast<int>(str.length()), nullptr, 0);
		// 文字列をリサイズ
		wstr.resize(size);
		
		// 実際にマルチバイト文字からワイド文字列に変換する
		MultiByteToWideChar(CP_ACP,
							MB_COMPOSITE | MB_ERR_INVALID_CHARS,
							str.data(), static_cast<int>(str.length()), wstr.data(), static_cast<int>(wstr.length()));
		return wstr;
	}

	inline std::wstring Utf8ToWide(const std::string& str)
	{
		if (str.empty())
		{
			return std::wstring();
		}

		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(size - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), size);
		return result;
	}

	inline std::wstring NormalizeToNFC(const std::wstring& input)
	{
		int len = NormalizeString(NormalizationC, input.c_str(), -1, nullptr, 0);
		if (len <= 0)
		{
			return input;
		}

		std::wstring output(len , L'\0');
		int result = NormalizeString(NormalizationC, input.c_str(), -1, output.data(), len);
		if (result <= 0)
		{
			return input;
		}

		output.resize(result - 1);
		return output;
	}
}