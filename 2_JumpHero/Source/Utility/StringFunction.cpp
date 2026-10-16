#include "StringFunction.h"
#include "DxLib.h"

std::wstring StringFunction::WStringFromString(const std::string& str)
{
	// 変換して返り値にするワイド文字列
	std::wstring result;
	
	// 受け取った引数の文字列のサイズを取得
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);

	// 文字列サイズ分の領域を確保
	result.resize(len);

	// もう一度関数を呼んで変換を実行
	// 受け取りができる引数にワイド文字列のdata()とsize()を使う
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), result.data(), static_cast<int>(result.size()));

	// 変換結果を返す
    return result;
}

std::vector<std::wstring> StringFunction::WrapTextToWidth(const std::wstring& text, int fontHandle, int maxWidth)
{
	std::vector<std::wstring> lines;
	if (text.empty() || maxWidth <= 0)
	{
		// 空白の文字を返す
		lines.emplace_back(L"");
		return lines;
	}
	const wchar_t* p = text.c_str();
	const size_t n = text.size();

	size_t lineStart = 0;
	size_t lastBreakPos = SIZE_MAX;
	int lastBreakWidth = 0;

	// 文字列サイズ分ループ
	for (size_t i = 0; i < n; i++)
	{
		// 文字を追加した際の幅を計測
		std::wstring tmp = text.substr(lineStart, i - lineStart + 1);
		int w = GetDrawStringWidthToHandle(tmp.c_str(), static_cast<int>(tmp.size()), fontHandle);

		int currentWidth = 0;

		if (w > maxWidth)
		{
			if (lastBreakPos != SIZE_MAX && lastBreakPos >= lineStart)
			{
				size_t cutLen = lastBreakPos - lineStart;
				lines.emplace_back(text.substr(lineStart, cutLen));

				lineStart = lastBreakPos + 1;
			}
			else
			{
				if (i > lineStart)
				{
					lines.emplace_back(text.substr(lineStart, i - lineStart));
					lineStart = i;
				}
				else
				{
					lines.emplace_back(text.substr(i, 1));
					lineStart = i + 1;
				}
			}

			std::wstring tmp2 = (lineStart < n) ? text.substr(lineStart, i - lineStart + 1) : L"";

			currentWidth = tmp2.empty() ? 0 : GetDrawStringWidthToHandle(tmp2.c_str(), static_cast<int>(tmp2.size()), fontHandle);

			lastBreakPos = SIZE_MAX;
		}
		else
		{
			currentWidth = w;
		}
	}

	if (lineStart <= n)
	{
		lines.emplace_back(text.substr(lineStart));
	}
	
	return lines;
}
