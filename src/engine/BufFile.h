/**
\author		Kotlyarov Semen aka Kimer
\date		16.03.2011 (c)Kotlyarov Semen

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _BUFFILE_H
#define _BUFFILE_H

#include "Common.h"

class CBufFile: public IFile
{
	char		_acName[MAX_PATH];
	uint32		_ui32Pos,
				_ui32Size;
	uint8		*_pData;

public:

	CBufFile(uint8 *pData, uint32 ui32DataSize);

	HRESULT CALLBACK Read(void *pBuffer, uint uiCount, uint &uiRead);
	HRESULT CALLBACK Write(const void *pBuffer, uint uiCount, uint &uiWritten);
	HRESULT CALLBACK Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position);
	HRESULT CALLBACK GetSize(uint32 &ui32Size);
	HRESULT CALLBACK IsOpen(bool &bOpened);
	HRESULT CALLBACK GetName(char *pcName, uint uiCharsCount);
	HRESULT CALLBACK GetPath(char *pcPath, uint uiCharsCount);
	HRESULT CALLBACK Free();

	IDGLE2_BASE_IMPLEMENTATION(IFile)
};

#endif //_BUFFILE_H