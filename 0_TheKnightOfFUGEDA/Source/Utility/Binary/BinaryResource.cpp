#include "BinaryResource.h"

BinaryResource::BinaryResource() : 
	m_readCursor(0)
{
}

bool BinaryResource::Load(const std::wstring& path)
{
	// 非同期読み込みをしているか確認
	// ファイルが完全に開かれていない状態で読み込むのを防ぐ
	bool isASyncLoading = GetUseASyncLoadFlag();
	SetUseASyncLoadFlag(false);

	// ファイルの総サイズを取得
	int64_t size = FileRead_size(path.c_str());
	// ファイルのサイズが0以下ならばロード失敗
	if (size <= 0) return false;

	// ファイルを開いてハンドルを得る
	int handle = FileRead_open(path.c_str());
	// ファイルが見つからなかった場合以下の処理を行わない
	if (handle == -1) return false;

	// ファイルのサイズ分確保する
	m_rawData.resize(static_cast<size_t>(size));
	// データをまとめて読み込む
	FileRead_read(m_rawData.data(), static_cast<int>(size), handle);

	// ファイルを閉じる
	FileRead_close(handle);

	// 読み込んだ数を初期化
	m_readCursor = 0;

	// 元の非同期読み込みフラグに戻す
	SetUseASyncLoadFlag(isASyncLoading);
	return true;
}

void BinaryResource::ReleaseRawData()
{
	m_rawData.clear();
	// 余分なメモリを確保している部分をなくす
	m_rawData.shrink_to_fit();
}
