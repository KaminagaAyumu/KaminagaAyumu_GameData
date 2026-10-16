#pragma once
#include "../../MyLib/MyString.h"
#include <vector>
#include <string>
#include "DxLib.h"

/// <summary>
/// CSVデータを格納するクラス
/// </summary>
class CSVResource
{
public:
	CSVResource();
	virtual ~CSVResource() = default;

	bool Load(const std::wstring& path);

protected:

	// CSVのデータ
	std::vector<std::vector<std::wstring>> m_data;

protected:

	/// <summary>
	/// 指定した型のデータを読み込む
	/// </summary>
	/// <typeparam name="T">指定した型</typeparam>
	/// <param name="row">行</param>
	/// <param name="cell">列</param>
	/// <returns>指定した型のデータ</returns>
	template<typename T>
	T Read(int row, int cell)
	{
		// 指定したデータの文字列を取得
		const std::wstring& wString = m_data[row][cell];

		// if constexprにすることでコンパイルできるようにする
		if constexpr (std::is_same_v<T, std::wstring>)
		{
			return MyLib::NormalizeToNFC(wString);
		}
		// int型を取得したい場合
		else if constexpr (std::is_same<T, int>::value)
		{
			return std::stoi(wString);
		}
		// float型を取得したい場合
		else if constexpr (std::is_same<T, float>::value)
		{
			return std::stof(wString);
		}
		// それ以外はそのまま文字列を返す
		else
		{
			return wString;
		}
	}

	/// <summary>
	/// データの数を取得する
	/// size_t型になっているのはコンテナのサイズを返すイテレータがsize_tを返すことと、
	/// 符号なしで扱いたいためそのまま返している
	/// </summary>
	/// <returns></returns>
	size_t GetDataCount() { return m_data.size(); }

	void ReleaseRawData();

};

