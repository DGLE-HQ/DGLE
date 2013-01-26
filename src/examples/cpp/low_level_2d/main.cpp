#include <DGLE.h>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "Low Level 2D"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

#define HELP_TEXT "You can resize this window to see it's behavior or go fullscreen by pressing \"Alt + Enter\" keys."

// This example is made for 800X600 screen resolution.
#define GAME_VP_WIDTH	800u
#define GAME_VP_HEIGHT	600u

// Screen resolution
#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

IEngineCore *pEngineCore = NULL;
IInput *pInput = NULL;
IRender2D *pRender2D = NULL;

// Textures
ITexture *pBg = NULL, *pSky = NULL, *pFog = NULL, *pTexGirl = NULL, *pLight = NULL,
	*pLightRound = NULL, *pOwl = NULL, *pVox = NULL;

IBitmapFont *pFont = NULL;

ISoundSample *pSndOwl = NULL, *pForestAmbient = NULL;
ISoundChannel *pChannelAmbientLoop = NULL;

// Lights positions
TPoint2 lights[] =
{
	TPoint2(0.f, 0.f),
	TPoint2(750.f,315.f),
	TPoint2(1000.f,450.f),
	TPoint2(1010.f,650.f),
	TPoint2(150.f,550.f)
};

int counter = 800;
uint prevWindowWidth, prevWindowHeight;

TPoint2 cameraPosition;
float cameraAngle = 0.f, cameraScale = 1.f;

TPoint2 stMousePos;

bool owlGoLeft = false;
float owlX = -200.f;

void DGLE_API OnFullScreenEvent(void *pParametr, IBaseEvent *pEvent)
{
	IEvGoFullScreen *p_event = (IEvGoFullScreen *)pEvent;

	uint res_width, res_height;
	bool go_fscreen;
	p_event->GetResolution(res_width, res_height, go_fscreen);

	if (go_fscreen)
	{
		prevWindowWidth = res_width;
		prevWindowHeight = res_height;

		pEngineCore->GetDesktopResolution( res_width, res_height);
		p_event->SetResolution(res_width, res_height);
	}
	else
		p_event->SetResolution(prevWindowWidth, prevWindowHeight);
}

void DGLE_API Init(void *pParametr)
{
	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	pEngineCore->GetSubSystem(ESS_INPUT, (IEngineSubSystem *&)pInput);
	
	IRender *p_render;
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)p_render);
	p_render->GetRender2D(pRender2D);

	// Background clear color setup.
	p_render->SetClearColor(TColor4(38, 38, 55, 255));

	// This example adapted only for 800X600 resolution, if even resolution will be higher it won't effect it.
	pRender2D->SetResolutionCorrection(GAME_VP_WIDTH, GAME_VP_HEIGHT);

	pInput->Configure(ICF_HIDE_CURSOR);

	p_res_man->GetDefaultResource(EOT_BITMAP_FONT, (IEngBaseObj*&)pFont);

	p_res_man->Load(RESOURCE_PATH"sounds\\owl.wav", (IEngBaseObj*&)pSndOwl);
	p_res_man->Load(RESOURCE_PATH"sounds\\forest_ambient.wav", (IEngBaseObj*&)pForestAmbient);

	pForestAmbient->PlayEx(pChannelAmbientLoop, SSP_LOOPED);
	
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_forest_background.png", (IEngBaseObj*&)pBg, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_cloudy_night_sky.jpg", (IEngBaseObj*&)pSky, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"textures\\smoke.png", (IEngBaseObj*&)pFog, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"textures\\light.jpg", (IEngBaseObj*&)pLightRound, TEXTURE_LOAD_DEFAULT_2D);

	p_res_man->Load(RESOURCE_PATH"sprites\\vox.jpg", (IEngBaseObj*&)pVox, TEXTURE_LOAD_DEFAULT_2D);
	pVox->SetFrameSize(149, 149);

	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_owl.png", (IEngBaseObj*&)pOwl, TEXTURE_LOAD_DEFAULT_2D);
	pOwl->SetFrameSize(48, 128);

	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_anime_girl.png", (IEngBaseObj*&)pTexGirl, TEXTURE_LOAD_DEFAULT_2D);
	pTexGirl->SetFrameSize(55, 117);

	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_mistery_light.jpg", (IEngBaseObj*&)pLight, TEXTURE_LOAD_DEFAULT_2D);
	pLight->SetFrameSize(64, 128);
}

void DGLE_API Free(void *pParametr)
{
	// It's a good idea to stop and unaquire looped sound channels before quit.
	pChannelAmbientLoop->Stop();
	pChannelAmbientLoop->Unaquire();
}

void DGLE_API Update(void *pParametr)
{
	bool prsd;
	
	pInput->GetKey(KEY_ESCAPE, prsd);
	
	if (prsd)
	{
		pEngineCore->QuitEngine();
		return;
	}

	TMouseStates states;
	pInput->GetMouseStates(states);
	stMousePos = TPoint2((float)states.iX, (float)states.iY); 

	if (counter % 250 == 0)
	{
		ISoundChannel *chan;
		pSndOwl->PlayEx(chan);
		chan->SetPan((int)(owlX - cameraPosition.x));
		chan->Unaquire();
	}

	cameraPosition.x = GAME_VP_WIDTH / 2.f + 250.f + cos(counter / 200.f) * 225.f;
	cameraPosition.y = GAME_VP_HEIGHT / 2.f + 300.f;

	cameraAngle= sin(counter / 200.f) * 5.f;
	cameraScale= 1.f + fabs(sin(counter / 200.f) * 0.25f);

	lights[0].x = 650.f - cos(counter / 100.f) * 100.f;
	lights[0].y = 700.f - sin(counter / 150.f) * 50.f;

	if (owlX > 1500.f)
		owlGoLeft = true;
	else
		if (owlX < 100.f)
			owlGoLeft = false;

	owlX += (owlGoLeft ? -1.f : 1.f) * 4.f;

	++counter;
}

void DGLE_API Render(void *pParametr)
{
	pRender2D->Begin2D();

	// Draw static sky
	
	pRender2D->DrawTex(pSky, TPoint2(), TPoint2(GAME_VP_WIDTH, GAME_VP_HEIGHT));

	// Draw background

	pRender2D->SetCamera(TPoint2(cameraPosition.x / 1.5f, cameraPosition.y / 1.5f), cameraAngle, TPoint2(cameraScale, cameraScale));

	pRender2D->SetBlendMode(EBF_NORMAL);

	pRender2D->DrawTexCropped(pBg, TPoint2(-200.f, 150.f), TPoint2(1399.f, 517.f), TRectF(0.f, 905.f, 1399.f, 517.f), 0.f, EF_BLEND);

	// Draw moveing fog on background

	pRender2D->SetCamera(TPoint2(cameraPosition.x / 1.2f, cameraPosition.y / 1.2f), cameraAngle, TPoint2(cameraScale, cameraScale));

	pRender2D->SetBlendMode(EBF_ADD);

	// Sometimes it's better to batch a lot of similar objects.

	pRender2D->BeginBatch(); 

	pRender2D->SetColorMix(TColor4(255, 255, 255, 64));
	
	for (uint i = 0; i < 22; ++i)
		for (uint j = 0; j < 2; ++j)
			pRender2D->DrawTex(pFog,
			TPoint2(500.f + sin((float)i) * 600.f - cos((float)counter / 50.f) * 10.f * (i % 4),
					475.f + sin((float)j * 2.5f) * 150.f - sin((float)counter / 50.f) * 20.f * (i % 3)),
			TPoint2(250.f, 150.f), 0.f, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_COLORMIX));

	pRender2D->EndBatch();

	// Draw foreground scene

	// Setup camera and blending
	pRender2D->SetCamera(cameraPosition, cameraAngle, TPoint2(cameraScale, cameraScale));

	pRender2D->SetBlendMode(EBF_NORMAL);

	// Owl

	pRender2D->SetColorMix(TColor4(150, 150, 150, 255)); // make sprite little darker
	pRender2D->DrawTexSprite(pOwl, TPoint2(owlX, 425.f), TPoint2(48.f, 128.f), (counter / 3) % 15, 0.f, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_COLORMIX | (owlGoLeft ? EF_FLIPX : EF_DEFAULT)));

	// Draw tree

	pRender2D->DrawTexCropped(pBg, TPoint2(), TPoint2(1399.f, 900.f), TRectF(0.f, 0.f, 1399.f, 900.f), 0.f, EF_BLEND);

	// Glowing disc under moving light
	
	pRender2D->SetBlendMode(EBF_ADD);
	
	pRender2D->SetVerticesOffset(TPoint2(-100.f, 0.f), TPoint2(-100.f, 0.f), TPoint2(), TPoint2());
	pRender2D->SetColorMix(TColor4(65, 59, 193, 255));
	pRender2D->DrawTex(pLightRound, TPoint2(lights[0].x - 32.f, lights[0].y + 64.f), TPoint2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_OFFSET | EF_BLEND | EF_COLORMIX));

	// Girl Shadow
	
	pRender2D->SetBlendMode(EBF_NORMAL);
	
	pRender2D->SetVerticesOffset(TPoint2(-150.f + cos((float)counter / 100.f) * 100.f, -55.f), TPoint2(-50.f + cos((float)counter / 100.f) * 100.f, -55.f), TPoint2(15.f, 5.f), TPoint2(15.f, 5.f));
	pRender2D->SetColorMix(TColor4(0, 0, 0, 128));
	pRender2D->DrawTexSprite(pTexGirl, TPoint2(550.f, 725.f), TPoint2(60.f, 120.f), (counter / 5) % 16, 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_OFFSET | EF_BLEND | EF_COLORMIX));

	// Girl Sprite

	pRender2D->DrawTexSprite(pTexGirl, TPoint2(550.f, 725.f), TPoint2(60.f, 120.f), (counter / 5) % 16, 0.f, EF_BLEND);

	// Lights

	pRender2D->SetBlendMode(EBF_ADD);

	for (uint i = 0; i < 5; ++i)
		pRender2D->DrawTexSprite(pLight, TPoint2(lights[i].x, lights[i].y), TPoint2(64.f, 128.f), (counter / 2) % 14, 0.f, EF_BLEND);

	pRender2D->ResetCamera();

	// We must calculate correct coordinates in game space because game resolution and screen resolution can be different.
	TPoint2 pos;
	pRender2D->CoordAbsoluteToResCorrect(stMousePos, pos);
	pRender2D->DrawTexSprite(pVox, TPoint2(pos.x - 37, pos.y - 37), TPoint2(75.f, 75.f), (counter / 2) % 16, 0.f, EF_BLEND);

	uint tw, th;
	pFont->GetTextDimensions(HELP_TEXT, tw, th);
	pFont->Draw2D((float)((GAME_VP_WIDTH - tw) / 2), (float)(GAME_VP_HEIGHT - th), HELP_TEXT);

	pRender2D->End2D();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL,APP_CAPTION,TEngWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_NONE, EWF_ALLOW_SIZEING))))
		{
			pEngineCore->AddProcedure(EPT_INIT,	&Init);
			pEngineCore->AddProcedure(EPT_FREE,	&Free);
			pEngineCore->AddProcedure(EPT_UPDATE, &Update);
			pEngineCore->AddProcedure(EPT_RENDER, &Render);
			
			pEngineCore->AddEventListner(ET_ON_FULLSCREEN, &OnFullScreenEvent);

			pEngineCore->StartEngine();
		}
		FreeEngine();
	}
	else
		MessageBox(NULL,"Couldn't load \""DLL_PATH"\"!", APP_CAPTION, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

	return 0;
}