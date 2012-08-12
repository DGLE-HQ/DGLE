/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "HDFile.h"

#include <fcntl.h>
#include <sys\stat.h>

using namespace std;

CHDFile::CHDFile(uint uiInstIdx, const char* pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags):
CInstancedObj(uiInstIdx),
_iFile(-1)
{
	int mode = 0;
	
	string  file_name = GetFileName(pcName),
			file_path = GetFilePath(pcName);

	if (file_name.size() < MAX_PATH)
		strcpy(_acName, file_name.c_str());
	else
		strcpy(_acName, "\"file_name\" is too long.");

	if (file_path.size() < MAX_PATH)
		strcpy(_acPath, file_path.c_str());
	else
		strcpy(_acPath, "\"file_path\" is too long.");

	if (eFlags & FSOF_WRITE && eFlags & FSOF_READ)
		mode = mode | _O_RDWR;
	else
	{
		if (eFlags & FSOF_WRITE) mode = mode | _O_WRONLY;
		if (eFlags & FSOF_READ) mode = mode | _O_RDONLY;
	}
	
	if (eFlags & FSOF_TRUNC) mode = mode | _O_TRUNC | _O_CREAT;
	if (eFlags & FSOF_BINARY) mode = mode | _O_BINARY;

	int perms = 0;

	if (mode & _O_RDWR)
		perms = _S_IREAD | _S_IWRITE;
	else
		if(mode & _O_WRONLY)
			perms = _S_IWRITE;
		else
			perms = _S_IREAD;

	if ((_iFile = _open(pcName, mode, perms)) == -1)
		LOG(string("Can't open file \"") + pcName + "\".", LT_ERROR);
}

CHDFile::~CHDFile()
{
	if (_close(_iFile) != 0)
		LOG(string("Can't close file \"") + _acName + "\".", LT_ERROR);
}

HRESULT CHDFile::Read(void *pBuffer, uint uiCount, uint &uiRead)
{
	uiRead = _read(_iFile, pBuffer, uiCount);
	return S_OK;
}

HRESULT CHDFile::Write(const void* pBuffer, uint uiCount, uint &uiWritten)
{
	uiWritten = _write(_iFile, pBuffer, uiCount);
	return S_OK;
}

HRESULT CHDFile::Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position)
{
	_lseek(_iFile, ui32Offset, (eWay == FSSF_BEGIN ? SEEK_SET : (eWay == FSSF_CURRENT ? SEEK_CUR : SEEK_END)));
	ui32Position = (uint32)(_tell(_iFile));
	return S_OK;
}

HRESULT CHDFile::GetSize(uint32 &ui32Size)
{
	ui32Size = (uint32)(_filelength(_iFile));
	return S_OK;
}

HRESULT CHDFile::IsOpen(bool &bOpened)
{
	bOpened = _iFile > 0;
	return S_OK;
}

HRESULT CHDFile::GetName(char* pcName, uint uiCharsCount)
{
	if (uiCharsCount < strlen(_acName))
	{
		LOG("Too small \"pcName\" buffer size.", LT_ERROR);
		return E_INVALIDARG;
	}

	strcpy(pcName, _acName);

	return S_OK;
}

HRESULT CALLBACK CHDFile::GetPath(char *pcPath, uint uiCharsCount)
{
	if (uiCharsCount < strlen(_acPath))
	{
		LOG("Too small \"pcPath\" buffer size.", LT_ERROR);
		return E_INVALIDARG;
	}

	strcpy(pcPath, _acPath);

	return S_OK;
}

HRESULT CALLBACK CHDFile::Free()
{
	delete this;
	return S_OK;
}