/**
\author		Kotlyarov Semen aka Kimer
\date		16.03.2011 (c)Kotlyarov Semen

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

class CBufFile: public IFile
{
	char		_acName[MAX_PATH];
	uint32		_ui32Pos,
				_ui32Size;
	uint8		*_pData;

public:

	CBufFile(uint8 *pData, uint32 ui32DataSize);

	HRESULT DGLE2_API Read(void *pBuffer, uint uiCount, uint &uiRead);
	HRESULT DGLE2_API Write(const void *pBuffer, uint uiCount, uint &uiWritten);
	HRESULT DGLE2_API Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position);
	HRESULT DGLE2_API GetSize(uint32 &ui32Size);
	HRESULT DGLE2_API IsOpen(bool &bOpened);
	HRESULT DGLE2_API GetName(char *pcName, uint uiCharsCount);
	HRESULT DGLE2_API GetPath(char *pcPath, uint uiCharsCount);
	HRESULT DGLE2_API Free();

	IDGLE2_BASE_IMPLEMENTATION(IFile)
};