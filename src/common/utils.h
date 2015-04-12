/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include <DGLE.h>
#include <string>

std::string ToLowerCase(const std::string &inString);
std::string ToUpperCase(const std::string &inString);

int StrToInt(const std::string &str);
DGLE::uint StrToUInt(const std::string &str);
float StrToFloat(const std::string &str);
bool StrToBool(const std::string &str);

std::string IntToStr(int val);
std::string Int64ToStr(DGLE::int64 val);
std::string UIntToStr(DGLE::uint val);
std::string UInt64ToStr(DGLE::uint64 val);
std::string UIntToStrX(DGLE::uint val);
std::string FloatToStr(float val);
std::string FloatToStrFmt(float val);
std::string DoubleToStr(double val);
std::string BoolToStr(bool val);

std::string GetFilePath(const char *name);
std::string GetFileName(const char *name);
std::string GetFileExt(const char *name);
std::string GetOnlyFileName(const char *name);

DGLE::uchar EngKeyToASCIIKey(const DGLE::uint8 key);
DGLE::uint8 ASCIIKeyToEngKey(const DGLE::uchar key);

DGLE::uint32 GetCRC32(const DGLE::uint8 *pData, DGLE::uint32 ui32Size);

DGLE::uint8 GetPixelDataAlignmentIncrement(DGLE::uint uiLineWidth, DGLE::uint8 ui8BytesPerPixel, DGLE::uint8 ui8Alignment);