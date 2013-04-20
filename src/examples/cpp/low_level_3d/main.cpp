#include <DGLE.h>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "Low Level 3D"

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
IRender *pRender = NULL;
IRender3D *pRender3D = NULL;

ITexture *pTexGrass, *pTexTree1, *pTexTree2, *pTexTree3, *pTexOwl, *pTexZard;

IMesh *pMeshTree1, *pMeshTree2, *pMeshTree3;

IModel *pModelZard;

IBitmapFont *pFont;

bool owlGoLeft = false;
float owlX = -2.f;

uint uiCounter = 0;

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)pRender);
	pRender->GetRender3D(pRender3D);
	pRender->SetClearColor(ColorGray());

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	const uint load_3d_flags = (uint)(TLF_FILTERING_ANISOTROPIC | TLF_ANISOTROPY_4X | TLF_GENERATE_MIPMAPS | TLF_COORDS_CLAMP);

	p_res_man->Load(RESOURCE_PATH"fonts\\Times_New_Roman_18_Bold.dft", (IEngineBaseObject *&)pFont);
	p_res_man->Load(RESOURCE_PATH"textures\\grass.jpg", (IEngineBaseObject *&)pTexGrass, load_3d_flags);

	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_owl.png", (IEngineBaseObject*&)pTexOwl, load_3d_flags);
	pTexOwl->SetFrameSize(48, 128);

	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_1.png", (IEngineBaseObject *&)pTexTree1, load_3d_flags);
	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_1.dmd", (IEngineBaseObject *&)pMeshTree1, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_2.png", (IEngineBaseObject *&)pTexTree2, load_3d_flags);
	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_2.dmd", (IEngineBaseObject *&)pMeshTree2, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_3.png", (IEngineBaseObject *&)pTexTree3, load_3d_flags);
	p_res_man->Load(RESOURCE_PATH"meshes\\trees\\tree_3.dmd", (IEngineBaseObject *&)pMeshTree3, MMLF_FORCE_MODEL_TO_MESH);

	p_res_man->Load(RESOURCE_PATH"meshes\\zard\\zard_diff.dds", (IEngineBaseObject *&)pTexZard, load_3d_flags);
	p_res_man->Load(RESOURCE_PATH"meshes\\zard\\zard_walk.dmd", (IEngineBaseObject *&)pModelZard);

	// add some fog to the scene
	pRender3D->SetFogColor(ColorGray());
	pRender3D->SetLinearFogBounds(1.25f, 4.f);
	pRender3D->ToggleFog(true);
}

void DGLE_API Update(void *pParameter)
{
	if (owlX > 2.5f)
		owlGoLeft = true;
	else
		if (owlX < -2.5f)
			owlGoLeft = false;

	owlX += (owlGoLeft ? -1.f : 1.f) * 0.0075f;

	++uiCounter;
}

void DGLE_API Render(void *pParameter)
{
	// Attention!
	// The main thing you should keep in the mind is that matrix multiplication order should be
	// reversed when passing matrix to pRender3D->SetMatrix or pRender3D->MultMatrix methods.

	// camera setup
	pRender3D->SetMatrix( // here and below matrix multiplication order is reversed
		MatrixRotate(sinf(uiCounter / 150.f) * 65.f, TVector3(0.f, 1.f, 0.f)) * // 4. add some periodic rotation left-right to 65 degrees
		MatrixRotate(30.f, TVector3(1.f, 0.f, 0.f)) * // 3. look down a little
		MatrixTranslate(TVector3(0.f, 0.f, -2.5f)) * // 2. move camera backwards
		MatrixIdentity() // 1. use identity matrix as zero point for all transformations
		);

	// If you want direct multiplication order you could rewrite code above like this way:
	/*
	// direct multiplication order here will make similar transformation as code above
	pRender3D->SetMatrix(MatrixIdentity()); // 1.
	pRender3D->MultMatrix(MatrixTranslate(TVector3(0.f, 0.f, -2.5f))); // 2.
	pRender3D->MultMatrix(MatrixRotate(30.f, TVector3(1.f, 0.f, 0.f))); // 3.
	pRender3D->MultMatrix(MatrixRotate(sinf(uiCounter / 150.f) * 65.f, TVector3(0.f, 1.f, 0.f))); // 4.
	*/

	// Draw entire scene.

	// draw tiled grass floor
	for (int i = -4; i < 4; ++i)
		for (int j = -4; j < 4; ++j)
		{
			pRender3D->PushMatrix(); // save current matrix
			
			pRender3D->MultMatrix( // multiplicates current matrix with given one
				MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)) *
				MatrixTranslate(TVector3((float)i, 0.f, (float)j)));
			
			pTexGrass->Draw3D(); // fast way to render texture as square plane with size 1.0

			pRender3D->PopMatrix(); // return previous matrix
		}

	// turn off backface culling because of trees leaves (they will look better) and sprites rendering (we want txt and owl to be visible from both sides)
	pRender3D->ToggleBackfaceCulling(false);

	// draw some trees

	// turn on alpha test for correct rendering of trees leaves
	pRender3D->ToggleAlphaTest(true);
	
	// precalculate similar for all trees transformation part to make it faster
	const TMatrix4 tree_rotate_and_scale = MatrixScale(TVector3(2.f, 2.f, 2.f)) * MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f));

	// some copy-pasted code for each tree to draw all of them

	pRender3D->PushMatrix();
	pRender3D->MultMatrix(tree_rotate_and_scale * MatrixTranslate(TVector3(2.3f, 0.f, 1.2f)));
	pTexTree1->Bind(); // way to set current texture
	pMeshTree1->Draw();
	pRender3D->PopMatrix();

	pRender3D->PushMatrix();
	pRender3D->MultMatrix(tree_rotate_and_scale * MatrixTranslate(TVector3(-2.15f, 0.f, -1.75f)));
	pTexTree1->Bind();
	pMeshTree1->Draw();
	pRender3D->PopMatrix();

	pRender3D->PushMatrix();
	pRender3D->MultMatrix(tree_rotate_and_scale * MatrixTranslate(TVector3(-0.5f, 0.f, -1.f)));
	pTexTree2->Bind();
	pMeshTree2->Draw();
	pRender3D->PopMatrix();

	pRender3D->PushMatrix();
	pRender3D->MultMatrix(tree_rotate_and_scale * MatrixTranslate(TVector3(0.75f, 0.f, 0.1f)));
	pTexTree2->Bind();
	pMeshTree2->Draw();
	pRender3D->PopMatrix();

	pRender3D->PushMatrix();
	pRender3D->MultMatrix(tree_rotate_and_scale * MatrixTranslate(TVector3(0.5f, 0.f, -1.5f)));
	pTexTree3->Bind();
	pMeshTree3->Draw();
	pRender3D->PopMatrix();

	pRender3D->PushMatrix();
	pRender3D->MultMatrix(tree_rotate_and_scale * MatrixTranslate(TVector3(-0.75f, 0.f, 0.25f)));
	pTexTree3->Bind();
	pMeshTree3->Draw();
	pRender3D->PopMatrix();

	pRender3D->ToggleAlphaTest(false); // we don't need alphatest anymore

	// we use blending for further font and sprite rendering
	pRender3D->ToggleBlending(true);

	// draw text
	
	pRender3D->PushMatrix();
	
	const char txt[] = "The very basics of the 3D graphics.";
	const float txt_scale = 0.005f;
	
	uint w, h;
	pFont->GetTextDimensions(txt, w, h);

	pRender3D->MultMatrix(
		MatrixScale(TVector3(txt_scale, txt_scale, txt_scale)) * // tex is drawing in rather huge coordinates, so we will downscale it
		MatrixTranslate(TVector3(0.f, h / 2.f * txt_scale, 1.25f)) // move text up on half of it's height and little forward
		);
	
	pRender3D->SetColor(ColorOfficialOrange()); // set current color

	pFont->Draw3D(txt);
	
	pRender3D->SetColor(ColorWhite()); // return color back to white

	pRender3D->PopMatrix();

	// draw owl animated sprite as billboard
	pRender3D->PushMatrix();
	
	// here is a way to make a billboard sprite in 3D
	TMatrix4 cur_matrix;
	pRender3D->GetMatrix(cur_matrix); // first we must get current matrix
	
	// now we set new current matrix
	pRender3D->SetMatrix(			
		MatrixBillboard( // this function will remove any rotation from given matrix
		MatrixScale(TVector3((owlGoLeft ? -1.f : 1.f) /* the way to mirror sprite picture */ * 0.35f, 0.35f, 0.35f)) *
		MatrixTranslate(TVector3(owlX, 1.f, -0.35f)) *
		cur_matrix)
		);
	
	pTexOwl->Draw3D((uiCounter / 2) % 15);
	
	pRender3D->PopMatrix();

	pRender3D->ToggleBlending(false);
	pRender3D->ToggleBackfaceCulling(true); // turn backface culling back on here

	// draw monster

	pRender3D->PushMatrix();
	
	TVector3 extents;
	pModelZard->GetExtents(extents); // retrieves dimensions of the model (ex. extents.x is a half of the models width)
	
	// here we will let monster go circles
	const float angle = uiCounter / 75.f, speed = 1.2f;
	const TVector3 circle_center(1.f, 0.f, 0.25f);
	
	pRender3D->MultMatrix(MatrixRotate(-90.f, TVector3(1.f, 0.f, 0.f)) *
		MatrixRotate(angle * 180.f / (float)M_PI, TVector3(0.f, -1.f, 0.f)) *
		MatrixTranslate(TVector3(cosf(angle) * speed, extents.z, sinf(angle) * speed) + circle_center));
	
	pTexZard->Bind();
	
	// this model uses frame based animation, we just need to switch between meshes
	uint meshes_count;
	pModelZard->MeshesCount(meshes_count);
	pModelZard->DrawMesh(uiCounter % meshes_count);
	
	pRender3D->PopMatrix();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_4X))))
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