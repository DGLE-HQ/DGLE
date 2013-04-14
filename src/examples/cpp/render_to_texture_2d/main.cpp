#include <DGLE.h>
#include <vector>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "Render To Texture 2D"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

#define SCREEN_WIDTH	1024u
#define SCREEN_HEIGHT	768u

const TVector2 screenSize = TVector2((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);

#define TO_RAD (float)M_PI / 180.f

IEngineCore *pEngineCore = NULL;
IInput *pInput = NULL;
IRender *pRender = NULL;
IRender2D *pRender2D = NULL;

ITexture *pTexLight = NULL, *pTexBg = NULL, *pTexTarget[3],
	*pTexBug = NULL, *pTexBugCorpse = NULL, *pTexSmoke = NULL,
	*pTexBlood = NULL;

ISoundSample *pSndDeath = NULL;

uint uiCounter = 0;

TPoint2 mousePos;

struct TBug
{
	TPoint2 pos;
	float angle;
	int lifes;

	TBug(const TPoint2 &stPos) : pos(stPos), lifes(100)
	{
		angle = (float)(rand() % 360);
	}
};

std::vector<TBug> bugs;

void Clear();

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_INPUT, (IEngineSubSystem *&)pInput);
	pInput->Configure(ICF_HIDE_CURSOR);

	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)pRender);
	pRender->GetRender2D(pRender2D);

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	p_res_man->Load(RESOURCE_PATH"sounds\\bug_death.wav", (IEngineBaseObject*&)pSndDeath);

	p_res_man->Load(RESOURCE_PATH"sprites\\light.jpg", (IEngineBaseObject*&)pTexLight, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\smoke.png", (IEngineBaseObject*&)pTexSmoke, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\bug.png", (IEngineBaseObject*&)pTexBug, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\bug_corpse.png", (IEngineBaseObject*&)pTexBugCorpse, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\blood.png", (IEngineBaseObject*&)pTexBlood, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"textures\\cartoon_grass.tga", (IEngineBaseObject*&)pTexBg, (uint)(TLF_FILTERING_BILINEAR | TLF_COORDS_REPEAT));
	
	pTexBug->SetFrameSize(52, 42);

	for (uint i = 0; i < 3; ++i)
		p_res_man->CreateTexture(pTexTarget[i], NULL, SCREEN_WIDTH, SCREEN_HEIGHT, TDF_RGB8, TCF_DEFAULT, (E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_BILINEAR | TLF_COORDS_CLAMP));

	Clear();
}

void Clear()
{
	bugs.clear();

	for (size_t i = 0; i < 100; ++i)
		bugs.push_back(TBug(TPoint2((float)(rand() % SCREEN_WIDTH), (float)(rand() % SCREEN_HEIGHT))));

	// render background image to texture the first time
	for (uint i = 0; i < 2; ++i)
	{
		pRender->SetRenderTarget(pTexTarget[i]);
		pRender2D->DrawTexture(pTexBg, TPoint2(), screenSize, 0.f, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_TILE_TEXTURE));
	}

	pRender->SetRenderTarget(NULL);
}

void DGLE_API Update(void *pParameter)
{
	// render to existing texture another old one and some new corpses
	// we must switch between two textures
	pRender->SetRenderTarget(pTexTarget[uiCounter % 2]);
	
	pRender2D->DrawTexture(pTexTarget[!(uiCounter % 2)], TPoint2(), screenSize, 0.f, EF_FLIP_VERTICALLY);
	
	std::vector<TBug>::iterator iter = bugs.begin();

	while (iter != bugs.end())
		if (iter->lifes < 0)
		{
			pSndDeath->Play();
			
			for (int i = 0; i < 5; ++i)
				pRender2D->DrawTextureCropped(pTexBlood, iter->pos - TPoint2(20.f, 20.f) + TPoint2((float)(rand() % 50), (float)(rand() % 50)),
					TVector2(35.f, 35.f), TRectF(100.f, 100.f, 100.f, 100.f), (float)(rand() % 360), EF_BLEND);

			pRender2D->DrawTexture(pTexBugCorpse, iter->pos + TPoint2(5.f, 0.f), TVector2(42.f, 42.f), 90.f + iter->angle);
			
			iter = bugs.erase(iter);
		}
		else
		{
			 // render bug trail
			if (uiCounter % 20 == 0)
			{
				pRender2D->SetColorMix(ColorBrown(75));
				pRender2D->DrawTexture(pTexSmoke, iter->pos + TPoint2(5.f, 0.f), TVector2(42.f, 42.f), 90.f + iter->angle, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_COLOR_MIX));
			}
			
			++iter;
		}

	// render scene texture wit bugs
	pRender->SetRenderTarget(pTexTarget[2]);
	
	pRender2D->DrawTexture(pTexTarget[uiCounter % 2], TPoint2(), screenSize, 0.f, EF_FLIP_VERTICALLY);

	for (size_t i = 0; i < bugs.size(); ++i)
	{
		TBug &bug = bugs[i];

		TPoint2 old = bug.pos;
		
		bug.pos.x += cosf(bug.angle * TO_RAD) * 0.5f;
		bug.pos.y += sinf(bug.angle * TO_RAD) * 0.5f;

		if (bug.pos.x > SCREEN_WIDTH || bug.pos.y > SCREEN_HEIGHT || bug.pos.x < 0.f || bug.pos.y < 0.f)
		{
			bug.pos = old;
			bug.angle = (float)(rand() % 360);
		}

		if (mousePos.DistTo(bug.pos) < 50.f)
			bug.lifes -= 2;

		pRender2D->SetColorMix(TColor4(255, (uint8)(bug.lifes * 2.5), (uint8)(bug.lifes * 2.5), 255));

		pRender2D->DrawTextureSprite(pTexBug, bug.pos, TVector2(52.f, 42.f), ((uint)bug.angle + uiCounter / 4) % 8, 90.f + bug.angle, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_COLOR_MIX));
	}

	pRender->SetRenderTarget(NULL);

	// input

	TMouseStates ms;
	pInput->GetMouseStates(ms);
	mousePos = TPoint2((float)ms.iX, (float)ms.iY);

	bool prsd;
	
	pInput->GetKey(KEY_ESCAPE, prsd);
	
	if (prsd)
	{
		pEngineCore->QuitEngine();
		return;
	}

	pInput->GetKey(KEY_SPACE, prsd);
	
	if (prsd)
	{
		Clear();
		uiCounter = 0;
		return;
	}

	++uiCounter;
}

void DGLE_API Render(void *pParameter)
{
	// Apply some lighting to the scene and draw prerendered texture.
	
	pRender2D->SetBlendMode(BE_NORMAL);
	pRender2D->DrawTexture(pTexLight, mousePos - TPoint2(200.f, 200.f), TVector2(400.f, 400.f), 0.f, EF_BLEND);
	
	pRender2D->SetBlendMode(BE_MASK);
	pRender2D->DrawTexture(pTexTarget[2], TPoint2(), screenSize, 0.f, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_FLIP_VERTICALLY));
	
	pRender2D->SetBlendMode(BE_NORMAL);
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