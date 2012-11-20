/**
\author		Kotlyarov Semen aka Kimer
\date		29.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CBuffFile: public IFile
{
	std::string _strFileName;
	uint32		_ui32Pos,
				_ui32Size;
	uint8		*_pData;
	bool		_bDelData;

public:

	CBuffFile(uint8 *pData, uint32 ui32DataSize, bool bDelData = true, const std::string &strFileName = "");

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