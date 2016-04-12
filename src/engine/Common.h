/**
\author		Korotkov Andrey aka DRON
\date		12.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#if defined _MSC_VER && _MSC_VER < 1900
#error old compiler version
#endif

#include "DGLE.h"

#define USE_GLEW_HEADER
#define OPENGL_LEGACY_BASE_OBJECTS
#include "DGLE_CoreRenderer.h"

using namespace DGLE;

// Platform specific //

#ifdef PLATFORM_WINDOWS

typedef HMODULE TDynLibHandle;

#define PLUGIN_FILE_EXTENSION ".dll"

#if defined(_MSC_VER)
#	ifdef _WIN64
//#		pragma warning(disable : 4267)
//#		pragma warning(disable : 4244)
#	endif
#endif

#else //PLATFORM_WINDOWS

#define MAX_PATH 260
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32)(uchar)(ch0) | ((uint32)(uchar)(ch1) << 8) | ((uint32)(uchar)(ch2) << 16) | ((uint32)(uchar)(ch3) << 24))

#endif

// Standart includes //

#ifdef _DEBUG
#	include <new>
#endif

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <strstream>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <tuple>
#include <functional>
#include <chrono>
#include <system_error>
#include <filesystem>

#if defined _MSC_VER && _MSC_VER <= 1900
// VS 2015 ships filesystem as TS
namespace fs = std::experimental::filesystem;
#else
// for fully C++17 conformant toolchain
namespace fs = std::filesystem;
#endif

#include "Utils.h"
#include "FuncDelegate.h"

#define ENGINE_PLATFORM_BASE
#include "PlatformAPI.h"

inline auto GetPerfTimer() noexcept
{
	using namespace std::chrono;
	typedef std::conditional_t<high_resolution_clock::is_steady, high_resolution_clock, steady_clock> clock;
	return clock::now().time_since_epoch();
}

template<class Duration>
inline auto GetPerfTimer() noexcept
{
	return std::chrono::duration_cast<Duration>(GetPerfTimer());
}

class CCore;
class CConsole;

struct TEngInstance
{
	E_GET_ENGINE_FLAGS eGetEngFlags;
	CConsole *pclConsole;
	CCore *pclCore;

	TEngInstance():
		eGetEngFlags(GEF_DEFAULT),
		pclConsole(NULL),
		pclCore(NULL)
	{}
};

TEngInstance *EngineInstance(uint uiInstIdx);

class CInstancedObj
{
	const uint _uiInstIdx;

protected:

	CInstancedObj(uint uiInstIdx):_uiInstIdx(uiInstIdx){}

public:

	inline uint InstIdx() const 
	{
		return _uiInstIdx;
	}
	
	inline CConsole *Console() const 
	{
		return EngineInstance(_uiInstIdx)->pclConsole;
	}

	inline CCore *Core() const 
	{
		return EngineInstance(_uiInstIdx)->pclCore;
	}
};

#include "Core.h"
#include "Console.h"

// Build options //

//Uncomment define below to exclude OpenGL legacy builtin renderer
//If NO_BUILTIN_RENDERER is uncommented you also must exclude glew.c from build.
//#define NO_BUILTIN_RENDERER

//Uncomment define below to exclude builtin sound realisation
//#define NO_BUILTIN_SOUND

//Uncomment define below to exclude builtin input realisation
//#define NO_BUILTIN_INPUT

// Platform specific build options //

#ifdef PLATFORM_WINDOWS

//Uncomment define below to build engine without using any from DirectX
//#define NO_DIRECTX

#	ifndef _DEBUG
//Uncomment define below to use PDB files based exception filter and get call stack on fatals
//#	define PDB_DEBUG
#		if defined(DGLE_USE_COM) && !defined(NO_DIRECTX)
#			define DXDIAG_VIDEO_INFO
#		endif
#	endif

#endif //PLATFORM_WINDOWS

// Defines //

#define DGLE_VERSION _DGLE_VER_" (" __TIMESTAMP__ ")"

// Externs //

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);
inline void LogWrite(uint uiInstIdx, const std::string &str, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	LogWrite(uiInstIdx, str.c_str(), eType, pcSrcFileName, iSrcLineNumber);
}

// Macroses //

#define PTHIS(cl_name) (reinterpret_cast<cl_name *>(pParameter))
#define CON(cl_name, txt) PTHIS(cl_name)->Console()->Write(txt, false)
#define LOG(txt, type) LogWrite(this->InstIdx(), txt, type, ExtractFilename(__FILE__), __LINE__)
#define LOG2(cl_name, txt, type) LogWrite(PTHIS(cl_name)->InstIdx(), txt, type, ExtractFilename(__FILE__), __LINE__)

#define LOG_POINT2(p) LogWrite(this->InstIdx(), ('(' + ToStrFmt(p.x) + ',' + ToStrFmt(p.y) + ')').c_str(), LT_INFO, ExtractFilename(__FILE__), __LINE__)
#define LOG_POINT3(p) LogWrite(this->InstIdx(), ('(' + ToStrFmt(p.x) + ',' + ToStrFmt(p.y) + ',' + ToStrFmt(p.z) + ')').c_str(), LT_INFO, ExtractFilename(__FILE__), __LINE__)
#define LOG_MATRIX4(m) LogWrite(this->InstIdx(), ('[' + ToStrFmt(m._2D[0][0]) + ',' + ToStrFmt(m._2D[0][1]) + ',' + ToStrFmt(m._2D[0][2]) + ',' + ToStrFmt(m._2D[0][3]) + '\n'\
	+ ' ' + ToStrFmt(m._2D[1][0]) + ',' + ToStrFmt(m._2D[1][1]) + ',' + ToStrFmt(m._2D[1][2]) + ',' + ToStrFmt(m._2D[1][3]) + '\n'\
	+ ' ' + ToStrFmt(m._2D[2][0]) + ',' + ToStrFmt(m._2D[2][1]) + ',' + ToStrFmt(m._2D[2][2]) + ',' + ToStrFmt(m._2D[2][3]) + '\n'\
	+ ' ' + ToStrFmt(m._2D[3][0]) + ',' + ToStrFmt(m._2D[3][1]) + ',' + ToStrFmt(m._2D[3][2]) + ',' + ToStrFmt(m._2D[3][3]) + ']').c_str(), LT_INFO, ExtractFilename(__FILE__), __LINE__)

#define IENGINE_BASE_OBJECT_IMPLEMENTATION(object_type) \
DGLE_RESULT DGLE_API Free() override\
{\
	bool can_delete;\
	Core()->pResMan()->RemoveResource(this, can_delete);\
	if (can_delete)\
	{\
		delete this;\
		return S_OK;\
	}\
	else\
		return S_FALSE;\
}\
DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType) override\
{\
	eObjType = object_type;\
	return S_OK;\
}\
DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType) override\
{\
	uiObjUnknownType = -1;\
	return S_FALSE;\
}