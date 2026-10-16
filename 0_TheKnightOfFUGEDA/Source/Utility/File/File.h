#pragma once
#include <string>

// プロトタイプ宣言
class FileManager; // ファイル管理クラスでこのクラスを管理するために宣言

/// <summary>
/// ファイルクラス
/// リソースハンドルを持っている
/// </summary>
class File
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	File();
	virtual ~File();

	/// <summary>
	/// リソースハンドルを取得する
	/// </summary>
	/// <returns>リソースハンドル</returns>
	int GetHandle()const { return m_handle; }

	/// <summary>
	/// リソースハンドルを開放する
	/// </summary>
	virtual void DeleteHandle() abstract;

private:
	// ハンドル削除やパスの設定を行えるようにfriend化
	friend class FileManager;

protected:
	// リソースハンドル
	int m_handle;
	// 参照カウンタ
	int m_count;
	// リソースのパス
	std::wstring m_path;
};

