#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "DxLib.h"

/// <summary>
/// バイナリデータを格納するクラス
/// </summary>
class BinaryResource
{
public:
	BinaryResource();
	virtual ~BinaryResource() = default;

	/// <summary>
	/// バイナリのデータをロードする
	/// </summary>
	/// <param name="path">ファイルのパス</param>
	/// <returns>true : ロード成功 false : ロード失敗</returns>
	bool Load(const std::wstring& path);


protected:
	// バイナリのデータ
	std::vector<uint8_t> m_rawData;
	// ファイルを読み込んだ数
	int m_readCursor;

protected:

	/// <summary>
	/// ファイルから指定した型のデータを読み込む
	/// </summary>
	/// <typeparam name="T">データの型</typeparam>
	/// <returns>指定した型のデータ</returns>
	template<typename T>
	T Read()
	{
		// バイナリの総データよりもサイズが大きい場合
		if (m_readCursor + sizeof(T) > m_rawData.size())
		{
			assert(false && "BinaryResource : 読み込むデータのサイズがバイナリの総データよりも大きくなっています");
		}

		// 返すための変数を準備
		T value;
		// 指定行のデータをvalueにコピーする
		std::memcpy(&value, &m_rawData[m_readCursor], sizeof(T));

		// Tのサイズ分読み込んだとする
		m_readCursor += sizeof(T);

		return value;
	}

	/// <summary>
	/// 指定した型の配列を読み込む
	/// </summary>
	/// <typeparam name="T">指定した型</typeparam>
	/// <param name="count">配列の数</param>
	/// <returns>指定した型の配列</returns>
	template<typename T>
	std::vector<T> ReadVector(int count)
	{
		// 配列のサイズを取得
		size_t totalSize = sizeof(T) * count;
		// バイナリの総データよりもサイズが大きい場合
		if (m_readCursor + totalSize > m_rawData.size())
		{
			assert(false && "BinaryResource : 読み込む配列のサイズがバイナリの総データよりも大きくなっています");
		}

		// 返すための変数を準備(サイズ分確保)
		std::vector<T> vec(count);
		// 指定業のデータをvecにコピーする
		std::memcpy(vec.data(), &m_rawData[m_readCursor], totalSize);

		// Tの数分読み込んだとする
		m_readCursor += static_cast<int>(totalSize);

		return vec;
	}

	void ReleaseRawData();
};

