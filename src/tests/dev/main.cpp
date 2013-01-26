//==============================================================================================================================//
//																																//
// Attention! This project is for debugging engine purpose only. It's not a tutorial nor sample it's only for engine developers.//
// You can do testing for any feature which you are working on here. And please do not commit this file!						//
//																																//
//==============================================================================================================================//

#include <DGLE.h>
#include <string>

DGLE_DYNAMIC_FUNC

using namespace DGLE;

#if defined(PLATFORM_WINDOWS)

#ifdef _DEBUG // for debug builds paths are configured for debugging from DGLE project
#	ifdef _WIN64
#		define DLL_PATH			"..\\..\\..\\bin\\windows\\x64\\DGLE.dll"
#		define PLUGS_PATH		"..\\..\\..\\bin\\windows\\x64\\plugins\\"
#	else
#		define DLL_PATH			"..\\..\\..\\bin\\windows\\DGLE.dll"
#		define PLUGS_PATH		"..\\..\\..\\bin\\windows\\plugins\\"
#	endif
#	define RESOURCE_PATH		"..\\..\\..\\resources\\"
#else // for release build paths are configured to run executeble itself
#	define DLL_PATH				"..\\DGLE.dll"
#	define PLUGS_PATH			"..\\plugins\\"
#	ifdef _WIN64
#		define RESOURCE_PATH	"..\\..\\..\\..\\resources\\"
#	else
#		define RESOURCE_PATH	"..\\..\\..\\resources\\"
#endif
#endif

#else // PLATFORM_WINDOWS
#	error Unknown platform!
#endif

#define APP_CAPTION	"DevTest"
#define SCREEN_X 800
#define SCREEN_Y 600

IEngineCore *pEngineCore = NULL;
IInput *pInput = NULL;
IRender2D *pRender2D = NULL;
uint uiCounter = 0;

ITexture *pTex = NULL;

void DGLE_API Init(void *pParametr)
{
	pEngineCore->GetSubSystem(ESS_INPUT, (IEngineSubSystem *&)pInput);

	IResourceManager *prman;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)prman);
	prman->Load(RESOURCE_PATH"tests\\npot_tex.bmp", (IEngBaseObj *&)pTex, TEXTURE_LOAD_DEFAULT_2D);

	IRender *pr;
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)pr);
	pr->GetRender2D(pRender2D);
}

void DGLE_API Free(void *pParametr)
{
}

void DGLE_API Update(void *pParametr)
{
	bool b_prsd;

	pInput->GetKey(KEY_ESCAPE, b_prsd);

	if (b_prsd)
		pEngineCore->QuitEngine();

	++uiCounter;
}

void DGLE_API Render(void *pParametr)
{
	uint w, h;
	pTex->GetDimensions(w, h);
	pTex->Draw2DSimple((SCREEN_X - (int)w)/2, (SCREEN_Y - (int)h)/2);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngWindow(SCREEN_X, SCREEN_Y, false, false, MM_NONE, EWF_ALLOW_SIZEING), 33, EIF_LOAD_ALL_PLUGINS)))
		{
			pEngineCore->ConsoleVisible(true);
			pEngineCore->ConsoleExec("core_profiler 1");
			
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