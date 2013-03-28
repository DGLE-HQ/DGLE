#include <DGLE.h>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "CoreRenderer and OpenGL"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

#define SCREEN_WIDTH	1024u
#define SCREEN_HEIGHT	768u

IEngineCore *pEngineCore = NULL;

void DGLE_API Init(void *pParameter)
{
	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
}

void DGLE_API Update(void *pParameter)
{
}

void DGLE_API Render(void *pParameter)
{
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false))))
		{
			pEngineCore->AddProcedure(EPT_INIT,	&Init);
			pEngineCore->AddProcedure(EPT_UPDATE, &Update);
			pEngineCore->AddProcedure(EPT_RENDER, &Render);

			pEngineCore->StartEngine();
		}

		FreeEngine();
	}
	else
		MessageBox(NULL,"Couldn't load \""DLL_PATH"\"!", APP_CAPTION, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

	return 0;
}