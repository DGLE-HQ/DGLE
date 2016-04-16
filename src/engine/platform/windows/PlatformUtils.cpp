/**
\author		Korotkov Andrey aka DRON
\date		16.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"

#ifdef DXDIAG_VIDEO_INFO
#include <dxdiag.h>
#pragma message("Linking with \"dxguid.lib\".")
#pragma comment(linker, "/defaultlib:dxguid.lib")
#endif

#define _WIN32_DCOM 1
#include <wbemidl.h>
#include <comutil.h>
#pragma message("Linking with \"comsuppw.lib\" and \"wbemuuid.lib\".")
#pragma comment(linker, "/defaultlib:comsuppw.lib")
#pragma comment(linker, "/defaultlib:wbemuuid.lib")

#include <direct.h>

#include <psapi.h>
#pragma message("Linking with \"psapi.lib\".")
#pragma comment(linker, "/defaultlib:psapi.lib")

using namespace std;

extern HMODULE hModule;

namespace DGLE
{

TWindowMessage WinAPIMsgToEngMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_PAINT:
		return TWindowMessage(WMT_REDRAW);

	case WM_SHOWWINDOW:
		if (wParam == TRUE && lParam == 0)
			return TWindowMessage(WMT_PRESENT);
		else
			return TWindowMessage();
		
	case WM_CLOSE:
		return TWindowMessage(WMT_CLOSE);

	case WM_CREATE:
		return TWindowMessage(WMT_CREATE);

	case WM_DESTROY:
		return TWindowMessage(WMT_DESTROY);

	case WM_SETFOCUS:
		return TWindowMessage(WMT_ACTIVATED, wParam);

	case WM_KILLFOCUS:
		return TWindowMessage(WMT_DEACTIVATED, wParam);

	case WM_MOVING:
		return TWindowMessage(WMT_MOVE, ((RECT*)lParam)->left, ((RECT*)lParam)->top, (RECT*)lParam);

	case WM_SIZING:
		// This message receives window rectangle not client, so this message should be override in window message handler.
		return TWindowMessage(WMT_SIZE, ((RECT*)lParam)->right - ((RECT*)lParam)->left, ((RECT*)lParam)->bottom - ((RECT*)lParam)->top, (RECT*)lParam);

	case WM_SIZE:
		RECT r;
		r.left = r.top = 0;
		r.right = LOWORD(lParam);
		r.bottom = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
			return TWindowMessage(WMT_MINIMIZED, r.right, r.bottom, &r);					
		else
			if (wParam == SIZE_RESTORED)
				return TWindowMessage(WMT_RESTORED, r.right, r.bottom, &r);			
			else
				return TWindowMessage(WMT_SIZE, r.right, r.bottom, &r);

	case WM_KEYUP:
		if (lParam & 0x00100000)
		{
			if (wParam == 16)
				return TWindowMessage(WMT_KEY_UP, KEY_RSHIFT);
			else
				if (wParam == 17)
					return TWindowMessage(WMT_KEY_UP, KEY_RCONTROL);
				else
					return TWindowMessage(WMT_KEY_UP, ASCIIKeyToEngKey((uchar)wParam));
		}
		else
			return TWindowMessage(WMT_KEY_UP, ASCIIKeyToEngKey((uchar)wParam));

	case WM_KEYDOWN:
		if (lParam & 0x00100000)
		{
			if (wParam == 16)
				return TWindowMessage(WMT_KEY_DOWN, KEY_RSHIFT);
			else
				if (wParam == 17)
					return TWindowMessage(WMT_KEY_DOWN, KEY_RCONTROL);
				else
					return TWindowMessage(WMT_KEY_DOWN, ASCIIKeyToEngKey((uchar)wParam));
		}
		else
			return TWindowMessage(WMT_KEY_DOWN, ASCIIKeyToEngKey((uchar)wParam));

	case WM_SYSKEYUP:
		if (wParam == VK_MENU)
			return TWindowMessage(WMT_KEY_UP, lParam & 0x00100000 ? KEY_RALT : KEY_LALT);
		else
			return TWindowMessage(WMT_KEY_UP, ASCIIKeyToEngKey((uchar)wParam));
	
	case WM_SYSKEYDOWN:
		if (wParam == VK_MENU)
			return TWindowMessage(WMT_KEY_DOWN, lParam & 0x00100000 ? KEY_RALT : KEY_LALT);
		else
			return TWindowMessage(WMT_KEY_DOWN, ASCIIKeyToEngKey((uchar)wParam));

	case WM_CHAR:
		return TWindowMessage(WMT_INPUT_CHAR, (uint32)wParam);

	case WM_MOUSEMOVE:
		return TWindowMessage(WMT_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam));

	case WM_NCMOUSELEAVE:
		return TWindowMessage(WMT_MOUSE_LEAVE);

	case WM_LBUTTONDOWN:
		return TWindowMessage(WMT_MOUSE_DOWN, 0);

	case WM_MBUTTONDOWN:
		return TWindowMessage(WMT_MOUSE_DOWN, 2);

	case WM_RBUTTONDOWN:
		return TWindowMessage(WMT_MOUSE_DOWN, 1);

	case WM_LBUTTONUP:
		return TWindowMessage(WMT_MOUSE_UP, 0);

	case WM_MBUTTONUP:
		return TWindowMessage(WMT_MOUSE_UP, 2);

	case WM_RBUTTONUP:
		return TWindowMessage(WMT_MOUSE_UP, 1);

	case WM_MOUSEWHEEL: 
		int delta;
		delta = GET_WHEEL_DELTA_WPARAM(wParam);
		return TWindowMessage(WMT_MOUSE_WHEEL, 0, 0, (PINT)delta);

	default: 
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		msg.lParam = lParam;
		msg.wParam = wParam;
		msg.message = Msg;
		return TWindowMessage(WMT_UNKNOWN, 0, 0, &msg);
	}
}

void EngMsgToWinAPIMsg(const TWindowMessage &msg, UINT &Msg, WPARAM &wParam, LPARAM &lParam)
{
	switch (msg.eMessage)
	{
	case WMT_REDRAW:
		Msg = WM_PAINT;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_PRESENT:
		Msg = WM_SHOWWINDOW;
		wParam = TRUE;
		lParam = 0;
		break;

	case WMT_CLOSE:
		Msg = WM_CLOSE;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_CREATE:
		Msg = WM_CREATE;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_DESTROY:
		Msg = WM_DESTROY;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_ACTIVATED:
		Msg = WM_SETFOCUS;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_DEACTIVATED:
		Msg = WM_KILLFOCUS;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_MINIMIZED:
		Msg = WM_SIZE;
		wParam = SIZE_MINIMIZED;
		lParam = MAKELPARAM((WORD)msg.ui32Param1, (WORD)msg.ui32Param2);
		break;

	case WMT_MOUSE_LEAVE:
		Msg = WM_NCMOUSELEAVE;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_MOVE:
		Msg = WM_MOVING;
		wParam = 0;
		if (msg.pParam3)
			lParam = LPARAM(msg.pParam3);
		else
			lParam = 0;
		break;

	case WMT_RESTORED:
	case WMT_SIZE:
		Msg = WM_SIZE;
		wParam = SIZE_RESTORED;
		lParam = MAKELPARAM((WORD)msg.ui32Param1, (WORD)msg.ui32Param2);
		break;

	case WMT_KEY_UP:
		Msg = WM_KEYUP;
		wParam = EngKeyToASCIIKey((uint8)msg.ui32Param1);
		lParam = 0;
		break;

	case WMT_KEY_DOWN:
		Msg = WM_KEYDOWN;
		wParam = EngKeyToASCIIKey((uint8)msg.ui32Param1);
		lParam = 0;
		break;

	case WMT_INPUT_CHAR:
		Msg = WM_CHAR;
		wParam = msg.ui32Param1;
		lParam = 0;
		break;

	case WMT_MOUSE_MOVE:
		Msg = WM_MOUSEMOVE;
		wParam = 0;
		lParam = MAKELPARAM((WORD)msg.ui32Param1, (WORD)msg.ui32Param2);
		break;

	case WMT_MOUSE_DOWN:
		if (msg.ui32Param1 == 0)
			Msg = WM_LBUTTONDOWN;
		else
			if (msg.ui32Param1 == 1)
					Msg = WM_RBUTTONDOWN;
				else
					Msg = WM_MBUTTONDOWN;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_MOUSE_UP:
		if (msg.ui32Param1 == 0)
			Msg = WM_LBUTTONUP;
		else
			if (msg.ui32Param1 == 1)
					Msg = WM_RBUTTONUP;
				else
					Msg = WM_MBUTTONUP;
		wParam = 0;
		lParam = 0;
		break;

	case WMT_MOUSE_WHEEL:
		Msg = WM_MOUSEWHEEL;
		if (msg.pParam3)
			wParam = MAKEWPARAM(0, *(int*)msg.pParam3);
		else
			wParam = 0;
		lParam = 0;
		break;
	}
}

void GetDisplaySize(uint &width, uint &height)
{
	HDC desktop_dc = GetDC(GetDesktopWindow());
	width = GetDeviceCaps(desktop_dc, HORZRES);
	height = GetDeviceCaps(desktop_dc, VERTRES);
	ReleaseDC(GetDesktopWindow(), desktop_dc);
}

bool PlatformInit()
{
	LoadKeyboardLayout("00000409", KLF_ACTIVATE);

	return true;
}

void OutputDebugTxt(const char *pcTxt)
{
	OutputDebugString((string(pcTxt) + '\n').c_str());
}

struct TTimer
{
	uint uiId;
	TProcDelegate *pDelegate;
};

vector<TTimer> timers;

void DGLE_API TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	const auto found = find_if(timers.cbegin(), timers.cend(), [idEvent](decltype(timers)::const_reference timer) { return timer.uiId == idEvent; });
	if (found != timers.cend())
		found->pDelegate->operator ()();
}

uint CreateTimer(uint uiInterval, TProcDelegate *pDelOnTimer)
{
	TTimer t;
	t.pDelegate = pDelOnTimer;

	UINT id = 0;

	if (t.uiId = (uint)SetTimer(NULL, id, uiInterval, TimerProc), t.uiId == 0)
		return -1;
	else
	{
		timers.push_back(t);
		return t.uiId;
	}
}

bool ReleaseTimer(uint id)
{
	const auto found = find_if(timers.cbegin(), timers.cend(), [id](decltype(timers)::const_reference timer) { return timer.uiId == id; });
	if (found != timers.cend())
		timers.erase(found);

	return KillTimer(NULL, id) != FALSE;
}

TDynLibHandle LoadDynamicLib(const char *pcFileName)
{
	return LoadLibraryA(pcFileName);
}

void *GetFuncAddress(TDynLibHandle tHandle, const char *pcFuncName)
{
	return GetProcAddress(tHandle, pcFuncName);
}

bool ReleaseDynamicLib(TDynLibHandle tHandle)
{
	return FreeLibrary(tHandle) != FALSE;
}

void ShowModalUserAlert(const char *pcTxt, const char *pcCaption)
{
	MessageBoxA(NULL, pcTxt, pcCaption, MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_SYSTEMMODAL);
}

fs::path GetEngineFilePath()
{
	char eng_file_name[MAX_PATH];
		
	if (GetModuleFileNameA(hModule, eng_file_name, MAX_PATH) == 0)
		strcpy(eng_file_name, "");

	return fs::path(eng_file_name).parent_path();
}

#ifdef DXDIAG_VIDEO_INFO
HRESULT GetStringValue(IDxDiagContainer *pObject, WCHAR *wstrName, TCHAR *strValue, int nStrLen)
{
	HRESULT hr;
	VARIANT var;
	VariantInit(&var);

	if (FAILED(hr = pObject->GetProp(wstrName, &var)))
		return hr;

	if (var.vt != VT_BSTR)
		return E_INVALIDARG;
	
#ifdef _UNICODE
	wcsncpy(strValue, var.bstrVal, nStrLen - 1);
#else
	wcstombs(strValue, var.bstrVal, nStrLen);
#endif

	strValue[nStrLen-1] = TEXT('\0');
	VariantClear( &var );

	return S_OK;
}
#endif

BOOL IsWindowsVersionOrGreater(DWORD major, DWORD minor)
{
	OSVERSIONINFOEX osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};
	ULONGLONG const condition_mask = VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = major;
	osvi.dwMinorVersion = minor;

	return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, condition_mask);
}

BOOL EqualsProductType(BYTE productType)
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.wProductType = productType;
	ULONGLONG condition_mask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);
	return VerifyVersionInfo(&osvi, VER_PRODUCT_TYPE, condition_mask);
}

void GetSystemInformation(string &strInfo, TSystemInfo &stSysInfo)
{
	string result = "System Information\n\t";

	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bool is_ok = true;

	if (IsWindowsVersionOrGreater(6, 3) != FALSE) // if is Windows 8.1 or greater avoid deprecated API
	{
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;
		osvi.wProductType = EqualsProductType(VER_NT_WORKSTATION) != FALSE ? VER_NT_WORKSTATION : VER_NT_SERVER;
		
		HKEY h_key;
		uint32 dw_type;
		uint32 dw_size;

		int32 ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &h_key);

		dw_size = 0;
		ret = RegQueryValueEx(h_key, "CurrentVersion", NULL, &dw_type, NULL, &dw_size);
		_ASSERT(dw_size > 0);

		char *tmp_string = new char[dw_size];
		ret = RegQueryValueEx(h_key, "CurrentVersion", NULL, &dw_type, (LPBYTE)tmp_string, &dw_size);

		if (ret == ERROR_SUCCESS)
		{
			const char *ver_part(strtok(tmp_string, "."));
			assert(ver_part);
			osvi.dwMajorVersion = strtoul(ver_part, NULL, 10);
			ver_part = strtok(NULL, "");
			assert(ver_part);
			osvi.dwMinorVersion = strtoul(ver_part, NULL, 10);
		}

		delete[] tmp_string;

		if (ret != ERROR_SUCCESS || osvi.dwMajorVersion == 0)
			for (uint32 i = osvi.dwMajorVersion; i < osvi.dwMajorVersion + 3; ++i)
				for (uint32 j = 0; j < 6; ++j)
					if (IsWindowsVersionOrGreater(i, j) != FALSE)
					{
						if (i > osvi.dwMajorVersion)
						{
							osvi.dwMajorVersion = i;
							osvi.dwMinorVersion = j;
						}
						else
							if (j > osvi.dwMinorVersion)
								osvi.dwMinorVersion = j;
					}

		dw_size = 0;
		ret = RegQueryValueEx(h_key, "CSDVersion", NULL, &dw_type, NULL, &dw_size);

		if (ret == ERROR_SUCCESS)
		{
			_ASSERT(dw_size > 0);

			if (dw_size <= sizeof(osvi.szCSDVersion))
				RegQueryValueEx(h_key, "CSDVersion", NULL, &dw_type, (LPBYTE)osvi.szCSDVersion, &dw_size);
		}

		dw_size = 0;
		ret = RegQueryValueEx(h_key, "CurrentBuildNumber", NULL, &dw_type, NULL, &dw_size);
		_ASSERT(dw_size > 0);

		tmp_string = new char[dw_size];
		ret = RegQueryValueEx(h_key, "CurrentBuildNumber", NULL, &dw_type, (LPBYTE)tmp_string, &dw_size);

		if (ret == ERROR_SUCCESS)
			osvi.dwBuildNumber = strtol(tmp_string, NULL, 10);

		delete[] tmp_string;

		RegCloseKey(h_key);
	}
	else
		is_ok = GetVersionEx((OSVERSIONINFO*)&osvi) != FALSE;

	string str = "Operating System: ";

	if (!is_ok) 
		str += "Couldn't get OS version!"; 
	else
	{
		if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion > 4)
		{	
				str += to_string(osvi.dwMajorVersion) + '.' + to_string(osvi.dwMinorVersion) + ' ';
				
				str += "\"Microsoft Windows ";

				if (osvi.dwMajorVersion == 6)
				{
					if (osvi.dwMinorVersion == 0)
					{
						if (osvi.wProductType == VER_NT_WORKSTATION)
							str += "Vista ";
						else
							str += "Server 2008 ";
					}
					else
						if (osvi.dwMinorVersion == 1)
						{
							if (osvi.wProductType == VER_NT_WORKSTATION)
								str += "7 ";
							else
								str += "Server 2008 R2 ";
						}
						else
							if (osvi.dwMinorVersion == 2)
							{
								if (osvi.wProductType == VER_NT_WORKSTATION)
									str += "8 ";
								else
									str += "Server 2012 ";
							}
							else
								if (osvi.dwMinorVersion == 3)
								{
									if (osvi.wProductType == VER_NT_WORKSTATION)
										str += "8.1 ";
									else
										str += "Server 2012 R2 ";
								}
								else
									if (osvi.dwMinorVersion == 4)
									{
										if (osvi.wProductType == VER_NT_WORKSTATION)
											str += "10 ";
										else
											str += "10 Server "; // marketing name yet unknown
									}
									else
										if (osvi.wProductType != VER_NT_WORKSTATION)
											str += "Server ";

					DWORD os_type;
					typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
					PGPI p_gpi;
					p_gpi = (PGPI)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetProductInfo");
					
					if (p_gpi(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &os_type) != 0)
						switch ( os_type )
						{
							case PRODUCT_UNLICENSED:
								str += "Unlicensed copy";
								break;
							case PRODUCT_ULTIMATE:
							case PRODUCT_ULTIMATE_N:
							case PRODUCT_ULTIMATE_E:
								str += "Ultimate Edition";
								break;
							case PRODUCT_PROFESSIONAL:
							case PRODUCT_PROFESSIONAL_N:
							case PRODUCT_PROFESSIONAL_E:
								str += "Professional";
								break;
							case PRODUCT_HOME_PREMIUM:
							case PRODUCT_HOME_PREMIUM_N:
							case PRODUCT_HOME_PREMIUM_E:
								str += "Home Premium Edition";
								break;
							case PRODUCT_HOME_BASIC:
							case PRODUCT_HOME_BASIC_N:
							case PRODUCT_HOME_BASIC_E:
								str += "Home Basic Edition";
								break;
							case PRODUCT_ENTERPRISE:
							case PRODUCT_ENTERPRISE_N:
							case PRODUCT_ENTERPRISE_E:
								str += "Enterprise Edition";
								break;
							case PRODUCT_BUSINESS:
							case PRODUCT_BUSINESS_N:
								str += "Business Edition";
								break;
							case PRODUCT_STARTER:
							case PRODUCT_STARTER_N:
							case PRODUCT_STARTER_E:
								str += "Starter Edition";
								break;
						}
				}
				else
					if (osvi.dwMajorVersion == 5)
					{
						if (osvi.dwMinorVersion == 0)
						{
							str += "2000 ";

							if (osvi.wProductType == VER_NT_WORKSTATION)
								str += "Professional";
							else 
							{
								if (osvi.wSuiteMask & VER_SUITE_DATACENTER )
									str += "Datacenter Server";
								else
									if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
										str += "Advanced Server";
									else
										str += "Server";
							}
						}
						else
							if (osvi.dwMinorVersion == 1)
							{
								str += "XP ";

								if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
									str += "Home Edition";
								else
									str += "Professional";

							}
							else
								if (osvi.dwMinorVersion == 2)
								{
									if (GetSystemMetrics(SM_SERVERR2))
										str += "Server 2003 R2, ";
									else
										if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
											str += "Storage Server 2003";
										else 
											if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
												str += "Home Server";
											else
												if (osvi.wProductType == VER_NT_WORKSTATION)
													str += "XP Professional x64 Edition";
												else
													str += "Server 2003";
						
									if (osvi.wProductType != VER_NT_WORKSTATION)
									{
										if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
											str += "Compute Cluster Edition";
										else 
											if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
												str += "Datacenter Edition";
											else
												if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
													str += "Enterprise Edition";
												else
													if (osvi.wSuiteMask & VER_SUITE_BLADE)
														str += "Web Edition";
													else
														str += "Standard Edition";
									}

								}
								else							
									if (osvi.wProductType != VER_NT_WORKSTATION)
										str += "Server";
					}

				str += '\"' + (strlen(osvi.szCSDVersion) > 0 ? ' ' + (string)osvi.szCSDVersion : "") + (osvi.dwBuildNumber != 0 ? " (Build " + to_string(osvi.dwBuildNumber) + ')' : "");

				if (osvi.dwMajorVersion >= 6)
				{
#ifdef _WIN64
					str+= ", 64-bit";
#else
					typedef BOOL (WINAPI *PISWOW64)(HANDLE, PBOOL);
					PISWOW64 p_is_wow;
					BOOL b_is_wow = FALSE;
					p_is_wow = (PISWOW64)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");
					if (NULL != p_is_wow) p_is_wow(GetCurrentProcess(), &b_is_wow);

					if (b_is_wow)
						str+= ", 64-bit";
					else
						str+= ", 32-bit";
#endif
				}

		}
		else 
			str += "Unsupported or unknown Microsoft Windows version.";

		result += str + "\n\t";

		if (str.size() < 128)
			strcpy(stSysInfo.cOSName, str.c_str());
		else
		{
			memcpy(stSysInfo.cOSName, str.c_str(), 127);
			stSysInfo.cOSName[127] = '\0';
		}
	} 

	HKEY h_key;
	uint32 dw_type;
	uint32 dw_size;
	uint32 mhz;
	
	int32 ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &h_key);
	
	dw_size = 0;
	ret = RegQueryValueEx(h_key, "ProcessorNameString", NULL, &dw_type, NULL, &dw_size);
	_ASSERT(dw_size > 0);
	
	char *pc_processor_name = new char[dw_size];
	ret = RegQueryValueEx(h_key, "ProcessorNameString", NULL, &dw_type, (LPBYTE)pc_processor_name, &dw_size);
	
	if (ret != ERROR_SUCCESS)
		pc_processor_name = "Couldn't get CPU name!";

	dw_size = sizeof(DWORD);
	ret = RegQueryValueEx(h_key, "~MHz", NULL, &dw_type, (LPBYTE)&mhz, &dw_size);
	if (ret != ERROR_SUCCESS)
		pc_processor_name = "Couldn't get CPU speed!";

	RegCloseKey(h_key);

	SYSTEM_INFO st_sys_info; 
	ZeroMemory(&st_sys_info, sizeof(SYSTEM_INFO));
	::GetSystemInfo((SYSTEM_INFO*) &st_sys_info);

	str = "CPU: ";
	str += pc_processor_name + " (~"s + to_string(mhz) + " Mhz";
	if (st_sys_info.dwNumberOfProcessors > 1) str += " X " + to_string(st_sys_info.dwNumberOfProcessors);
	str += ')';

	result += str + "\n\t";
	stSysInfo.uiCPUFrequency = mhz;
	stSysInfo.uiCPUCount = st_sys_info.dwNumberOfProcessors;
	
	if (strlen(pc_processor_name) <= 128)
		strcpy(stSysInfo.cCPUName, pc_processor_name);
	else
	{
		memcpy(stSysInfo.cCPUName, pc_processor_name, 127);
		stSysInfo.cCPUName[127] = '\0';
	}
	
	delete[] pc_processor_name;

	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx(&stat);

	uint64 ram_free = (uint64)ceil(stat.ullAvailPhys / 1024.0 / 1024.0),
	ram_total = (uint64)ceil(stat.ullTotalPhys / 1024.0 / 1024.0);

	str = "RAM Total: " + to_string(ram_total) + " MiB";
	result += str + "\n\t";
	str = "RAM Available: " + to_string(ram_free) + " MiB";
	result += str + "\n\t";

	stSysInfo.uiRAMAvailable = (uint)ram_free;
	stSysInfo.uiRAMTotal = (uint)ram_total;

	str = "Video device: ";

	HRESULT hr, chr = CoInitialize(NULL);

	string vcard_advanced_str = "";

#ifdef DXDIAG_VIDEO_INFO
	IDxDiagProvider * p_dx_diag_provider;

	hr = E_FAIL;

	if (SUCCEEDED(chr))
		hr = CoCreateInstance( CLSID_DxDiagProvider, NULL, CLSCTX_INPROC_SERVER, IID_IDxDiagProvider, (LPVOID *)&p_dx_diag_provider);
	
	if (SUCCEEDED(hr))
	{
		DXDIAG_INIT_PARAMS dx_diag_init_param;
		ZeroMemory(&dx_diag_init_param, sizeof(DXDIAG_INIT_PARAMS));

		dx_diag_init_param.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
		dx_diag_init_param.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
		dx_diag_init_param.bAllowWHQLChecks        = BOOL(false);
		dx_diag_init_param.pReserved               = NULL;

		hr = p_dx_diag_provider->Initialize(&dx_diag_init_param);

		IDxDiagContainer *p_dx_diag_root;

		hr = p_dx_diag_provider->GetRootContainer(&p_dx_diag_root);

		IDxDiagContainer *p_container = NULL;

		hr = p_dx_diag_root->GetChildContainer(L"DxDiag_DisplayDevices", &p_container);
		
		DWORD nInstanceCount = 0;
		hr = p_container->GetNumberOfChildContainers(&nInstanceCount);

		if (nInstanceCount > 1)
			str += "(Count: " + to_string(nInstanceCount) + ") ";

		stSysInfo.uiVideocardCount = nInstanceCount;

		IDxDiagContainer *p_object = NULL;

		wchar wc_container[256];

		hr = p_container->EnumChildContainerNames(0, wc_container, 256);
		hr = p_container->GetChildContainer(wc_container, &p_object);

		#define EXPAND(x) x, sizeof(x) / sizeof(TCHAR)
		
		char txt[200];

		GetStringValue(p_object, L"szDescription", EXPAND(txt));
		str += txt;

		strncpy(stSysInfo.cVideocardName, txt, size(stSysInfo.cVideocardName) - 1);
		*prev(end(stSysInfo.cVideocardName)) = '\0';

		GetStringValue( p_object, L"szDisplayMemoryLocalized", EXPAND(txt));
		
		stSysInfo.uiVideocardRAM = strtoul(txt, nullptr, 10);

		GetStringValue( p_object, L"szDriverName", EXPAND(txt));
		vcard_advanced_str = "(driver \""s + txt + "\",";

		GetStringValue( p_object, L"szDriverVersion", EXPAND(txt));
		vcard_advanced_str += " ver. "s + txt + ',';

		GetStringValue( p_object, L"szDriverDateLocalized", EXPAND(txt));
		vcard_advanced_str += " date "s + txt + ')';

		p_object->Release();
		p_container->Release();
		p_dx_diag_root->Release();
		p_dx_diag_provider->Release();
	}
	else 
	{
#endif

	DISPLAY_DEVICE dd;

	ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
	dd.cb = sizeof(DISPLAY_DEVICE);

	if (EnumDisplayDevices(NULL, 0, (DISPLAY_DEVICE*)&dd, 0) == 0)
		str = "Error getting video device!";
	else
		str += dd.DeviceString;

	stSysInfo.uiVideocardRAM = 0;
	stSysInfo.uiVideocardCount = 0;

	strcpy(stSysInfo.cVideocardName, dd.DeviceString); 

#ifdef DXDIAG_VIDEO_INFO
	}
#endif

	IWbemLocator *locator = NULL;
	uint vram = 0;
	hr = E_FAIL;

	if (SUCCEEDED(chr))
		hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void**)&locator);

	if (SUCCEEDED(hr))
	{
		IWbemServices *services = NULL;
		hr = locator->ConnectServer(_bstr_t("root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &services);
		locator->Release();

		if (SUCCEEDED(hr))
		{
			hr = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
			if (FAILED(hr))
				services->Release();
			else
			{
				IEnumWbemClassObject *instance_enum = NULL;
				hr = services->CreateInstanceEnum(_bstr_t("Win32_VideoController"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &instance_enum);
				services->Release();
				
				if (SUCCEEDED(hr))
				{
					IWbemClassObject *instance;
					ULONG objectsReturned = 0;

					while (true)
					{
						hr = instance_enum->Next(WBEM_INFINITE, 1, &instance, &objectsReturned);
						
						if (FAILED(hr))
							break;

						if (objectsReturned != 1)
							break;

						VARIANT v;
						VariantInit(&v);
						hr = instance->Get(_bstr_t("AdapterRAM"), 0, &v, NULL, NULL);
						
						if (SUCCEEDED(hr))
							vram = (uint)ceil(V_UI4(&v) / 1024.0 / 1024.0);

						VariantClear(&v);
						instance->Release();
						instance = NULL;
					}

					instance_enum->Release();
				}
			}
		}
	}

	if (SUCCEEDED(chr))
		CoUninitialize();

	if (vram != 0)
		stSysInfo.uiVideocardRAM = vram;

	result += str + ' ' + to_string(stSysInfo.uiVideocardRAM) + " MiB " + vcard_advanced_str;

	strInfo = result;
}

uint32 GetProcessMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS pmc;
	
	if (FALSE == GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(PROCESS_MEMORY_COUNTERS)))
		return 0;
	else
		return pmc.WorkingSetSize;
}

}