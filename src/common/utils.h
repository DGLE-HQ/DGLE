/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <DGLE.h>
#include <string>

using namespace DGLE;

std::string ToLowerCase(const std::string &inString);
std::string ToUpperCase(const std::string &inString);

int StrToInt(const std::string &str);
uint StrToUInt(const std::string &str);
float StrToFloat(const std::string &str);
bool StrToBool(const std::string &str);

std::string IntToStr(int val);
std::string Int64ToStr(int64 val);
std::string UIntToStr(uint val);
std::string UInt64ToStr(uint64 val);
std::string UIntToStrX(uint val);
std::string FloatToStr(float val);
std::string DoubleToStr(double val);
std::string BoolToStr(bool val);

std::string GetFilePath(const char *name);
std::string GetFileName(const char *name);
std::string GetFileExt(const char *name);
std::string GetOnlyFileName(const char *name);

uchar EngKeyToASCIIKey(const uint8 key);
uint8 ASCIIKeyToEngKey(const uchar key);

uint32 GetCRC32(uint8 *pData, uint32 ui32Size);

uint8 GetDataAlignmentIncrement(uint uiLineWidth, uint8 ui8BytesPerPixel, uint8 ui8Alignment);