#include <DGLE.h>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "3D In 2D"

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
IResourceManager *pResMan = NULL;
IRender *pRender = NULL;
IRender2D *pRender2D = NULL;
IInput *pInput = NULL;

TPoint2 stMouseOnScreen, stMouseInCamera;

ITexture *pTexGrass, *pTexStone, *pTexTree1, *pTexTree2, *pTexTree3, *pTexHouse, *pTexCopter, *pTexZombie, *pTexRotor,
	*pMeshTree1Shadow, *pMeshTree2Shadow, *pMeshTree3Shadow, *pMeshHouseShadow, *pMeshCopterShadow, *pRotorShadow;

IMesh *pMeshTree1, *pMeshTree2, *pMeshTree3, *pMeshHouse, *pMeshCopter;

ISoundSample *pHelicopter;
ISoundChannel *pHelicopterSndChan;

TPoint2 stCopterPos = TPoint2(400, 200);
float fCopterAngle = 0.f;

TPoint2 stZombiePos = TPoint2(10, 400);
float fZombieAngle = 0.f, fZombieAnimFrame = 0.f;
bool bFrameForward = true;

#define TO_RAD(x) (x * (float)M_PI / 180.f)
#define TO_DEG(x) (x * 180.f / (float)M_PI)

uint uiCounter = 1;

// This function creates texture and renders models shadow to it.
void RenderMeshToTexture(ITexture *&pTex, IMesh *pMesh, ITexture *pMeshTex)
{
	pResMan->CreateTexture(pTex, NULL, 256, 256, TDF_RGBA8, TCF_DEFAULT, TLF_FILTERING_BILINEAR);
	pRender->SetRenderTarget(pTex);
	pRender2D->DrawMesh(pMesh, pMeshTex, TPoint2(128.f, 128.f), TVector3(256.f, 256.f, 1.f));
	pRender->SetRenderTarget(NULL); // switch back to on-screen drawing
}

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_INPUT,(IEngineSubSystem *&)pInput);
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER,(IEngineSubSystem *&)pResMan);
	pEngineCore->GetSubSystem(ESS_RENDER,(IEngineSubSystem *&)pRender);
	
	pRender->GetRender2D(pRender2D);

	pResMan->Load(RESOURCE_PATH"sounds\\helicopter.wav", (IEngineBaseObject *&)pHelicopter);
	pHelicopter->PlayEx(pHelicopterSndChan, SSP_LOOPED);

	pResMan->Load(RESOURCE_PATH"textures\\grass.jpg", (IEngineBaseObject *&)pTexGrass, TEXTURE_LOAD_DEFAULT_2D);
	pResMan->Load(RESOURCE_PATH"textures\\stone.tga", (IEngineBaseObject *&)pTexStone, TEXTURE_LOAD_DEFAULT_2D);
	
	pResMan->Load(RESOURCE_PATH"meshes\\house\\tex_diffuse.jpg", (IEngineBaseObject *&)pTexHouse, TEXTURE_LOAD_DEFAULT_3D);
	pResMan->Load(RESOURCE_PATH"meshes\\house\\house.dmd", (IEngineBaseObject *&)pMeshHouse, MMLF_FORCE_MODEL_TO_MESH);

	pResMan->Load(RESOURCE_PATH"meshes\\trees\\tree_1.png", (IEngineBaseObject *&)pTexTree1, TEXTURE_LOAD_DEFAULT_2D);
	pResMan->Load(RESOURCE_PATH"meshes\\trees\\tree_1.dmd", (IEngineBaseObject *&)pMeshTree1, MMLF_FORCE_MODEL_TO_MESH);

	pResMan->Load(RESOURCE_PATH"meshes\\trees\\tree_2.png", (IEngineBaseObject *&)pTexTree2, TEXTURE_LOAD_DEFAULT_2D);
	pResMan->Load(RESOURCE_PATH"meshes\\trees\\tree_2.dmd", (IEngineBaseObject *&)pMeshTree2, MMLF_FORCE_MODEL_TO_MESH);

	pResMan->Load(RESOURCE_PATH"meshes\\trees\\tree_3.png", (IEngineBaseObject *&)pTexTree3, TEXTURE_LOAD_DEFAULT_2D);
	pResMan->Load(RESOURCE_PATH"meshes\\trees\\tree_3.dmd", (IEngineBaseObject *&)pMeshTree3, MMLF_FORCE_MODEL_TO_MESH);

	pResMan->Load(RESOURCE_PATH"meshes\\copter\\copter.png", (IEngineBaseObject *&)pTexCopter, TEXTURE_LOAD_DEFAULT_3D);
	pResMan->Load(RESOURCE_PATH"meshes\\copter\\copter.dmd", (IEngineBaseObject *&)pMeshCopter, MMLF_FORCE_MODEL_TO_MESH);

	pResMan->Load(RESOURCE_PATH"sprites\\zombie.png", (IEngineBaseObject *&)pTexZombie, TEXTURE_LOAD_DEFAULT_2D);
	pTexZombie->SetFrameSize(256, 256);

	pResMan->Load(RESOURCE_PATH"meshes\\copter\\propeller.png", (IEngineBaseObject *&)pTexRotor, TEXTURE_LOAD_DEFAULT_2D);

	// render shadows
	RenderMeshToTexture(pMeshTree1Shadow, pMeshTree1, pTexTree1);
	RenderMeshToTexture(pMeshTree2Shadow, pMeshTree2, pTexTree2);
	RenderMeshToTexture(pMeshTree3Shadow, pMeshTree3, pTexTree3);
	RenderMeshToTexture(pMeshHouseShadow, pMeshHouse, pTexHouse);
	RenderMeshToTexture(pMeshCopterShadow, pMeshCopter, pTexCopter);

	// render rotor shadow
	pResMan->CreateTexture(pRotorShadow, NULL, 256, 256, TDF_RGBA8, TCF_DEFAULT, TLF_FILTERING_BILINEAR);
	pRender->SetRenderTarget(pRotorShadow);
	pRender2D->DrawCircle(TPoint2(128, 128), 100, 64, ColorWhite(), PF_FILL);
	pRender->SetRenderTarget(NULL);
}

void DGLE_API Free(void *pParameter)
{
	pHelicopterSndChan->Unaquire();
}

void DGLE_API Update(void *pParameter)
{
	bool prsd;
	
	pInput->GetKey(KEY_ESCAPE, prsd);
	
	if (prsd)
	{
		pEngineCore->QuitEngine();
		return;
	}

	fCopterAngle += 0.5;
	stCopterPos.x += cosf(TO_RAD(fCopterAngle)) * 4.f;
	stCopterPos.y += sinf(TO_RAD(fCopterAngle)) * 6.f;

	// Choper flyes on screen between -60 and 856 coords so the way to conver its coords to [-100,100] range.
	pHelicopterSndChan->SetPan((int)((stCopterPos.x + 60.f - 460.f) * 0.2f));

	// process zombie movment and its sprite animation

	if (bFrameForward)
		fZombieAnimFrame += 0.3f;
	else
		fZombieAnimFrame -= 0.3f;

	if (bFrameForward && (int)fZombieAnimFrame == 15)
		bFrameForward = false;
	else
		if (!bFrameForward && (int)fZombieAnimFrame == 0)
			bFrameForward = true;

	TMouseStates mouse;
	pInput->GetMouseStates(mouse);
	stMouseOnScreen = TPoint2((float)mouse.iX, (float)mouse.iY);
	
	fZombieAngle = TO_DEG(atan2f(stMouseInCamera.y - (stZombiePos.y + 64.f), stMouseInCamera.x - (stZombiePos.x + 64.f)));

	stZombiePos.x += cosf(TO_RAD(fZombieAngle)) * 0.75f;
	stZombiePos.y += sinf(TO_RAD(fZombieAngle)) * 0.75f;

	++uiCounter;
}

void DGLE_API Render(void *pParameter)
{
	pRender2D->Begin2D();

	// make floating camera
	pRender2D->SetCamera(TPoint2(350.f + cosf(uiCounter / 200.f) * 200.f, 350.f + sinf(uiCounter / 200.f) * 80.f), sinf(uiCounter / 50.f) * 15.f);

	// conver our on-screen mouse coordinates to camera space
	pRender2D->ProjectScreenToCamera(stMouseOnScreen, stMouseInCamera);

	// draw background textures
	for(int i = -3; i < 5; ++i)
		for(int j = -3; j < 5; ++j)
			pTexGrass->Draw2DSimple(i * 256, j * 256);

	// draw stone layer with per vertex blending
	pRender2D->SetVerticesColors(TColor4(255, 255, 255, 0), ColorWhite(), ColorWhite(), TColor4(255, 255, 255, 0));
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f, 225.f), TVector2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLORS | EF_BLEND));

	pRender2D->SetVerticesColors(ColorWhite(), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0), ColorWhite());
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f + 256.f, 225.f), TVector2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLORS | EF_BLEND));
	
	pRender2D->SetVerticesColors(ColorWhite(), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0));
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f + 256.f, 225.f + 256.f), TVector2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLORS | EF_BLEND));

	pRender2D->SetVerticesColors(TColor4(255, 255, 255, 0), ColorWhite(), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0));
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f, 225.f + 256.f), TVector2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLORS | EF_BLEND));

	// set color mix to semi-transparent black for shadows
	pRender2D->SetColorMix(ColorBlack(128));

	// draw zombie, first draw shadow then zombie itself
	pRender2D->DrawTextureSprite(pTexZombie, stZombiePos + TPoint2(10.f, 10.f), TVector2(128, 128.f), (uint)fZombieAnimFrame, fZombieAngle + 90.f, (E_EFFECT2D_FLAGS)(EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTextureSprite(pTexZombie, stZombiePos, TVector2(128, 128.f), (uint)fZombieAnimFrame, fZombieAngle + 90.f);

	// render objects shadows
	pRender2D->DrawTexture(pMeshHouseShadow, TPoint2(180.f - 200.f, 150.f - 200.f), TVector2(450.f, 500.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTexture(pMeshHouseShadow, TPoint2(600.f - 200.f, 550.f - 200.f), TVector2(450.f, 500.f), 90.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));

	pRender2D->DrawTexture(pMeshTree1Shadow, TPoint2(900.f - 225.f, 500.f - 225.f), TVector2(450.f, 450.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTexture(pMeshTree1Shadow, TPoint2(-250.f - 225.f, 300.f - 225.f), TVector2(450.f, 450.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTexture(pMeshTree1Shadow, TPoint2(800.f - 225.f, 200.f - 225.f), TVector2(450.f, 450.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTexture(pMeshTree2Shadow, TPoint2(0.f - 175.f, 450.f - 175.f), TVector2(350.f, 350.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTexture(pMeshTree2Shadow, TPoint2(50.f - 175.f, 750.f - 175.f), TVector2(350.f, 350.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	pRender2D->DrawTexture(pMeshTree3Shadow, TPoint2(500.f - 225.f, 150.f - 225.f), TVector2(450.f, 450.f), 0.f, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));

	// draw 3D models in 2D space
	pRender2D->DrawMesh(pMeshHouse, pTexHouse, TPoint2(180.f, 150.f), TVector3(400.f, 400.f, 600.f));
	pRender2D->DrawMesh(pMeshHouse, pTexHouse, TPoint2(600.f, 550.f), TVector3(400.f, 400.f, 600.f), TVector3(0.f, 0.f, 1.f), 90.f);

	pRender2D->DrawMesh(pMeshTree1, pTexTree1, TPoint2(900.f, 500.f), TVector3(400.f, 400.f, 500.f));
	pRender2D->DrawMesh(pMeshTree1, pTexTree1, TPoint2(-250.f, 300.f), TVector3(400.f, 400.f, 500.f));
	pRender2D->DrawMesh(pMeshTree1, pTexTree1, TPoint2(800.f, 200.f), TVector3(400.f, 400.f, 400.f));
	pRender2D->DrawMesh(pMeshTree2, pTexTree2, TPoint2(0.f, 450.f), TVector3(300.f, 300.f, 400.f));
	pRender2D->DrawMesh(pMeshTree2, pTexTree2, TPoint2(50.f, 750.f), TVector3(300.f, 300.f, 300.f));
	pRender2D->DrawMesh(pMeshTree3, pTexTree3, TPoint2(500.f, 150.f), TVector3(400.f, 400.f, 500.f));

	// draw copter model and rotor sprite
	const TPoint2 vint_pos(stCopterPos.x - 200.f + cosf(TO_RAD(fCopterAngle)) * 80.f, stCopterPos.y - 200.f + sinf(TO_RAD(fCopterAngle)) * 80.f);
	pRender2D->DrawTexture(pMeshCopterShadow, stCopterPos - TPoint2(200.f, 200.f), TVector2(600.f, 600.f), fCopterAngle, (E_EFFECT2D_FLAGS)(EF_FLIP_VERTICALLY | EF_COLOR_MIX | EF_BLEND));
	
	// make blinky rotor shadow
	pRender2D->SetColorMix(ColorBlack(uiCounter % 3 == 2 ? 64 : 16));
	pRender2D->DrawTexture(pRotorShadow, vint_pos + TPoint2(100.f, 100.f), TVector2(400.f, 400.f), 0.f, (E_EFFECT2D_FLAGS)(EF_COLOR_MIX | EF_BLEND));

	pRender2D->DrawMesh(pMeshCopter, pTexCopter, stCopterPos, TPoint3(600.f, 600.f, 600.f), TVector3(0.f, 0.f, 1.f), fCopterAngle, EF_DEFAULT, false, 90.f, true);
	pRender2D->DrawTexture(pTexRotor, vint_pos, TVector2(400.f, 400.f), uiCounter * 25.f);

	pRender2D->End2D();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_4X))))
		{
			pEngineCore->AddProcedure(EPT_INIT,	&Init);
			pEngineCore->AddProcedure(EPT_FREE,	&Free);
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