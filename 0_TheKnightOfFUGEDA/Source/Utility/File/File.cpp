#include "File.h"
#include "FileManager.h"

File::File() : 
	m_handle(-1),
	m_count(0),
	m_path{}
{
}

File::~File()
{

}
