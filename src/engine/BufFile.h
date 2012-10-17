/**
\author		Kotlyarov Semen aka Kimer
\date		16.03.2011 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
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

	DGLE_RESULT DGLE_API Read(void *pBuffer, uint uiCount, uint &uiRead);
	DGLE_RESULT DGLE_API Write(const void *pBuffer, uint uiCount, uint &uiWritten);
	DGLE_RESULT DGLE_API Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position);
	DGLE_RESULT DGLE_API GetSize(uint32 &ui32Size);
	DGLE_RESULT DGLE_API IsOpen(bool &bOpened);
	DGLE_RESULT DGLE_API GetName(char *pcName, uint &uiCharsCount);
	DGLE_RESULT DGLE_API GetPath(char *pcPath, uint &uiCharsCount);
	DGLE_RESULT DGLE_API Free();

	IDGLE_BASE_IMPLEMENTATION(IFile)
};