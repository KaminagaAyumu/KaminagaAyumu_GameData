#include "FileManager.h"
#include "File.h"
#include "ImageFile.h"
#include "ModelFile.h"
#include "DxLib.h"
#include <cassert>

FileManager::FileManager() : 
	m_totalASyncFile(0),
	m_loadedCount(0)
{
}

FileManager::~FileManager()
{
}

FileManager& FileManager::GetInstance()
{
	static FileManager instance;
	return instance;
}

void FileManager::Init()
{

}

void FileManager::End()
{
	// 非同期読み込みを終了する
	SetUseASyncLoadFlag(FALSE);

	// ゲーム中常に確保していた
	// リソースのハンドルをすべて開放する
	for (auto& [path, file] : m_etarnalFiles)
	{
		file->DeleteHandle();
	}
	m_etarnalFiles.clear();
	
	// 通常リソースのハンドルをすべて開放する
	for (auto& [path, file] : m_files)
	{
		file->DeleteHandle();
	}
	m_files.clear();

	// 非同期読み込みのリソース情報をリセットする
	m_entryQueue.clear();
}

void FileManager::Update()
{
	// 非同期読み込みでロードを行う
	SetUseASyncLoadFlag(TRUE);

	for (auto& entry : m_entryQueue)
	{
		// すでにロードが完了している場合処理をしない
		if (entry.isCompleted)
		{
			continue;
		}
		// ロードが開始されていない場合はロードを開始する
		if (!entry.isStarted)
		{
			// ファイルのタイプによってロードのやり方を変える
			switch (entry.type)
			{
			case FileType::Image:
				LoadImageASync(entry.path.c_str(), entry.isEtarnal);
				break;
			case FileType::Model:
				LoadModelASync(entry.path.c_str(), entry.isEtarnal);
				break;
			}
			// 非同期読み込みが開始されたとする
			entry.isStarted = true;
		}
		else
		{
			// ロードの完了チェック
			auto& table = entry.isEtarnal ? m_etarnalFiles : m_files;
			auto it = table.find(entry.path);
			// ロードが完了している場合はフラグを立てる
			if (it != table.end())
			{
				// 非同期読み込みの完了チェックを行う
				int loadState = CheckHandleASyncLoad(it->second->GetHandle());

				// TRUE以外の場合はロードが完了したとする
				if (loadState != TRUE)
				{
					// ロードが完了したとする
					entry.isCompleted = true;
					++m_loadedCount;
				}
			}
		}
		
	}

	// 非同期読み込みを終了する
	SetUseASyncLoadFlag(FALSE);
}

void FileManager::ReserveImage(const wchar_t* path, bool isEtarnal)
{
	// すでに登録されているものはロードされたとする
	if (m_files.count(path) || m_etarnalFiles.count(path))
	{
		++m_totalASyncFile;
		++m_loadedCount;
		return;
	}
	// ロードするvectorに情報を追加
	m_entryQueue.push_back({ path, FileType::Image, isEtarnal });
	++m_totalASyncFile;
}

void FileManager::ReserveModel(const wchar_t* path, bool isEtarnal)
{
	// すでに登録されているものはロードされたとする
	if (m_files.count(path) || m_etarnalFiles.count(path))
	{
		++m_totalASyncFile;
		++m_loadedCount;
		return;
	}
	// ロードするvectorに情報を追加
	m_entryQueue.push_back({ path, FileType::Model, isEtarnal });
	++m_totalASyncFile;
}

float FileManager::GetLoadProgress() const
{
	// 非同期読み込みをするファイルがない場合1(完了)とする
	if (m_totalASyncFile == 0) return 1.0f;

	// 非同期読み込みのファイルの中で終わったものの割合を計算する
	return static_cast<float>(m_loadedCount) / static_cast<float>(m_totalASyncFile);
}

bool FileManager::IsLoadingComplete() const
{
	// 非同期読み込みをするファイルがすべてロードされていた場合trueになる
	return m_totalASyncFile > 0 && m_loadedCount >= m_totalASyncFile;
}

std::shared_ptr<File> FileManager::GetImage(const wchar_t* path, bool isEtarnal)
{
	auto& table = isEtarnal ? m_etarnalFiles : m_files;
	auto it = table.find(path);
	if (it == table.end())
	{
		assert(false && "FileManager:画像のロードが完了していません");
		return nullptr;
	}
	++it->second->m_count;
	auto imageFile = std::dynamic_pointer_cast<ImageFile>(it->second);
	return imageFile ? std::make_shared<ImageFile>(*imageFile) : nullptr;
}

std::shared_ptr<File> FileManager::GetModel(const wchar_t* path, bool isEtarnal)
{
	auto& table = isEtarnal ? m_etarnalFiles : m_files;
	auto it = table.find(path);
	if (it == table.end())
	{
		assert(false && "FileManager:モデルのロードが完了していません");
		return nullptr;
	}
	++it->second->m_count;
	auto modelFile = std::dynamic_pointer_cast<ModelFile>(it->second);
	
	if (modelFile == nullptr)
	{
		OutputDebugStringA(typeid(*it->second).name());


		assert(false && "FileManager:Fileの実際の型がModelFileではありません");
		return nullptr;
	}
	
	return modelFile ? std::make_shared<ModelFile>(*modelFile) : nullptr;
}

std::shared_ptr<File> FileManager::LoadImageASync(const wchar_t* path, bool isEtarnal)
{
	// 常に存在するリソースに登録されていたら
	if (m_etarnalFiles.find(path) != m_etarnalFiles.end())
	{
		std::shared_ptr<ImageFile> file = std::dynamic_pointer_cast<ImageFile>(m_etarnalFiles.find(path)->second);

		return file ? std::make_shared<ImageFile>(*file) : nullptr;
	}

	// 通常リソースを確認
	auto it = m_files.find(path);
	// 初回ロード時の場合
	if (it == m_files.end())
	{
		// 画像をロード
		int handle = LoadGraph(path);
		// ロード失敗の場合assert、nullを返す
		assert(handle != -1 && "FileManager:画像のロードに失敗しました");
		if (handle == -1) return nullptr;

		// 画像ファイルを作成
		std::shared_ptr<ImageFile> newFile = std::make_shared<ImageFile>();
		// ハンドルとパスを代入して参照カウントをリセットする
		newFile->m_handle = handle;
		newFile->m_count = 0;
		newFile->m_path = path;

		// フラグを見てリソースに登録
		if (isEtarnal) // 常に存在するリソースの場合
		{
			m_etarnalFiles[path] = newFile;
		}
		else // 通常リソースの場合
		{
			m_files[path] = newFile;
		}
	}

	auto& tableRef = isEtarnal ? m_etarnalFiles : m_files;

	// ファイルのリソースを取得
	std::shared_ptr<File>& file = tableRef[path];
	// ファイルの参照カウントを増加
	++file->m_count;

	// ダウンキャストして画像ファイルのポインタにする
	std::shared_ptr<ImageFile> imageFile = std::dynamic_pointer_cast<ImageFile>(file);
	
	// 画像ファイルがnullだったらnullを返す
	if (imageFile == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<ImageFile>(*imageFile);
}

std::shared_ptr<File> FileManager::LoadModelASync(const wchar_t* path, bool isEtarnal)
{
	// 常に存在するリソースに登録されていたら
	if (m_etarnalFiles.find(path) != m_etarnalFiles.end())
	{
		std::shared_ptr<ModelFile> file = std::dynamic_pointer_cast<ModelFile>(m_etarnalFiles.find(path)->second);

		return file ? std::make_shared<ModelFile>(*file) : nullptr;
	}

	// 通常リソースを確認
	auto it = m_files.find(path);
	// 初回ロード時の場合
	if (it == m_files.end())
	{
		// 画像をロード
		int handle = MV1LoadModel(path);
		// ロード失敗の場合assert、nullを返す
		assert(handle != -1 && "FileManager:モデルのロードに失敗しました");
		if (handle == -1) return nullptr;

		// 画像ファイルを作成
		std::shared_ptr<ModelFile> newFile = std::make_shared<ModelFile>();
		// ハンドルとパスを代入して参照カウントをリセットする
		newFile->m_handle = handle;
		newFile->m_count = 0;
		newFile->m_path = path;

		// フラグを見てリソースに登録
		if (isEtarnal) // 常に存在するリソースの場合
		{
			m_etarnalFiles[path] = newFile;
		}
		else // 通常リソースの場合
		{
			m_files[path] = newFile;
		}
	}

	auto& tableRef = isEtarnal ? m_etarnalFiles : m_files;

	// ファイルのリソースを取得
	std::shared_ptr<File>& file = tableRef[path];
	// ファイルの参照カウントを増加
	++file->m_count;

	// ダウンキャストしてモデルファイルのポインタにする
	std::shared_ptr<ModelFile> modelFile = std::dynamic_pointer_cast<ModelFile>(file);

	// モデルファイルがnullだったらnullを返す
	if (modelFile == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<ModelFile>(*modelFile);
}

void FileManager::OnDeleteFile(const std::wstring& path)
{
	// 常に存在するリソースの場合は処理をしない
	if (m_etarnalFiles.count(path)) return;

	// ファイルを探す
	auto it = m_files.find(path);
	// リソース内に存在しない場合return
	if (it == m_files.end()) return;

	// ファイルを取得する
	std::shared_ptr<File>& file = it->second;
	// ファイルの参照カウントを減らす
	--file->m_count;
	//参照カウントが0になったら
	if (file->m_count <= 0)
	{
		// リソースを開放
		file->DeleteHandle();
		// リストからファイルを消去
		m_files.erase(it);
	}
}
