/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "ResFile.h"

CResFile::CResFile(uint	uiInstIdx, WORD resId):
CInstancedObj(uiInstIdx),
_hRes(NULL),
_pData(NULL),
_ui32Pos(0),
_ui32Size(0)
{	
	strcpy(_acName,IntToStr(resId).c_str());
	HRSRC res = FindResource(hModule, MAKEINTRESOURCE(resId), "BIN");
	
	if (res)
		_hRes = LoadResource(hModule,res);
	
	_bOpened = _hRes!=NULL;
	
	if (_bOpened)
	{
		_pData	= LockResource(_hRes);
		_ui32Size = SizeofResource(hModule, res);
	}
}

CResFile::~CResFile()
{
	if (_bOpened)
	{
		UnlockResource(_hRes);
		FreeResource(_hRes);
	}
}

DGLE_RESULT CResFile::Read(void *pBuffer, uint uiCount, uint &uiRead)
{
	memcpy(pBuffer, (uint8 *)_pData + _ui32Pos, uiCount);

	_ui32Pos += uiCount;

	uiRead = uiCount;

	return S_OK;
}

DGLE_RESULT CResFile::Write(const void *pBuffer, uint uiCount, uint &uiWritten)
{
	return E_NOTIMPL;
}

DGLE_RESULT CResFile::Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position)
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

DGLE_RESULT CResFile::GetSize(uint32 &ui32Size)
{
	ui32Size = _ui32Size;
	return S_OK;
}

DGLE_RESULT CResFile::IsOpen(bool &bOpened)
{
	bOpened = _bOpened;
	return S_OK;
}

DGLE_RESULT CResFile::GetName(char *pcName, uint &uiCharsCount)
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

DGLE_RESULT DGLE_API CResFile::GetPath(char *pcPath, uint &uiCharsCount)
{
	if (!pcPath)
	{
		uiCharsCount = 1;
		return S_OK;
	}

	if (uiCharsCount < 1)
	{
		uiCharsCount = 1;
		strcpy(pcPath, "");
		return E_INVALIDARG;
	}

	strcpy(pcPath, "");

	return S_OK;
}

DGLE_RESULT DGLE_API CResFile::Free()
{
	delete this;
	return S_OK;
}