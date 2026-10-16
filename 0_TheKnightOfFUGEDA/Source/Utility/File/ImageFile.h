#pragma once
#include "File.h"

class ImageFile : public File
{
public:
	ImageFile();
	virtual ~ImageFile() = default;

	/// <summary>
	/// リソースハンドルを開放する
	/// </summary>
	void DeleteHandle() override;
};

