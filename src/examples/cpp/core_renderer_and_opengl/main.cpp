#include <DGLE.h>

#define OPENGL_LEGACY_BASE_OBJECTS // we will use legacy OpenGL
#include <DGLE_CoreRenderer.h>

#pragma comment(linker, "/defaultlib:opengl32.lib")
#pragma comment(linker, "/defaultlib:glu32.lib")
#pragma message("Linking with \"opengl32.lib\" and \"glu32.lib\".")

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

#define SCREEN_WIDTH	800u
#define SCREEN_HEIGHT	600u

IEngineCore *pEngineCore = NULL;
ICoreRenderer *pCoreRenderer = NULL;

IMesh *pMesh;
ITexture *pTex;

uint uiCounter = 0;

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_CORE_RENDERER, (IEngineSubSystem *&)pCoreRenderer);

	// We must be shure that we are working with legacy OpenGL implementation!
	E_CORE_RENDERER_TYPE type;
	pCoreRenderer->GetRendererType(type);
	if (type != CRT_OPENGL_LEGACY)
		pEngineCore->WriteToLogEx("This example will work only with Legacy OpenGL renderer!", LT_FATAL, __FILE__, __LINE__);

	// Consider of turning off state manager, because our direct OpenGL calls could interrupt it in some cases.
	// But it is highly not recommended!
	//pCoreRenderer->ToggleStateFilter(false);

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
	p_res_man->Load(RESOURCE_PATH"meshes\\torus.dmd", (IEngineBaseObject *&)pMesh, MMLF_FORCE_MODEL_TO_MESH);
	p_res_man->Load(RESOURCE_PATH"textures\\stone.tga", (IEngineBaseObject *&)pTex, TEXTURE_LOAD_DEFAULT_3D);
}

void DGLE_API Update(void *pParameter)
{
	++uiCounter;
}

void DGLE_API Render(void *pParameter)
{
	// retrieving OpenGL texture
	ICoreTexture *p_core_tex;
	pTex->GetCoreTexture(p_core_tex);
	IOpenGLTextureContainer *p_gl_tex;
	p_core_tex->GetBaseObject((IBaseRenderObjectContainer *&)p_gl_tex);

	GLuint gl_tex; // OpenGL texture Id
	p_gl_tex->GetTexture(gl_tex);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glBindTexture(GL_TEXTURE_2D, gl_tex);

	glPushMatrix();

	glTranslatef(0.f, 0.f, -1.5f);
	glRotatef(uiCounter / 2.f, 0.6f, 0.2f, 0.3f);

	// We can also acess geometry buffer via pMesh->GetGeometryBuffer(p_geom_buff) and then p_geom_buff->GetBaseObject(...).
	// But it is easier to use engine.
	pMesh->Draw();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	// If you've corrupted engine pipeline somehow, you may want to invalidate state manager.
	// And remember that IRender3D can also cache some states.
	//pCoreRenderer->InvalidateStateFilter();
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