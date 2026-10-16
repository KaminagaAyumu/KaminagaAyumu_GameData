#include "ModelFile.h"
#include "DxLib.h"

ModelFile::ModelFile()
{
}

void ModelFile::DeleteHandle()
{
	MV1DeleteModel(m_handle);
}
