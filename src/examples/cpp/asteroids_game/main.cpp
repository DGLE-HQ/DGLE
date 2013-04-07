#include "game.h" // already include DGLE header and namespace

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "Asteroids Game"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#endif

#define SCREEN_WIDTH	1024u
#define SCREEN_HEIGHT	768u

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	IEngineCore *pEngineCore = NULL;
	
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		CGame game(pEngineCore);
		
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_4X))))
			pEngineCore->StartEngine();

		FreeEngine();
	}
	else
		MessageBox(NULL,"Couldn't load \""DLL_PATH"\"!", APP_CAPTION, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

	return 0;
}