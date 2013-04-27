#include "DGLE.h"

using namespace DGLE;

DGLE_DYNAMIC_FUNC

IEngineCore	*pEngineCore = NULL;
IBitmapFont	*pFont = NULL;

void DGLE_API Init(void *pParameter)
{
	IResourceManager *resMan;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)resMan);
	resMan->GetDefaultResource(EOT_BITMAP_FONT, (IEngBaseObj *&)pFont);
}

void DGLE_API Render(void *pParameter)
{
	pFont->Draw2D(0, 0, "Hello World!");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if ( GetEngine("DGLE.dll", pEngineCore) )
	{
		pEngineCore->InitializeEngine(NULL, "My First App");
		pEngineCore->AddProcedure(EPT_INIT, &Init);
		pEngineCore->AddProcedure(EPT_RENDER, &Render);
		pEngineCore->StartEngine();
		FreeEngine();
	}
	else
		MessageBoxA(NULL, "Couldn't load \"DGLE.dll\"!", "My First App", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	
	return 0;
}