#pragma once
#include "CSVResource.h"
#include "../ResourceRequest.h"
#include <vector>

/// <summary>
/// リソース読み込みリストを管理するCSVデータクラス
/// </summary>
class ResourceManifestData : public CSVResource
{
public:

	ResourceManifestData() = default;
	virtual ~ResourceManifestData() = default;

	void ConvertManifestData();

	const std::vector<ResourceRequest>& GetResourceRequests() const { return m_resourceRequests; }

private:

	FileManager::FileType ToFileType(const std::wstring& typeStr) const;

	std::vector<ResourceRequest> m_resourceRequests;
};

