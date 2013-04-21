/**
\author		Korotkov Andrey aka DRON
\date		22.01.2010 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "..\Common.h"

extern HMODULE hModule;

class CResFile: public CInstancedObj, public IFile
{
	char _acName[MAX_PATH];
	uint32 _ui32Pos, _ui32Size;
	bool _bOpened;
	HGLOBAL _hRes;
	LPVOID _pData;

public:

	CResFile(uint uiInstIdx, WORD resId);
	~CResFile();

	DGLE_RESULT DGLE_API Read(void *pBuffer, uint uiCount, uint &uiRead);
	DGLE_RESULT DGLE_API Write(const void *pBuffer, uint uiCount, uint &uiWritten);
	DGLE_RESULT DGLE_API Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position);
	DGLE_RESULT DGLE_API GetSize(uint32 &ui32Size);
	DGLE_RESULT DGLE_API IsOpen(bool &bOpened);
	DGLE_RESULT DGLE_API GetName(char *pcName, uint &uiCharsCount);
	DGLE_RESULT DGLE_API GetPath(char *pcPath, uint &uiCharsCount);
	DGLE_RESULT DGLE_API Free();

	IDGLE_BASE_IMPLEMENTATION(IFile, INTERFACE_IMPL_END)
};