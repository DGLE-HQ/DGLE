/**
\author		Sivkov Ilya
\date		23.04.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "DCPFile.h"

CDCPFile::CDCPFile(uint uiInstIdx, const char* pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, uint8 *pBuffer, uint32 ui32Size):
CInstancedObj(uiInstIdx), _ui32SeekPos(0)
{
	strcpy(_acName, GetFileName(pcName).c_str());
	strcpy(_acPath, GetFilePath(pcName).c_str());
	
	_pBuffer = pBuffer;
	_ui32Size = ui32Size;
}

DGLE_RESULT DGLE_API CDCPFile::Read(void *pBuffer, uint uiCount, uint &uiRead)
{	
	uiRead = uiCount > _ui32Size - _ui32SeekPos ? _ui32Size - _ui32SeekPos : uiCount;

	memcpy(pBuffer, (void*)&_pBuffer[_ui32SeekPos], uiRead);

	_ui32SeekPos += uiRead;

	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFile::Write(const void *pBuffer, uint uiCount, uint &uiWritten)
{
	return E_NOTIMPL;
}

DGLE_RESULT DGLE_API CDCPFile::Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position)
{
	switch(eWay)
	{
	case FSSF_BEGIN: _ui32SeekPos =  ui32Offset; break;
	case FSSF_END: _ui32SeekPos = _ui32Size - ui32Offset; break;
	case FSSF_CURRENT: _ui32SeekPos += ui32Offset; break;
	}

	ui32Position = _ui32SeekPos;

	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFile::GetSize(uint32 &ui32Size)
{
	ui32Size = _ui32Size;
	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFile::IsOpen(bool &bOpened)
{
	if (_pBuffer)
		bOpened = true;
	else
		bOpened = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFile::GetName(char *pcName, uint &uiCharsCount)
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

DGLE_RESULT DGLE_API CDCPFile::GetPath(char *pcPath, uint &uiCharsCount)
{
	if (!pcPath)
	{
		uiCharsCount = strlen(_acPath) + 1;
		return S_OK;	
	}
	
	if (uiCharsCount <= strlen(_acPath))
	{
		uiCharsCount = strlen(_acPath) + 1;
		strcpy(pcPath, "");
		return E_INVALIDARG;
	}

	strcpy(pcPath, _acPath);

	return S_OK;
}

DGLE_RESULT DGLE_API CDCPFile::Free()
{
	delete[] _pBuffer;
	delete this;
	
	return S_OK;
}