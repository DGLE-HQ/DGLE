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
IRender2D *pRender2D = NULL;
IRender3D *pRender3D = NULL;
IInput *pInput = NULL;

TPoint2 stMouseOnScreen, stMouseInCamera;

ITexture *pTexGrass, *pTexStone, *pTexTree1, *pTexTree2, *pTexTree3, *pTexHouse, *pTexCopter, *pTexZombie, *pTexVint;
IMesh *pMeshTree1, *pMeshTree2, *pMeshTree3, *pMeshHouse, *pMeshCopter;
ISoundSample *pHelicopter;
ISoundChannel *pHelicopterSndChan;

TPoint2 stCopterPos = TPoint2(400, 200);
float fCopterAngle = 0.f;

TPoint2 stZombiePos = TPoint2(10, 200);
float fZombieAngle = 0.f, fZombieAnimFrame = 0.f;
bool bFrameForward = true;

#define TO_RAD(x) (x * (float)M_PI / 180.f)
#define TO_DEG(x) (x * 180.f / (float)M_PI)

uint uiCounter = 1;

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_INPUT,(IEngineSubSystem *&)pInput);

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER,(IEngineSubSystem *&)p_res_man);
	
	IRender *p_render;
	pEngineCore->GetSubSystem(ESS_RENDER,(IEngineSubSystem *&)p_render);
	p_render->GetRender2D(pRender2D);
	p_render->GetRender3D(pRender3D);

	p_res_man->Load(RESOURCE_PATH"sounds\\helicopter.wav", (IEngineBaseObject *&)pHelicopter);
	pHelicopter->PlayEx(pHelicopterSndChan, SSP_LOOPED);

	p_res_man->Load(RESOURCE_PATH"textures\\grass.jpg", (IEngineBaseObject *&)pTexGrass);
	p_res_man->Load(RESOURCE_PATH"textures\\stone.tga", (IEngineBaseObject *&)pTexStone);
	
	p_res_man->Load(RESOURCE_PATH"meshes\\house\\tex_diffuse.jpg", (IEngineBaseObject *&)pTexHouse, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"meshes\\house\\house.dmd", (IEngineBaseObject *&)pMeshHouse, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_1.png", (IEngineBaseObject *&)pTexTree1, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_1.dmd", (IEngineBaseObject *&)pMeshTree1, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_2.png", (IEngineBaseObject *&)pTexTree2, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_2.dmd", (IEngineBaseObject *&)pMeshTree2, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_3.png", (IEngineBaseObject *&)pTexTree3, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_3.dmd", (IEngineBaseObject *&)pMeshTree3, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\copter\\copter.png", (IEngineBaseObject *&)pTexCopter, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"meshes\\copter\\copter.dmd", (IEngineBaseObject *&)pMeshCopter, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"sprites\\zombie.png", (IEngineBaseObject *&)pTexZombie, TEXTURE_LOAD_DEFAULT_2D);
	pTexZombie->SetFrameSize(256, 256);

	p_res_man->Load(RESOURCE_PATH"meshes\\copter\\propeller.png", (IEngineBaseObject *&)pTexVint, TEXTURE_LOAD_DEFAULT_2D);
}

void DGLE_API Free(void *pParameter)
{
	pHelicopterSndChan->Unaquire();
}

void DGLE_API Update(void *pParameter)
{
	fCopterAngle += 0.5;
	stCopterPos.x += cosf(TO_RAD(fCopterAngle)) * 4.f;
	stCopterPos.y += sinf(TO_RAD(fCopterAngle)) * 6.f;

	// Choper flyes on screen between -60 and 856 coords so the way to conver its coords to [-100,100] range.
	pHelicopterSndChan->SetPan((int)((stCopterPos.x + 60.f - 460.f) * 0.2f));

	// Process zombie.

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
	stMouseOnScreen = TPoint2(mouse.iX, mouse.iY);
	
	fZombieAngle = TO_DEG(atan2f(stMouseInCamera.y - (stZombiePos.y + 64.f), stMouseInCamera.x - (stZombiePos.x + 64.f)));

	stZombiePos.x += cosf(TO_RAD(fZombieAngle)) * 0.75f;
	stZombiePos.y += sinf(TO_RAD(fZombieAngle)) * 0.75f;

	++uiCounter;
}

void DGLE_API Render(void *pParameter)
{
	pRender2D->Begin2D();

	// Make floating camera.
	pRender2D->SetCamera(TPoint2(350.f + cosf(uiCounter / 200.f) * 200.f, 350.f + sinf(uiCounter / 200.f) * 80.f), sinf(uiCounter / 50.f) * 15.f);

	// Conver our on-screen mouse coordinates to camera space.
	pRender2D->ProjectScreenToCamera(stMouseOnScreen, stMouseInCamera);

	// Draw background.
	for(int i = -3; i < 5; ++i)
		for(int j = -3; j < 5; ++j)
			pTexGrass->Draw2DSimple(i * 256, j * 256);

	// Draw stone layer with per vertex blending.
	pRender2D->SetVerticesColors(TColor4(255, 255, 255, 0), ColorWhite(), ColorWhite(), TColor4(255, 255, 255, 0));
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f, 225.f), TPoint2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLOR | EF_BLEND));

	pRender2D->SetVerticesColors(ColorWhite(), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0), ColorWhite());
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f + 256.f, 225.f), TPoint2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLOR | EF_BLEND));
	
	pRender2D->SetVerticesColors(ColorWhite(), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0));
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f + 256.f, 225.f + 256.f), TPoint2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLOR | EF_BLEND));

	pRender2D->SetVerticesColors(TColor4(255, 255, 255, 0), ColorWhite(), TColor4(255, 255, 255, 0), TColor4(255, 255, 255, 0));
	pRender2D->DrawTexture(pTexStone, TPoint2(0.f, 225.f + 256.f), TPoint2(256.f, 256.f), 0.f, (E_EFFECT2D_FLAGS)(EF_VERTICES_COLOR | EF_BLEND));

	// Draw zombie. First draw shadow then zombie itself.
	pRender2D->SetColorMix(TColor4(0, 0, 0, 128));
	pRender2D->DrawTextureSprite(pTexZombie, stZombiePos + TPoint2(10.f, 10.f), TPoint2(128, 128.f), (uint)fZombieAnimFrame, fZombieAngle + 90.f, (E_EFFECT2D_FLAGS)(EF_COLORMIX | EF_BLEND));
	
	pRender2D->DrawTextureSprite(pTexZombie, stZombiePos, TPoint2(128, 128.f), (uint)fZombieAnimFrame, fZombieAngle + 90.f);

	// Draw 3D models.
	pRender2D->DrawMesh(pMeshHouse, pTexHouse, TPoint2(180.f, 150.f), TVector3(400.f, 400.f, 600.f));
	pRender2D->DrawMesh(pMeshHouse, pTexHouse, TPoint2(600.f, 550.f), TVector3(400.f, 400.f, 600.f), TPoint3(0.f, 0.f, 1.f), 90.f);

	pRender2D->DrawMesh(pMeshTree1, pTexTree1, TPoint2(900.f, 500.f), TVector3(400.f, 400.f, 500.f));
	pRender2D->DrawMesh(pMeshTree1, pTexTree1, TPoint2(-250.f, 300.f), TVector3(400.f, 400.f, 500.f));
	pRender2D->DrawMesh(pMeshTree2, pTexTree2, TPoint2(0.f, 450.f), TVector3(300.f, 300.f, 400.f));
	pRender2D->DrawMesh(pMeshTree1, pTexTree2, TPoint2(500.f, 150.f), TVector3(400.f, 400.f, 500.f));
	pRender2D->DrawMesh(pMeshTree3, pTexTree3, TPoint2(50.f, 750.f), TVector3(300.f, 300.f, 300.f));
	pRender2D->DrawMesh(pMeshTree3, pTexTree3, TPoint2(800.f, 200.f), TVector3(400.f, 400.f, 500.f));

	// Need to clear depth buffer to draw copter model over houses and trees.
	pRender3D->ClearDepthBuffer();

	// Draw copter model and rotor sprite.
	pRender2D->DrawMesh(pMeshCopter, pTexCopter, stCopterPos, TPoint3(600.f, 600.f, 600.f), TPoint3(0.f, 0.f, 1.f), fCopterAngle, false);
	pRender2D->DrawTexture(pTexVint, TPoint2(stCopterPos.x - 200.f + cosf(TO_RAD(fCopterAngle)) * 80.f, stCopterPos.y - 200.f + sinf(TO_RAD(fCopterAngle)) * 80.f), TPoint2(400.f, 400.f), uiCounter * 25.f);

	pRender2D->End2D();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false))))
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