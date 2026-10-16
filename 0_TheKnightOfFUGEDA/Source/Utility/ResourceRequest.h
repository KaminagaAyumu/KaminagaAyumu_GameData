#pragma once
#include "File/FileManager.h"
#include <string>

struct ResourceRequest
{
	std::wstring path; // リソースのパス
	FileManager::FileType type; // リソースの種類
	bool isEternal; // 常に存在するリソースかどうか
};