#include "ResourceManifestData.h"
#include <cassert>

namespace
{
	// CSVの列番号
	constexpr int kPathColumn = 0;	// リソースのパス
	constexpr int kTypeColumn = 1;	// リソースの種類
	constexpr int kIsEternalColumn = 2;	// 常に存在するリソースかどうか(0以外でtrue)
}

void ResourceManifestData::ConvertManifestData()
{
	size_t size = GetDataCount();
	m_resourceRequests.reserve(size);

	for (size_t i = 0; i < size; ++i)
	{
		int row = static_cast<int>(i);

		ResourceRequest request;
		request.path = Read<std::wstring>(row, kPathColumn);
		request.type = ToFileType(Read<std::wstring>(row, kTypeColumn));
		request.isEternal = Read<int>(row, kIsEternalColumn) != 0;

		m_resourceRequests.push_back(request);
	}

	ReleaseRawData();
}

FileManager::FileType ResourceManifestData::ToFileType(const std::wstring& typeStr) const
{
	if (typeStr == L"Image")
	{
		return FileManager::FileType::Image;
	}
	else if (typeStr == L"Model")
	{
		return FileManager::FileType::Model;
	}
	else
	{
		assert(false && "ResourceManifestData:不明なリソースの種類です");
	}

	return FileManager::FileType();
}
