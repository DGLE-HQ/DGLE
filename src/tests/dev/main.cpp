//==============================================================================================================================//
//																																//
// Attention! This project is for debugging engine purpose only. It's not a tutorial nor sample it's only for engine developers.//
// You can do testing for any feature which you are working on here. And please do not commit this file!						//
//																																//
//==============================================================================================================================//

#include <DGLE2.h>

using namespace DGLE2;

ENG_DYNAMIC_FUNC

#if defined(PLATFORM_WINDOWS)

#ifdef _DEBUG // for debug builds paths are configured for debugging from DGLE2 project
#	ifdef _WIN64
#		define DLL_PATH			"..\\..\\..\\bin\\win\\x64\\DGLE2.dll"
#		define EXT_PLUG_PATH	"..\\..\\..\\bin\\win\\x64\\plugins\\DGLE2_EXT.dplug"
#		define RESOURCE_PATH	"..\\..\\..\\..\\resources\\"
#	else
#		define DLL_PATH			"..\\..\\..\\bin\\win\\DGLE2.dll"
#		define EXT_PLUG_PATH	"..\\..\\..\\bin\\win\\plugins\\DGLE2_EXT.dplug"
#		define RESOURCE_PATH	"..\\..\\..\\resources\\"
#	endif
#else // for release build paths are configured to run executeble itself
#	define DLL_PATH				"..\\DGLE2.dll"
#	define EXT_PLUG_PATH		"..\\plugins\\DGLE2_EXT.dplug"
#	define RESOURCE_PATH		"..\\..\\..\\..\\resources\\"
#endif

#else // PLATFORM_WINDOWS
#	error Unknown platform!
#endif

#define APP_CAPTION	"DevTest"

IEngineCore *pEngineCore = NULL;
uint uiCounter = 0;

void CALLBACK Init(void *pParametr)
{

}

void CALLBACK Free(void *pParametr)
{

}

void CALLBACK Update(void *pParametr)
{
	uiCounter++;
}

void CALLBACK Render(void *pParametr)
{

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngWindow(800, 600, false, false, MM_NONE, EWF_ALLOW_SIZEING), 33, EIF_LOAD_ALL_PLUGINS)))
		{
			pEngineCore->ConsoleExec("core_fps_in_caption 1");
			pEngineCore->AddProcedure(EPT_INIT, &Init);
			pEngineCore->AddProcedure(EPT_FREE, &Free);
			pEngineCore->AddProcedure(EPT_UPDATE, &Update);
			pEngineCore->AddProcedure(EPT_RENDER, &Render);
			pEngineCore->StartEngine();
		}

		FreeEngine();
	}
	else
		MessageBox(NULL, "Couldn't load \""DLL_PATH"\"!", APP_CAPTION, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

	return 0;
}