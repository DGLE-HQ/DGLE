/**
\author		Korotkov Andrey aka DRON
\date		24.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "HDDFile.h"

using namespace std;

CHDDFile::CHDDFile(uint uiInstIdx, const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags) :
CInstancedObj(uiInstIdx), _file()
{
	const string	file_name = GetFileName(pcName),
					file_path = GetFilePath(pcName);

	if (file_name.size() < MAX_PATH)
		strcpy(_acName, file_name.c_str());
	else
		strcpy(_acName, "\"file_name\" is too long.");

	if (file_path.size() < MAX_PATH)
		strcpy(_acPath, file_path.c_str());
	else
		strcpy(_acPath, "\"file_path\" is too long.");

	char mode[4], *write_pos = mode;

	if (eFlags & FSOF_TRUNC)
	{
		if (!(eFlags & FSOF_WRITE))
		{
			LOG("Can't open file \""s + pcName + "\": 'FSOF_TRUNC' specified without 'FSOF_WRITE'.", LT_ERROR);
			return;
		}
		*write_pos++ = 'w';
		if (eFlags & FSOF_READ)
			*write_pos++ = '+';
	}
	else
	{
		*write_pos++ = 'r';
		if (eFlags & FSOF_WRITE)
			*write_pos++ = '+';
	}

	*write_pos++ = eFlags & FSOF_BINARY ? 'b' : 't';
	*write_pos = '\0';
	
	if (!(_file = fopen(pcName, mode)))
		LOG("Can't open file \""s + pcName + "\".", LT_ERROR);
}

CHDDFile::~CHDDFile()
{
	if (fclose(_file) != 0)
		LOG("Can't close file \""s + _acName + "\".", LT_ERROR);
}

DGLE_RESULT DGLE_API CHDDFile::Read(void *pBuffer, uint uiCount, uint &uiRead)
{
	uiRead = fread(pBuffer, 1, uiCount, _file);
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::Write(const void *pBuffer, uint uiCount, uint &uiWritten)
{
	uiWritten = fwrite(pBuffer, 1, uiCount, _file);
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position)
{
	long offset = ui32Offset;
	const int origin = [eWay, &offset]
	{
		switch (eWay)
		{
		case FSSF_BEGIN:					return SEEK_SET;
		case FSSF_CURRENT:					return SEEK_CUR;
		case FSSF_END: offset = -offset;	return SEEK_END;
		default:
			assert(false);
			__assume(false);
		}
	}();
	if (fseek(_file, offset, origin) != 0)
		return E_FAIL;
	ui32Position = ftell(_file);
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::GetSize(uint32 &ui32Size)
{
	const auto pos = ftell(_file);
	if (fseek(_file, 0, SEEK_END) != 0)
		return E_FAIL;
	ui32Size = ftell(_file);
	if (fseek(_file, pos, SEEK_SET) != 0)
		return E_FAIL;
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::IsOpen(bool &bOpened)
{
	bOpened = _file;
	return S_OK;
}

DGLE_RESULT DGLE_API CHDDFile::GetName(char *pcName, uint &uiCharsCount)
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