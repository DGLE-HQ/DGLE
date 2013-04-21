/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "HDDFileSystem.h"
#include "HDDFile.h"

#include <sys\stat.h>

// CHDDFileIterator //

CHDDFileIterator::CHDDFileIterator(uint uiInstIdx, const char* pcName, HANDLE fileHandle):
CInstancedObj(uiInstIdx)
{	
	if (strlen(pcName) <= MAX_PATH)
		strcpy(_acName, pcName);
	else
	{
		strcpy(_acName, "");
		LOG("Filename string \"pcName\" is too long.", LT_ERROR);
	}

	_fileHandle = fileHandle;
}

CHDDFileIterator::~CHDDFileIterator()
{
	if (FindClose(_fileHandle) == 0)
		LOG("Can't close find handle.",LT_ERROR);
}

DGLE_RESULT DGLE_API CHDDFileIterator::FileName(char *pcName, uint &uiCharsCount)
{
	if (!pcName)
	{
		uiCharsCount = strlen(_acName) + 1;
		return S_OK;
	}

	if (strlen(_acName) < uiCharsCount)
	{
		strcpy(pcName, _acName);
		return S_OK;
	}
	else
	{
		uiCharsCount = strlen(_acName) + 1;
		strcpy(pcName, "");
		return E_INVALIDARG;
	}
}

DGLE_RESULT DGLE_API CHDDFileIterator::Next()
{
	WIN32_FIND_DATAA find_file;

	if (FindNextFile(_fileHandle, &find_file))
	{
		strcpy(_acName, find_file.cFileName);
		return S_OK;
	}

	return S_FALSE;
}

DGLE_RESULT DGLE_API CHDDFileIterator::Free()
{
	delete this;
	return S_OK;
}

// CHDDFileSystem //

CHDDFileSystem::CHDDFileSystem(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{}

DGLE_RESULT DGLE_API CHDDFileSystem::OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile)
{
	prFile = NULL;

	if (pcName[strlen(pcName) - 1] == '\\') //if directory
		return CreateDirectoryA(pcName, NULL) == 0 ? S_FALSE : S_OK;

	prFile = new CHDDFile(InstIdx(), pcName, eFlags);
	bool _is_open;
	prFile->IsOpen(_is_open);
	
	return _is_open ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CHDDFileSystem::DeleteFile(const char *pcName)
{
	if (pcName[strlen(pcName) - 1] == '\\') //if directory
		return RemoveDirectoryA(pcName) == 0 ? S_FALSE : S_OK;

	return ::DeleteFile(pcName) == 0 ? S_FALSE : S_OK;
}

DGLE_RESULT DGLE_API CHDDFileSystem::FileExists(const char *pcName, bool &bExists)
{
	if (pcName[strlen(pcName) - 1] == '\\') //if directory
	{
		struct _stat statBuffer;
		bExists = (_stat(pcName, &statBuffer) == 0 && statBuffer.st_mode & S_IFDIR); 
		return S_OK;
	}

	bExists = _access(pcName, 0) != -1;

	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFileSystem::Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator)
{
	WIN32_FIND_DATAA find_file;

	HANDLE fhandle = FindFirstFile(pcMask, &find_file);

	if (fhandle != INVALID_HANDLE_VALUE)
	{
		prIterator = new CHDDFileIterator(InstIdx(), find_file.cFileName, fhandle);
		return S_OK;
	}

	prIterator = NULL;

	return S_FALSE;
}