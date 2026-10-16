#include "ImageFile.h"
#include "DxLib.h"

ImageFile::ImageFile()
{
}

void ImageFile::DeleteHandle()
{
	DeleteGraph(m_handle);
}
