#pragma once
#include "File.h"

class ModelFile : public File
{
public:
	ModelFile();
	virtual ~ModelFile() = default;

	/// <summary>
	/// リソースハンドルを開放する
	/// </summary>
	void DeleteHandle() override;
};

