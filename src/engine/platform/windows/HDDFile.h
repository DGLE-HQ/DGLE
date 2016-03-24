/**
\author		Korotkov Andrey aka DRON
\date		24.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CInstancedObj;

class CHDDFile final : public CInstancedObj, public IFile
{
	/*
		FILE used rather than fstream because no C++ features like RAII and exception safety required here for this C style code
		and it have less memory footprint for not opened files.
		fstream though provides better portability regarding seeking relative to the end of file and for text files
		so it can be used in future if required.
	*/
	std::FILE *_file;
	char	_acName[MAX_PATH],
			_acPath[MAX_PATH];

public:

	CHDDFile(uint uiInstIdx, const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags = FSOF_READ);
	~CHDDFile();

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