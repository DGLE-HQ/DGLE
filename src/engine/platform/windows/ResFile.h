/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

extern HMODULE hModule;

class CResFile final : public CInstancedObj, public IFile
{
	char _acName[MAX_PATH];
	uint32 _ui32Pos, _ui32Size;
	bool _bOpened;
	HGLOBAL _hRes;
	LPVOID _pData;

public:

	CResFile(uint uiInstIdx, WORD resId);
	~CResFile();

	DGLE_RESULT DGLE_API Read(void *pBuffer, uint uiCount, uint &uiRead) override;
	DGLE_RESULT DGLE_API Write(const void *pBuffer, uint uiCount, uint &uiWritten) override;
	DGLE_RESULT DGLE_API Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position) override;
	DGLE_RESULT DGLE_API GetSize(uint32 &ui32Size) override;
	DGLE_RESULT DGLE_API IsOpen(bool &bOpened) override;
	DGLE_RESULT DGLE_API GetName(char *pcName, uint &uiCharsCount) override;
	DGLE_RESULT DGLE_API GetPath(char *pcPath, uint &uiCharsCount) override;
	DGLE_RESULT DGLE_API Free() override;

	IDGLE_BASE_IMPLEMENTATION(IFile, INTERFACE_IMPL_END)
};