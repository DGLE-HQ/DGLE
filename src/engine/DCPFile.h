/**
\author		Korotkov Andrey aka DRON
\date		23.04.2012 (c)Sivkov Ilya

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

class CDCPFile : public CInstancedObj, public IFile
{
	uint8	*_pBuffer;
	uint32	_ui32Size;
	uint32	_ui32SeekPos;
	char	_acName[MAX_PATH], _acPath[MAX_PATH];
	
public:
	CDCPFile(uint uiInstIdx, const char* pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, uint8 *pBuffer, uint32 ui32Size);

	HRESULT CALLBACK Read(void *pBuffer, uint uiCount, uint &uiRead);
	HRESULT CALLBACK Write(const void *pBuffer, uint uiCount, uint &uiWritten);
	HRESULT CALLBACK Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position);
	HRESULT CALLBACK GetSize(uint32 &ui32Size);
	HRESULT CALLBACK IsOpen(bool &bOpened);
	HRESULT CALLBACK GetName(char *pcName, uint uiCharsCount);
	HRESULT CALLBACK GetPath(char *pcPath, uint uiCharsCount);
	HRESULT CALLBACK Free();

	IDGLE2_BASE_IMPLEMENTATION(IFile);
};