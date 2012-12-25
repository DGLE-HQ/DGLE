/**
\author		Korotkov Andrey aka DRON
\date		19.10.2010 (c)Korotkov Andrey

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
#define PLUGIN_VERSION			("1.0 (" + std::string(__DATE__) + ")").c_str()
#define PLUGIN_VENDOR			"Your Company Name"
#define PLUGIN_DESCRIPTION		"Just simple template for any DGLE plugin."
#define PLUGIN_INTERFACE_NAME	"IPlugin"

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

#define LOG(txt, type) LogWrite(_uiInstIdx, std::string(txt).c_str(), type, GetFileName(__FILE__).c_str(), __LINE__)
