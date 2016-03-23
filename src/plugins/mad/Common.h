/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include <DGLE.h>
#include <mad.h>

#include <string>
#include <vector>

#include "utils.h"

using namespace DGLE;

#define PLUGIN_NAME				"MP3 Audio Decoder"
#define PLUGIN_VERSION			"1.0 (" __DATE__ ")"
#define PLUGIN_VENDOR			"DGLE Team"
#define PLUGIN_DESCRIPTION		"MPEG Layer III (MP3) compatible audio format decoder."
#define PLUGIN_INTERFACE_NAME	"IPlugin"

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);
inline void LogWrite(uint uiInstIdx, const std::string &str, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber)
{
	LogWrite(uiInstIdx, str.c_str(), eType, pcSrcFileName, iSrcLineNumber);
}

#define LOG(txt, type) LogWrite(_uiInstIdx, txt, type, ExtractFilename(__FILE__), __LINE__)
