#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

// プロトタイプ宣言
class File;

/// <summary>
/// リソースファイル管理クラス
/// </summary>
class FileManager
{
public:
	// デストラクタ
	virtual ~FileManager();

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns>ファイル管理クラスのインスタンス</returns>
	static FileManager& GetInstance();

	void Init();

	void End();

	void Update();

	void ReserveImage(const wchar_t* path, bool isEtarnal);
	void ReserveModel(const wchar_t* path, bool isEtarnal);

	/// <summary>
	/// ロード進捗率(0.0~1.0)を取得する
	/// </summary>
	/// <returns></returns>
	float GetLoadProgress() const;

	/// <summary>
	/// ロードが終了したかどうかを取得する
	/// </summary>
	/// <returns></returns>
	bool IsLoadingComplete() const;

	std::shared_ptr<File> GetImage(const wchar_t* path, bool isEtarnal);
	std::shared_ptr<File> GetModel(const wchar_t* path, bool isEtarnal);

	/// <summary>
	/// ファイルの種類
	/// </summary>
	enum class FileType : uint8_t
	{
		Image,	// 画像
		Model	// モデル
	};

private:
	/// <summary>
	/// コンストラクタ
	/// シングルトンクラスのためprivateで宣言する
	/// ※宣言の際にcppファイルの一番上に配置しています
	/// </summary>
	FileManager();
	FileManager(const FileManager&) = delete;		// コピーコンストラクタを作れないようにする
	void operator=(const FileManager&) = delete;	// 代入演算子を使えないようにする

	/// <summary>
	/// 非同期読み込みのファイルの状態
	/// </summary>
	struct ASyncEntryFile
	{
		std::wstring path;	// ファイルのパス
		FileType type;		// ファイルの形式
		bool isEtarnal;		// 常駐ファイルフラグ
		bool isStarted;		// ロード開始したか
		bool isCompleted;	// ロードが完了したか
	};

	std::shared_ptr<File> LoadImageASync(const wchar_t* path, bool isEtarnal);
	std::shared_ptr<File> LoadModelASync(const wchar_t* path, bool isEtarnal);

	/// <summary>
	/// ファイルの削除処理を行う
	/// </summary>
	/// <param name="path">ファイルのパス</param>
	void OnDeleteFile(const std::wstring& path);

	// ファイルのリソース
	std::unordered_map<std::wstring, std::shared_ptr<File>> m_files;

	// ゲーム中常に存在するリソース
	std::unordered_map<std::wstring, std::shared_ptr<File>> m_etarnalFiles;

	// 非同期読み込みのリソースをまとめるvector
	std::vector<ASyncEntryFile> m_entryQueue;

	// 非同期読み込みする予定のファイルの数
	int m_totalASyncFile;

	// ロードが終了した数
	int m_loadedCount;
};

