//==============================================================================================================================//
//																																//
// Attention! This project is for debugging engine purpose only. It's not a tutorial nor sample it's only for engine developers.//
// You can do testing for any feature which you are working on here. And please do not commit this file!						//
//																																//
//==============================================================================================================================//

#include <DGLE2.h>
#include <string>

using namespace DGLE2;

ENG_DYNAMIC_FUNC

#if defined(PLATFORM_WINDOWS)

#ifdef _DEBUG // for debug builds paths are configured for debugging from DGLE2 project
#	ifdef _WIN64
#		define DLL_PATH			"..\\..\\..\\bin\\win\\x64\\DGLE2.dll"
#		define EXT_PLUG_PATH	"..\\..\\..\\bin\\win\\x64\\plugins\\DGLE2_EXT.dplug"
#	else
#		define DLL_PATH			"..\\..\\..\\bin\\win\\DGLE2.dll"
#		define EXT_PLUG_PATH	"..\\..\\..\\bin\\win\\plugins\\DGLE2_EXT.dplug"
#	endif
#	define RESOURCE_PATH		"..\\..\\..\\resources\\"
#else // for release build paths are configured to run executeble itself
#	define DLL_PATH				"..\\DGLE2.dll"
#	define EXT_PLUG_PATH		"..\\plugins\\DGLE2_EXT.dplug"
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
IRender2D *pRender2D = NULL;
uint uiCounter = 0;

ITexture *pTex = NULL;

void DGLE2_API Init(void *pParametr)
{
	IResourceManager *prman;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)prman);
	prman->Load(RESOURCE_PATH"tests\\npot_tex.bmp", (IEngBaseObj *&)pTex, TEXTURE_LOAD_DEFAULT_2D);

	IRender *pr;
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)pr);
	pr->GetRender2D(pRender2D);
}

void DGLE2_API Free(void *pParametr)
{
}

void DGLE2_API Update(void *pParametr)
{
	++uiCounter;
}

void DGLE2_API Render(void *pParametr)
{
	pRender2D->Begin2D();
	
	uint w, h;
	pTex->GetDimensions(w, h);

	pTex->Draw2DSimple((SCREEN_X - w)/2, (SCREEN_Y - w)/2);

	pRender2D->End2D();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngWindow(SCREEN_X, SCREEN_Y, false, false, MM_NONE, EWF_ALLOW_SIZEING), 33, EIF_LOAD_ALL_PLUGINS)))
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