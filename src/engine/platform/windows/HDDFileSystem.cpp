/**
\author		Korotkov Andrey aka DRON
\date		20.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "HDDFileSystem.h"
#include "HDDFile.h"

// CHDDFileIterator //

namespace
{
	class CHDDFileIterator final : public CInstancedObj, public IFileIterator
	{
		char _acName[MAX_PATH];
		HANDLE _fileHandle;

	public:

		CHDDFileIterator(uint uiInstIdx, const char *pcName, HANDLE fileHandle);
		~CHDDFileIterator();

		DGLE_RESULT DGLE_API FileName(char *pcName, uint &uiCharsCount) override;
		DGLE_RESULT DGLE_API Next() override;
		DGLE_RESULT DGLE_API Free() override;

		IDGLE_BASE_IMPLEMENTATION(IFileIterator, INTERFACE_IMPL_END)
	};
}

CHDDFileIterator::CHDDFileIterator(uint uiInstIdx, const char *pcName, HANDLE fileHandle) :
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
		LOG("Can't close find handle.", LT_ERROR);
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

DGLE_RESULT DGLE_API CHDDFileSystem::OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile)
{
	prFile = NULL;

	if (pcName[strlen(pcName) - 1] == '\\') //if directory
	{
		std::error_code error;
		return fs::create_directory(pcName, error) ? S_OK : error ? E_FAIL : S_FALSE;
	}

	prFile = new CHDDFile(InstIdx(), pcName, eFlags);
	bool _is_open;
	prFile->IsOpen(_is_open);
	
	return _is_open ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CHDDFileSystem::DeleteFile(const char *pcName)
{
	std::error_code error;
	return fs::remove(pcName, error) ? S_OK : error ? E_FAIL : S_FALSE;
}

DGLE_RESULT DGLE_API CHDDFileSystem::FileExists(const char *pcName, bool &bExists)
{
	std::error_code error;
	bExists = fs::exists(pcName, error);

	return error ? E_FAIL : S_OK;
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