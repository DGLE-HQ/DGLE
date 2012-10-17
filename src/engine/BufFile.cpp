/**
\author		Kotlyarov Semen aka Kimer
\date		16.03.2011 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "BufFile.h"

CBufFile::CBufFile(uint8 *pData, uint32 ui32DataSize):
_pData(pData),
_ui32Pos(0),
_ui32Size(ui32DataSize)
{
	strcpy(_acName, (std::string("Virtual.") + UIntToStr(*pData)).c_str());
}

DGLE_RESULT CBufFile::Read(void *pBuffer, uint uiCount, uint &uiRead)
{
	if (memcpy(pBuffer, (char*)(_pData) + _ui32Pos, uiCount) == 0)
	{
		_ui32Pos += uiCount;
		uiRead = uiCount;
		return S_OK;
	}
	else
	{
		uiRead = 0;
		return S_FALSE;
	}
}

DGLE_RESULT CBufFile::Write(const void *pBuffer, uint uiCount, uint &uiWritten)
{
	return E_FAIL;
}

DGLE_RESULT CBufFile::Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position)
{
	switch(eWay)
	{
	case FSSF_BEGIN:
		_ui32Pos = ui32Offset;
		break;
	case FSSF_END:
		_ui32Pos = _ui32Size - ui32Offset;
		break;
	case FSSF_CURRENT:
		_ui32Pos += ui32Offset;
		break;
	}
	
	ui32Position = _ui32Pos;

	return S_OK;
}

DGLE_RESULT CBufFile::GetSize(uint32 &ui32Size)
{
	ui32Size = _ui32Size;
	return S_OK;
}

DGLE_RESULT CBufFile::IsOpen(bool &bOpened)
{
	bOpened = true;
	return S_OK;
}

DGLE_RESULT CBufFile::GetName(char *pcName, uint &uiCharsCount)
{
	if (!pcName)
	{
		uiCharsCount = strlen(_acName) + 1;
		return S_OK;
	}

	if (uiCharsCount <= strlen(_acName))
	{
		uiCharsCount = strlen(_acName) + 1;
		strcpy(pcName, "");
		return E_INVALIDARG;
	}

	strcpy(pcName, _acName);

	return S_OK;
}

DGLE_RESULT DGLE_API CBufFile::GetPath(char *pcPath, uint &uiCharsCount)
{
	if (!pcPath)
	{
		uiCharsCount = 1;
		return S_OK;
	}

	strcpy(pcPath, "");
	
	return S_OK;
}

DGLE_RESULT DGLE_API CBufFile::Free()
{
	delete this;
	return S_OK;
}