#include <DGLE.h>

using namespace DGLE;

DGLE_DYNAMIC_FUNC // Include GetEngine and FreeEngine functions to load engine library at runtime.

#define APP_CAPTION "Hello World"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

#define SCREEN_WIDTH	320u
#define SCREEN_HEIGHT	240u

// pointers to engine classes
IEngineCore *pEngineCore = NULL;
IInput *pInput = NULL;

// pointers to engine objects
IBitmapFont *pFont = NULL;
ITexture *pTex = NULL;

void DGLE_API Init(void *pParameter)
{
	// You may load any application data here and get engine subsystems.
	
	pEngineCore->GetSubSystem(ESS_INPUT, (IEngineSubSystem *&)pInput);

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	p_res_man->GetDefaultResource(EOT_BITMAP_FONT, (IEngineBaseObject *&)pFont);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_cloudy_night_sky.jpg", (IEngineBaseObject *&)pTex, TEXTURE_LOAD_DEFAULT_2D);
}

void DGLE_API Free(void *pParameter)
{
	// You may free application data here.
}

void DGLE_API Update(void *pParameter)
{
	// Application update routine put your logic here.

	bool is_pressed;
	
	pInput->GetKey(KEY_ESCAPE, is_pressed);

	if (is_pressed)
		pEngineCore->QuitEngine(); // exit engine main loop
}

void DGLE_API Render(void *pParameter)
{
	// Frame drawing routine.

	// stretch background texture to fit screen
	pTex->Draw2D(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// render text at the center of the screen
	uint font_width, font_height;
	pFont->GetTextDimensions(APP_CAPTION, font_width, font_height);
	pFont->Draw2DSimple((SCREEN_WIDTH - font_width) / 2, (SCREEN_HEIGHT - font_height) / 2, APP_CAPTION, ColorWhite());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Main application entry point.

	if (GetEngine(DLL_PATH, pEngineCore)) // load engine library
	{
		pEngineCore->ConsoleVisible(true); // show engine console (you can use "~" key to show it in any application)

		// engine initialization
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false))))
		{
			// register engine callbacks
			pEngineCore->AddProcedure(EPT_INIT,	&Init);
			pEngineCore->AddProcedure(EPT_FREE,	&Free);
			pEngineCore->AddProcedure(EPT_UPDATE, &Update);
			pEngineCore->AddProcedure(EPT_RENDER, &Render);

			pEngineCore->StartEngine(); // start engine main loop
		}

		FreeEngine(); // unload engine library
	}
	else
		MessageBox(NULL,"Couldn't load \""DLL_PATH"\"!", APP_CAPTION, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

	return 0;
}