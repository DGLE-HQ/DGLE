/**
\author		Korotkov Andrey aka DRON
\date		17.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include <DGLE.h>
#include <string>

std::string ToLowerCase(std::string str);
std::string ToUpperCase(std::string str);

bool StrToBool(std::string str);

std::string ToStrX(DGLE::uint val);
std::string ToStrFmt(double val);
std::string ToStrExp(double val);
std::string BoolToStr(bool val);

std::string GetFilePath(const char *name);
std::string GetFileName(const char *name);
std::string GetFileExt(const char *name);
std::string GetOnlyFileName(const char *name);

DGLE::uchar EngKeyToASCIIKey(const DGLE::uint8 key);
DGLE::uint8 ASCIIKeyToEngKey(const DGLE::uchar key);

DGLE::uint32 GetCRC32(const DGLE::uint8 *pData, DGLE::uint32 ui32Size);

DGLE::uint8 GetPixelDataAlignmentIncrement(DGLE::uint uiLineWidth, DGLE::uint8 ui8BytesPerPixel, DGLE::uint8 ui8Alignment);