/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include <DGLE.h>

#include <string>
#include <vector>

#include "utils.h"

using namespace DGLE;

#define PLUGIN_NAME				"Plugin Template"
#define PLUGIN_VERSION			"1.0 (" __DATE__ ")"
#define PLUGIN_VENDOR			"Your Company Name"
#define PLUGIN_DESCRIPTION		"Just simple template for any DGLE plugin."
#define PLUGIN_INTERFACE_NAME	"IPlugin"

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);
inline void LogWrite(uint uiInstIdx, const std::string &str, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	LogWrite(uiInstIdx, str.c_str(), eType, pcSrcFileName, iSrcLineNumber);
}

#define LOG(txt, type) LogWrite(_uiInstIdx, txt, type, ExtractFilename(__FILE__), __LINE__)
#define LOG_POINT2(p) LogWrite(_uiInstIdx, ('(' + FloatToStrFmt(p.x) + ',' + FloatToStrFmt(p.y) + ')').c_str(), LT_INFO, ExtractFilename(__FILE__), __LINE__)
#define LOG_POINT3(p) LogWrite(_uiInstIdx, ('(' + FloatToStrFmt(p.x) + ',' + FloatToStrFmt(p.y) + ',' + FloatToStrFmt(p.z) + ')').c_str(), LT_INFO, ExtractFilename(__FILE__), __LINE__)
#define LOG_MATRIX4(m) LogWrite(_uiInstIdx, ('[' + FloatToStrFmt(m._2D[0][0]) + ',' + FloatToStrFmt(m._2D[0][1]) + ',' + FloatToStrFmt(m._2D[0][2]) + ',' + FloatToStrFmt(m._2D[0][3]) + '\n'\
	+ ' ' + FloatToStrFmt(m._2D[1][0]) + ',' + FloatToStrFmt(m._2D[1][1]) + ',' + FloatToStrFmt(m._2D[1][2]) + ',' + FloatToStrFmt(m._2D[1][3]) + '\n'\
	+ ' ' + FloatToStrFmt(m._2D[2][0]) + ',' + FloatToStrFmt(m._2D[2][1]) + ',' + FloatToStrFmt(m._2D[2][2]) + ',' + FloatToStrFmt(m._2D[2][3]) + '\n'\
	+ ' ' + FloatToStrFmt(m._2D[3][0]) + ',' + FloatToStrFmt(m._2D[3][1]) + ',' + FloatToStrFmt(m._2D[3][2]) + ',' + FloatToStrFmt(m._2D[3][3]) + ']').c_str(), LT_INFO, ExtractFilename(__FILE__), __LINE__)
