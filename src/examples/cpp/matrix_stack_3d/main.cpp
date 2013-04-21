#include <DGLE.h>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "Matrix Stack 3D"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

#define SCREEN_WIDTH	1280u
#define SCREEN_HEIGHT	800u

IEngineCore *pEngineCore = NULL;
IRender *pRender = NULL;
IRender3D *pRender3D = NULL;

ITexture *pTexFloor, *pTexLight;
IModel *pMdlDesk, *pMdlLamp, *pMdlChair, *pMdlMusicBox,
	*pSnowGlobe, *pModelChurch;

ILight *pLightDirect, *pLightSpot;

TMatrix4x4Stack transform;

uint uiCounter = 1500;

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)pRender);
	pRender->GetRender3D(pRender3D);

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	p_res_man->Load(RESOURCE_PATH"textures\\floor.dds", (IEngineBaseObject *&)pTexFloor, TEXTURE_LOAD_DEFAULT_3D);
	p_res_man->Load(RESOURCE_PATH"sprites\\light.jpg", (IEngineBaseObject *&)pTexLight, TEXTURE_LOAD_DEFAULT_2D);

	// some global lighting

	pRender3D->ToggleLighting(true);
	pRender3D->SetGlobalAmbientLighting(ColorBlack()); // turn off ambient lighting

	// setup lights

	// use single directional light to simulate ambient lighting
	p_res_man->CreateLight(pLightDirect);
	pLightDirect->SetType(LT_DIRECTIONAL);
	pLightDirect->SetColor(ColorGray()); // dim light
	pLightDirect->SetDirection(TVector3(-0.5f, 0.5f, 0.75f));
	pLightDirect->SetEnabled(true);
	
	// Position is ignored for direction lights but is used by engine for debug drawing.
	// Use "rnd3d_draw_lights 1" console command to debug lights.
	pLightDirect->SetPosition(TPoint3(0.f, 7.5f, 0.f));

	// create light for the table-lamp
	p_res_man->CreateLight(pLightSpot);
	pLightSpot->SetType(LT_SPOT);
	pLightSpot->SetColor(ColorYellow());
	pLightSpot->SetSpotAngle(100.f);
	pLightSpot->SetDirection(TVector3(0.15f, 0.f, -1.f));
	pLightSpot->SetEnabled(true);

	// create and setup materials and load models

	ITexture *p_tex;
	IMaterial *p_mat;

	// desk
	p_res_man->CreateMaterial(p_mat);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\desk\\desk_diff.jpg", (IEngineBaseObject *&)p_tex, TEXTURE_LOAD_DEFAULT_3D);
	p_mat->SetDiffuseTexture(p_tex);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\desk\\desk.dmd", (IEngineBaseObject *&)pMdlDesk);
	pMdlDesk->SetModelMaterial(p_mat);

	// table-lamp
	p_res_man->CreateMaterial(p_mat);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\table_lamp\\lamp_diff.jpg", (IEngineBaseObject *&)p_tex, TEXTURE_LOAD_DEFAULT_3D);
	p_mat->SetDiffuseTexture(p_tex);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\table_lamp\\lamp.dmd", (IEngineBaseObject *&)pMdlLamp);
	pMdlLamp->SetModelMaterial(p_mat);

	// chair
	p_res_man->CreateMaterial(p_mat);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\chair\\chair_diff.jpg", (IEngineBaseObject *&)p_tex, TEXTURE_LOAD_DEFAULT_3D);
	p_mat->SetDiffuseTexture(p_tex);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\chair\\chair.dmd", (IEngineBaseObject *&)pMdlChair);
	pMdlChair->SetModelMaterial(p_mat);

	// music box
	p_res_man->CreateMaterial(p_mat);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\music_box\\mbox_d.dds", (IEngineBaseObject *&)p_tex, TEXTURE_LOAD_DEFAULT_3D);
	p_mat->SetDiffuseTexture(p_tex);
	p_res_man->Load(RESOURCE_PATH"meshes\\furniture\\music_box\\music_box.dmd", (IEngineBaseObject *&)pMdlMusicBox);
	pMdlMusicBox->SetModelMaterial(p_mat);

	// church
	p_res_man->Load(RESOURCE_PATH"meshes\\church\\church.dmd", (IEngineBaseObject *&)pModelChurch);
	pModelChurch->SetModelMaterial(p_mat);

	p_res_man->CreateMaterial(p_mat);
	p_mat->SetDiffuseColor(ColorSilver());
	p_mat->SetSpecularColor(ColorWhite());
	p_mat->SetShininess(25.f);
	pModelChurch->SetMeshMaterial(0, p_mat);

	p_res_man->CreateMaterial(p_mat);
	p_res_man->Load(RESOURCE_PATH"meshes\\church\\church_roof.jpg", (IEngineBaseObject *&)p_tex, TEXTURE_LOAD_DEFAULT_3D);
	p_mat->SetDiffuseTexture(p_tex);
	pModelChurch->SetMeshMaterial(1, p_mat);

	p_res_man->CreateMaterial(p_mat);
	p_res_man->Load(RESOURCE_PATH"meshes\\church\\church_main.jpg", (IEngineBaseObject *&)p_tex, TEXTURE_LOAD_DEFAULT_3D);
	p_mat->SetDiffuseTexture(p_tex);
	pModelChurch->SetMeshMaterial(2, p_mat);

	// snow globe
	p_res_man->Load(RESOURCE_PATH"meshes\\snow_globe.dmd", (IEngineBaseObject *&)pSnowGlobe);
	
	p_res_man->CreateMaterial(p_mat);
	p_mat->SetDiffuseColor(ColorWhite());
	pSnowGlobe->SetMeshMaterial(0, p_mat);

	p_res_man->CreateMaterial(p_mat);
	p_mat->SetDiffuseColor(ColorBrown());
	p_mat->SetSpecularColor(ColorWhite());
	p_mat->SetShininess(25.f);
	pSnowGlobe->SetMeshMaterial(2, p_mat);

	p_res_man->CreateMaterial(p_mat);
	p_mat->SetDiffuseColor(ColorWhite(100));
	p_mat->SetSpecularColor(ColorWhite());
	p_mat->SetShininess(50.f);
	p_mat->SetBlending(true, BE_NORMAL);
	
	// When material with blending is set model will sort mesh order for meshes with blending to be the last.
	pSnowGlobe->SetMeshMaterial(1, p_mat);

	// We will use black fog to simulate darkness.
	pRender3D->SetFogColor(ColorBlack());
	pRender3D->SetLinearFogBounds(12.5f, 20.f);
	pRender3D->ToggleFog(true);
}

void DGLE_API Update(void *pParameter)
{	
	if (uiCounter % 25 == 0)
		pLightSpot->SetEnabled(rand() % 100 < 75);

	++uiCounter;
}

void DGLE_API Render(void *pParameter)
{
	// camera setup
	transform.Clear(MatrixIdentity());
	transform.MultLocal(MatrixTranslate(TVector3(0.f, -4.5f, -4.f -fabs(sinf(uiCounter / 250.f)) * 5.f)));
	transform.MultLocal(MatrixRotate(15.f + cosf(uiCounter / 200.f) * 10.f, TVector3(1.f, 0.f, 0.f)));
	transform.MultLocal(MatrixRotate(uiCounter / 5.f, TVector3(0.f, 1.f, 0.f)));
	
	pRender3D->SetMatrix(transform.Top());

	// set light in the world coordinate system
	pLightDirect->Update();

	// draw floor

	pRender->Unbind(EOT_MATERIAL); // unbind last material from previous frame
	
	for (int i = -5; i < 5; ++i)
		for (int j = -5; j < 5; ++j)
		{
			transform.Push();		
			
			transform.MultLocal(MatrixTranslate(TVector3(i * 5.f, 0.f, j * 5.f)));
			transform.MultLocal(MatrixScale(TVector3(5.f, 5.f, 5.f)));
			transform.MultLocal(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)));
			
			pRender3D->SetMatrix(transform.Top());
			pTexFloor->Draw3D();
			
			transform.Pop();
		}
	
	// draw desk
	
	TVector3 desk_extents, extents;
	pMdlDesk->GetExtents(desk_extents);
	TPoint3 desk_pos = TVector3(0.f, desk_extents.z * 10.f, 0.f);

	transform.Push();
	transform.MultLocal(MatrixTranslate(desk_pos));
	transform.MultLocal(MatrixScale(TVector3(10.f, 10.f, 10.f)));
	transform.MultLocal(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)));
	transform.MultLocal(MatrixRotate(180.f, TVector3(0.f, 0.f, 1.f)));
	pRender3D->SetMatrix(transform.Top());
	pMdlDesk->Draw();
	transform.Pop();

	// draw chair
	pMdlChair->GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(TVector3(-1.5f, extents.z * 5.f, 4.5f)));
	transform.MultLocal(MatrixScale(TVector3(5.f, 5.f, 5.f)));
	transform.MultLocal(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)));
	transform.MultLocal(MatrixRotate(65.f, TVector3(0.f, 0.f, 1.f)));
	pRender3D->SetMatrix(transform.Top());
	pMdlChair->Draw();
	transform.Pop();

	// draw music box
	pMdlMusicBox->GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(desk_pos + TVector3(-2.75f, extents.y * 6.f, -1.f)));
	transform.MultLocal(MatrixScale(TVector3(3.f, 3.f, 3.f)));
	transform.MultLocal(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)));
	transform.MultLocal(MatrixRotate(25.f, TVector3(0.f, 0.f, 1.f)));
	pRender3D->SetMatrix(transform.Top());
	pMdlMusicBox->Draw();
	transform.Pop();

	// draw table-lamp
	pMdlLamp->GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(desk_pos + TVector3(3.75f, extents.z * 8.f - 0.2f, -1.f)));
	transform.MultLocal(MatrixScale(TVector3(4.f, 4.f, 4.f)));
	transform.MultLocal(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)));
	transform.MultLocal(MatrixRotate(-150.f, TVector3(0.f, 0.f, 1.f)));
	pRender3D->SetMatrix(transform.Top());
	pMdlLamp->Draw();
	
	// here we set light position in the coordinate space of the lamp (object space)
	TPoint3 light_pos = TPoint3(0.35f, 0.f, extents.z - 0.15f);
	pLightSpot->SetPosition(light_pos);
	pLightSpot->Update();
	
	// draw light halo
	bool is_light_active;
	pLightSpot->GetEnabled(is_light_active);

	if (is_light_active)
	{
		pRender3D->ToggleLighting(false);
		pRender3D->SetBlendMode(BE_ADD);
		pRender3D->ToggleBlending(true);
		pRender3D->SetColor(ColorYellow(200));
		transform.MultLocal(MatrixTranslate(light_pos + TPoint3(0.f, 0.f, -0.1f)));
		transform.MultLocal(MatrixScale(TVector3(0.25f, 0.25f, 0.25f)));
		pRender3D->SetMatrix(MatrixBillboard(transform.Top()));
		pTexLight->Draw3D();
		pRender3D->ToggleLighting(true);
	}

	transform.Pop();

	// draw snow globe and church
	pSnowGlobe->GetExtents(extents);
	transform.Push();
	transform.MultLocal(MatrixTranslate(desk_pos + TVector3(1.f, extents.z * 4.f + 0.75f, 0.5f)));
	transform.MultLocal(MatrixScale(TVector3(2.f, 2.f, 2.f)));
	transform.MultLocal(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)));
	
	transform.Push();
	transform.MultLocal(MatrixTranslate(TVector3(0.f, 0.f, 0.125f)));
	transform.MultLocal(MatrixScale(TVector3(0.7f, 0.7f, 0.7f)));
	pRender3D->SetMatrix(transform.Top());
	pModelChurch->Draw();
	transform.Pop();
	
	pRender3D->SetMatrix(transform.Top());
	pSnowGlobe->Draw();
	
	transform.Pop();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_4X))))
		{
			//pEngineCore->ConsoleExecute("rnd3d_draw_lights 1");

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