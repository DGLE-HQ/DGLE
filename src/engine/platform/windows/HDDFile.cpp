/**
\author		Korotkov Andrey aka DRON
\date		16.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "HDDFile.h"

#include <fcntl.h>
#include <sys\stat.h>

using namespace std;

CHDDFile::CHDDFile(uint uiInstIdx, const char* pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags):
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

CHDDFile::~CHDDFile()
{
	if (_close(_iFile) != 0)
		LOG(string("Can't close file \"") + _acName + "\".", LT_ERROR);
}

DGLE_RESULT DGLE_API CHDDFile::Read(void *pBuffer, uint uiCount, uint &uiRead)
{
	uiRead = _read(_iFile, pBuffer, uiCount);
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::Write(const void* pBuffer, uint uiCount, uint &uiWritten)
{
	uiWritten = _write(_iFile, pBuffer, uiCount);
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position)
{
	const int origin = (eWay == FSSF_BEGIN ? SEEK_SET : (eWay == FSSF_CURRENT ? SEEK_CUR : SEEK_END));
	_lseek(_iFile, origin == SEEK_END ? -(long)ui32Offset : ui32Offset, origin);
	ui32Position = (uint32)(_tell(_iFile));
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::GetSize(uint32 &ui32Size)
{
	ui32Size = (uint32)(_filelength(_iFile));
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::IsOpen(bool &bOpened)
{
	bOpened = _iFile > 0;
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::GetName(char* pcName, uint &uiCharsCount)
{
	if (!pcName)
	{
		uiCharsCount = strlen(_acName) + 1;
		return S_OK;	
	}
	
	if (uiCharsCount <= strlen(_acName))
	{
		if (uiCharsCount > 0)
			strcpy(pcName, "");

		uiCharsCount = strlen(_acName) + 1;

		return E_INVALIDARG;
	}

	strcpy(pcName, _acName);

	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::GetPath(char *pcPath, uint &uiCharsCount)
{
	if (!pcPath)
	{
		uiCharsCount = strlen(_acPath) + 1;
		return S_OK;	
	}
	
	if (uiCharsCount <= strlen(_acPath))
	{
		if (uiCharsCount > 0)
			strcpy(pcPath, "");

		uiCharsCount = strlen(_acPath) + 1;

		return E_INVALIDARG;
	}

	strcpy(pcPath, _acPath);

	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::Free()
{
	delete this;
	return S_OK;
}