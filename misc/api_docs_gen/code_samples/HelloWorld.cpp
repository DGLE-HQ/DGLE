#include "DGLE2.h"

using namespace DGLE2;

ENG_DYNAMIC_FUNC

IEngineCore	*pEngineCore = NULL;
IBitmapFont	*pFont = NULL;
IRender2D	*pRender2D = NULL;

void DGLE2_API Init(void *pParametr)
{
	IResourceManager *resMan;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)resMan);

	IRender *render;
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)render);
	render->GetRender2D(pRender2D);
	
	resMan->GetDefaultResource(EOT_BITMAP_FONT, (IEngBaseObj *&)pFont);
}

void DGLE2_API Render(void *pParametr)
{
	pFont->Draw2D(0, 0, "Hello, World!");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if ( GetEngine("DGLE2.dll", pEngineCore) )
	{
		pEngineCore->InitializeEngine(NULL, "My First App");
		pEngineCore->AddProcedure(EPT_INIT, &Init);
		pEngineCore->AddProcedure(EPT_RENDER, &Render);
		pEngineCore->StartEngine();
		FreeEngine();
	}
	else
		MessageBoxA(NULL, "Couldn't load \"DGLE2.dll\"!", "My First App", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	
	return 0;
}