#pragma once
#include <vector>
#include <string>

class StringFunction
{
public:
	/// <summary>
	/// マルチバイト文字列をワイド文字列に変換する
	/// </summary>
	/// <param name="str">マルチバイト文字列</param>
	/// <returns>ワイド文字列</returns>
	static std::wstring WStringFromString(const std::string& str);

	/// <summary>
	/// テキストを改行して変換する
	/// </summary>
	/// <param name="text">テキスト</param>
	/// <param name="fontHandle">フォントのハンドル</param>
	/// <param name="maxWidth">1行の範囲</param>
	/// <returns>改行したテキストのvector</returns>
	static std::vector<std::wstring> WrapTextToWidth(const std::wstring& text, int fontHandle, int maxWidth);
};

