/**
\author		Korotkov Andrey aka DRON
\date		19.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Utils.h"

#include <cctype>
#include <cstdio>
#include <algorithm>
#include <utility>
#include <limits>

using namespace DGLE;
using namespace std;

string ToLowerCase(string str)
{
	transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

string ToUpperCase(string str)
{
	transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

bool StrToBool(string str)
{
	const string val = ToLowerCase(move(str));

	return
		val == "true"		||
		val == "on"			||
		val == "enabled"	||
		val == "1"			||
		val == "active";
}

string ToStrX(uint val)
{
	char res[2/*0x*/ + numeric_limits<decltype(val)>::digits10 + 1/*correction for digits10*/ + 1/*terminator*/];
	sprintf(res, "0x%x", val);
	return res;
}

string ToStrFmt(double val)
{
	char res[16];
	snprintf(res, size(res), "% .4f", val);
	return res;
}

string ToStrExp(double val)
{
	char res[16];
	snprintf(res, size(res), "%e", val);
	return res;
}

string BoolToStr(bool val)
{
	return val ? "true" : "false";
}

string GetFilePath(const char *name)
{
	string path(name);
	
	if (path.empty()) 
		return path;
	
	switch (path[path.length() - 1])
	{
	case '\\':
	case '/':
		path.pop_back();
	}

	const string::size_type pos = path.find_last_of("\\/");
	
	if (pos != string::npos)
	{
		path.erase(pos).shrink_to_fit();
		return path;
	}
	else 
		return "";
}

string GetFileName(const char *name)
{
	string path(name);
	
	if (path.empty())
		return path;
	
	switch (path[path.length() - 1])
	{
	case '\\':
	case '/':
		path.pop_back();
	}

	const string::size_type pos = path.find_last_of("\\/");
	
	if (pos != string::npos)
		path.erase(0, pos + 1).shrink_to_fit();
	
	return path;
}

string GetFileExt(const char *name)
{
	string path(name);
	const string::size_type pos = path.find_last_of('.');
	if (pos != string::npos)
		path.erase(0, pos + 1).shrink_to_fit();
	return path;
}

string GetOnlyFileName(const char *name)
{
	string path(GetFileName(name));
	const string::size_type pos = path.find_last_of('.');
	if (pos != string::npos)
		path.erase(pos).shrink_to_fit();
	/*
		Shrinking can non-beneficial for short extensions
		but shrink_to_fit() is non-binding anyway.
		Leave the decision concerning shrinking to the stdlib implementation.
	*/
	return path;
}

uchar EngKeyToASCIIKey(const uint8 key)
{
	switch(key)
	{
		case KEY_ESCAPE			: return 27;
		case KEY_TAB			: return 9;
		case KEY_GRAVE			: return 192;
		case KEY_CAPSLOCK		: return 20;
		case KEY_BACKSPACE		: return 8;
		case KEY_RETURN			: return 13;
		case KEY_SPACE			: return 32;
		case KEY_SLASH			: return 191;
		case KEY_BACKSLASH		: return 220; 

		case KEY_SYSRQ			: return 44;
		case KEY_SCROLL			: return 145;
		case KEY_PAUSE			: return 19;

		case KEY_INSERT			: return 45;
		case KEY_DELETE			: return 46;
		case KEY_HOME			: return 36;
		case KEY_END			: return 35;
		case KEY_PGUP			: return 33;
		case KEY_PGDN			: return 34;

		case KEY_LSHIFT			: return 16;
		case KEY_RSHIFT			: return 16;
		case KEY_LALT			: return 18;
		case KEY_RALT			: return 18; 
		case KEY_LCONTROL		: return 17;
		case KEY_RCONTROL		: return 17;

		case KEY_UP				: return 38;
		case KEY_RIGHT			: return 39;
		case KEY_LEFT			: return 37;
		case KEY_DOWN			: return 40;

		case KEY_0				: return 48;
		case KEY_1				: return 49;
		case KEY_2				: return 50;
		case KEY_3				: return 51;
		case KEY_4				: return 52;
		case KEY_5				: return 53;
		case KEY_6				: return 54;
		case KEY_7				: return 55;
		case KEY_8				: return 56;
		case KEY_9				: return 57;

		case KEY_F1				: return 112;
		case KEY_F2				: return 113;
		case KEY_F3				: return 114;
		case KEY_F4				: return 115;
		case KEY_F5				: return 116;
		case KEY_F6				: return 117;
		case KEY_F7				: return 118;
		case KEY_F8				: return 119;
		case KEY_F9				: return 120;
		case KEY_F10			: return 121;
		case KEY_F11			: return 122;
		case KEY_F12			: return 123;

		case KEY_Q				: return 81;
		case KEY_W				: return 87;
		case KEY_E				: return 69;
		case KEY_R				: return 82;
		case KEY_T				: return 84;
		case KEY_Y				: return 89;
		case KEY_U				: return 85;
		case KEY_I				: return 73;
		case KEY_O				: return 79;
		case KEY_P				: return 80; 
		case KEY_A				: return 65;
		case KEY_S				: return 83;
		case KEY_D				: return 68;
		case KEY_F				: return 70;
		case KEY_G				: return 71;
		case KEY_H				: return 72;
		case KEY_J				: return 74;
		case KEY_K				: return 75;
		case KEY_L				: return 76;
		case KEY_Z				: return 90;
		case KEY_X				: return 88;
		case KEY_C				: return 67;
		case KEY_V				: return 86;
		case KEY_B				: return 66;
		case KEY_N				: return 78;
		case KEY_M				: return 77;

		case KEY_MINUS			: return 189;
		case KEY_PLUS			: return 187;
		case KEY_LBRACKET		: return 219;
		case KEY_RBRACKET		: return 221; 

		case KEY_SEMICOLON		: return 186;
		case KEY_APOSTROPHE		: return 222; 

		case KEY_COMMA			: return 188;
		case KEY_PERIOD			: return 190;

		case KEY_NUMPAD0		: return 96;
		case KEY_NUMPAD1		: return 97;
		case KEY_NUMPAD2		: return 98;
		case KEY_NUMPAD3		: return 99;
		case KEY_NUMPAD4		: return 100;
		case KEY_NUMPAD5		: return 101;
		case KEY_NUMPAD6		: return 102;
		case KEY_NUMPAD7		: return 103;
		case KEY_NUMPAD8		: return 104;
		case KEY_NUMPAD9		: return 105;
		case KEY_NUMPADPERIOD	: return 110;
		case KEY_NUMPADSTAR		: return 106;
		case KEY_NUMPADPLUS		: return 107;
		case KEY_NUMPADMINUS	: return 109;
		case KEY_NUMPADSLASH	: return 111;
		case KEY_NUMLOCK		: return 144;
		default : return 0;
	}
}

uint8 ASCIIKeyToEngKey(const uchar key)
{
	switch(key)
	{
		case 27             : return KEY_ESCAPE;
		case 9              : return KEY_TAB;
		case 192            : return KEY_GRAVE;
		case 20	            : return KEY_CAPSLOCK;
		case 8	            : return KEY_BACKSPACE;
		case 13             : return KEY_RETURN;
		case 32             : return KEY_SPACE;
		case 191            : return KEY_SLASH;
		case 220            : return KEY_BACKSLASH; 

		case 44             : return KEY_SYSRQ;
		case 145            : return KEY_SCROLL;
		case 19             : return KEY_PAUSE;

		case 45             : return KEY_INSERT;
		case 46             : return KEY_DELETE;
		case 36             : return KEY_HOME;
		case 35             : return KEY_END;
		case 33             : return KEY_PGUP;
		case 34             : return KEY_PGDN;

		case 16             : return KEY_LSHIFT;
		case 18		        : return KEY_LALT;
		case 17             : return KEY_LCONTROL;

		case 38             : return KEY_UP;
		case 39             : return KEY_RIGHT;
		case 37             : return KEY_LEFT;
		case 40             : return KEY_DOWN;

		case 48			    : return KEY_0;
		case 49             : return KEY_1;
		case 50             : return KEY_2;
		case 51             : return KEY_3;
		case 52             : return KEY_4;
		case 53             : return KEY_5;
		case 54             : return KEY_6;
		case 55             : return KEY_7;
		case 56             : return KEY_8;
		case 57             : return KEY_9;

		case 112		    : return KEY_F1;
		case 113            : return KEY_F2;
		case 114            : return KEY_F3;
		case 115            : return KEY_F4;
		case 116            : return KEY_F5;
		case 117            : return KEY_F6;
		case 118            : return KEY_F7;
		case 119            : return KEY_F8;
		case 120            : return KEY_F9;
		case 121            : return KEY_F10;
		case 122            : return KEY_F11;
		case 123            : return KEY_F12;

		case 81             : return KEY_Q;
		case 87             : return KEY_W;
		case 69             : return KEY_E;
		case 82             : return KEY_R;
		case 84             : return KEY_T;
		case 89             : return KEY_Y;
		case 85             : return KEY_U;
		case 73             : return KEY_I;
		case 79             : return KEY_O;
		case 80             : return KEY_P; 
		case 65             : return KEY_A;
		case 83             : return KEY_S;
		case 68             : return KEY_D;
		case 70             : return KEY_F;
		case 71             : return KEY_G;
		case 72             : return KEY_H;
		case 74             : return KEY_J;
		case 75             : return KEY_K;
		case 76             : return KEY_L;
		case 90             : return KEY_Z;
		case 88             : return KEY_X;
		case 67             : return KEY_C;
		case 86             : return KEY_V;
		case 66             : return KEY_B;
		case 78             : return KEY_N;
		case 77             : return KEY_M;

		case 189            : return KEY_MINUS;
		case 187            : return KEY_PLUS;
		case 219            : return KEY_LBRACKET;
		case 221            : return KEY_RBRACKET; 

		case 186            : return KEY_SEMICOLON;
		case 222            : return KEY_APOSTROPHE; 

		case 188            : return KEY_COMMA;
		case 190            : return KEY_PERIOD;

		case 96             : return KEY_NUMPAD0;
		case 97             : return KEY_NUMPAD1;
		case 98             : return KEY_NUMPAD2;
		case 99             : return KEY_NUMPAD3;
		case 100            : return KEY_NUMPAD4;
		case 101            : return KEY_NUMPAD5;
		case 102            : return KEY_NUMPAD6;
		case 103            : return KEY_NUMPAD7;
		case 104            : return KEY_NUMPAD8;
		case 105            : return KEY_NUMPAD9;
		case 110            : return KEY_NUMPADPERIOD;
		case 106            : return KEY_NUMPADSTAR;
		case 107            : return KEY_NUMPADPLUS;
		case 109            : return KEY_NUMPADMINUS;
		case 111            : return KEY_NUMPADSLASH;
		case 144            : return KEY_NUMLOCK;
		default : return 0;
	}	
}

uint32 GetCRC32(const uint8 *pData, uint32 ui32Size)
{
	if (pData == NULL || ui32Size == 0)
		return 0;

	static uint32 crc_table[256];
	static bool gen_crc = true;

	uint32 crc;

	if (gen_crc)
	{
		for (int i = 0; i < 256; ++i)
		{
			crc = i;
			for (int j = 0; j < 8; ++j)
				crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

			crc_table[i] = crc;
		};

		gen_crc = false;
	}

	crc = 0xFFFFFFFFUL;

	while (ui32Size--) 
		crc = crc_table[(crc ^ *pData++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
}

uint8 GetPixelDataAlignmentIncrement(uint uiLineWidth, uint8 ui8BytesPerPixel, uint8 ui8Alignment)
{
	const uint8 a = (uiLineWidth * ui8BytesPerPixel) % ui8Alignment;
	
	if (a != 0)
		return ui8Alignment - a;
	else
		return a;
}