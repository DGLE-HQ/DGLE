#include <DGLE.h>
#include <vector>
#include <string>

using namespace DGLE;

DGLE_DYNAMIC_FUNC

#define APP_CAPTION "Basic 2D"

#ifdef _DEBUG
#	define DLL_PATH "..\\..\\..\\..\\bin\\windows\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define DLL_PATH "..\\..\\DGLE.dll"
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

#define SCREEN_WIDTH	1024u
#define SCREEN_HEIGHT	768u

#define TO_RAD (float)M_PI / 180.f

IEngineCore *pEngineCore = NULL;
IRender *pRender = NULL;
IRender2D *pRender2D = NULL;

TColor4 stRandomCol;
IBitmapFont *pFont, *pFontBold, *pFontHard;
ITexture *pTexGrass, *pTexGirl, *pTexTankBody, *pTexTankTurret,
	*pTexLight, *pTexPlanet, *pTexMask, *pTexBg, *pTexPlanetRenderIn,
	*pTexLightRound, *pTexAnimWater[11], *pTexJellyFish;

std::vector<TVertex2> waterWaves;

uint uiCounter = 0;

void DGLE_API Init(void *pParameter)
{
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)pRender);
	pRender->SetClearColor(ColorOfficialBlack());
	pRender->GetRender2D(pRender2D);

	IResourceManager *p_res_man;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	// for fonts part

	p_res_man->Load(RESOURCE_PATH"fonts\\Times_New_Roman_12_rus.dft", (IEngineBaseObject *&)pFont);
	p_res_man->Load(RESOURCE_PATH"fonts\\Times_New_Roman_18_Bold.dft", (IEngineBaseObject *&)pFontBold);
	p_res_man->Load(RESOURCE_PATH"fonts\\TheHard_18.dft", (IEngineBaseObject *&)pFontHard);
	
	// for sprites part

	const uint unfiltered_2d_flag = (uint)(TLF_FILTERING_NONE /* for cool old-school pixelized image */ | TLF_COORDS_CLAMP);
	p_res_man->Load(RESOURCE_PATH"textures\\cartoon_grass.tga", (IEngineBaseObject *&)pTexGrass,
		unfiltered_2d_flag | TLF_DECREASE_QUALITY_MEDIUM /* decrease texture size to make it more pixelized */);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_tank_body.png", (IEngineBaseObject *&)pTexTankBody, unfiltered_2d_flag);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_tank_turret.png", (IEngineBaseObject *&)pTexTankTurret, unfiltered_2d_flag);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_anime_girl.png", (IEngineBaseObject*&)pTexGirl, unfiltered_2d_flag);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_mistery_light.jpg", (IEngineBaseObject*&)pTexLight, unfiltered_2d_flag | TLF_DECREASE_QUALITY_HIGH);

	pTexGirl->SetFrameSize(55, 117); // set animation frame size
	pTexLight->SetFrameSize(64, 128);

	// for advanced part

	p_res_man->Load(RESOURCE_PATH"meshes\\planets\\asteroid.jpg", (IEngineBaseObject*&)pTexPlanet, (E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_BILINEAR | TLF_COORDS_REPEAT));
	p_res_man->Load(RESOURCE_PATH"sprites\\light.jpg", (IEngineBaseObject*&)pTexLightRound, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\sphere_mask.png", (IEngineBaseObject*&)pTexMask, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\cartoon_cloudy_night_sky.jpg", (IEngineBaseObject*&)pTexBg, TEXTURE_LOAD_DEFAULT_2D);
	p_res_man->Load(RESOURCE_PATH"sprites\\jellyfish.png", (IEngineBaseObject*&)pTexJellyFish, TEXTURE_LOAD_DEFAULT_2D);

	for (int i = 0; i < 11; ++i)
		p_res_man->Load((std::string(RESOURCE_PATH"textures\\water\\water_") + std::to_string((uint64)i) + ".tga").c_str(), (IEngineBaseObject*&)pTexAnimWater[i],
			(E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_BILINEAR | TLF_COORDS_MIRROR_REPEAT /* let's repeat the texture this way to make it little weird */));

	p_res_man->CreateTexture(pTexPlanetRenderIn, NULL, 256, 256, TDF_RGBA8, TCF_DEFAULT, (E_TEXTURE_LOAD_FLAGS)(TLF_FILTERING_BILINEAR | TLF_COORDS_CLAMP)); 
}

void RenderPlanetInToTexture()
{
	pRender->SetClearColor(ColorWhite(0)); // set clear color here because SetRenderTarget will clear color buffer for us
	
	pRender->SetRenderTarget(pTexPlanetRenderIn);
	
	pRender2D->Begin2D();

	pRender2D->DrawTexture(pTexMask, TPoint2(), TVector2(256.f, 256.f), 0.f, EF_BLEND);
	
	pRender2D->SetBlendMode(BE_MASK);
	
	const float tex_offset_x = uiCounter / 200.f;
	const TVertex2 quad[] = { // generate two triangles to form a quad and move their texture coordinates horizontally
		TVertex2(0.f, 0.f, tex_offset_x, 0.f, 1.f, 1.f, 1.f, 1.f), TVertex2(256.f, 0.f, 1.f + tex_offset_x, 0.f, 1.f, 1.f, 1.f, 1.f), TVertex2(256.f, 256.f, 1.f + tex_offset_x, 1.f, 1.f, 1.f, 1.f, 1.f),
		TVertex2(0.f, 0.f, tex_offset_x, 0.f, 1.f, 1.f, 1.f, 1.f), TVertex2(256.f, 256.f, 1.f + tex_offset_x, 1.f, 1.f, 1.f, 1.f, 1.f), TVertex2(0.f, 256.f, 0.f + tex_offset_x, 1.f, 1.f, 1.f, 1.f, 1.f)
	};
	
	pRender2D->DrawTriangles(pTexPlanet, quad, 6, PF_FILL);

	pRender2D->End2D();

	pRender->SetClearColor(ColorOfficialBlack()); // set clear color back
	
	pRender->SetRenderTarget(NULL);
}

void DGLE_API Update(void *pParameter)
{
	// render into the texture for advanced example part
	RenderPlanetInToTexture();

	// create water polygon for advanced example part

	TRectF section_adv_rect = TRectF(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
	
	const float bottom_y = section_adv_rect.y + section_adv_rect.height, right_x = section_adv_rect.x + section_adv_rect.width,
		texture_coord_koeff = 75.f;

	waterWaves.clear();
	waterWaves.push_back(TVertex2(section_adv_rect.x, bottom_y, section_adv_rect.x / texture_coord_koeff, bottom_y / texture_coord_koeff, 0.274f, 0.863f, 0.863f, 1.f));
	
	for (int i = 0; i < 33; ++i)
	{
		const float x = section_adv_rect.x + i * section_adv_rect.width / 32.f,
		y = (section_adv_rect.y + section_adv_rect.height / 2.f) + sinf((float)uiCounter / 20.f + (float)i * 50.f) * 15.f;
		waterWaves.push_back(TVertex2(x, y, x / texture_coord_koeff, y / texture_coord_koeff, 0.274f, 0.863f, 0.863f, 1.f));
	}

	waterWaves.push_back(TVertex2(right_x, bottom_y, right_x / texture_coord_koeff, bottom_y / texture_coord_koeff, 0.274f, 0.863f, 0.863f, 1.f));

	// other stuff

	stRandomCol = TColor4(rand() % 255, rand() % 255, rand() % 255, 255);
	
	++uiCounter;
}

void DrawPrimitives(const TRectF &screen)
{
	pRender2D->DrawRectangle(TRectF(5.f, 5.f, 170.f, 170.f), ColorBlue(), PF_LINE);
	pRender2D->DrawRectangle(TRectF(10.f, 10.f, 160.f, 160.f), ColorGray(), PF_FILL);

	const TVertex2 triangles[] = {TVertex2(25.f, 155.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f), TVertex2(25.f, 50.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f), TVertex2(140.f, 155.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f)};
	pRender2D->DrawTriangles(NULL, triangles, 3, PF_FILL);

	for (int i = 0; i <= 12; ++i)
		pRender2D->DrawPoint(TPoint2(screen.x, screen.y) + TPoint2(15.f + 12 * i, 15.f + 12 * i), ColorTeal(), 1 + i);

	for (int i = 0; i < 15; ++i)
	{
		pRender2D->SetLineWidth(1 + i / 2);
		pRender2D->DrawLine(TPoint2(screen.width - i * 20, screen.height), TPoint2(screen.width, screen.height - i * 20), ColorOfficialOrange(255 - i * 17));
	}

	pRender2D->SetLineWidth(2);
	pRender2D->SetVerticesColors(ColorAqua(), ColorFuchsia(), TColor4(), TColor4()); // override per vertex color for the line
	pRender2D->DrawLine(TPoint2(screen.width, 75.f), TPoint2(200.f, screen.height), TColor4(), PF_VERTICES_COLORS);

	pRender2D->SetLineWidth(1);

	pRender2D->SetVerticesColors(ColorGray(), ColorMagenta(), ColorOrange(), ColorViolet()); // override per vertex color for the rectangle
	pRender2D->DrawRectangle(TRectF(250.f, 25.f, 125.f, 125.f), TColor4(), (E_PRIMITIVE2D_FLAGS)(PF_LINE | PF_VERTICES_COLORS));
	
	pRender2D->SetVerticesColors(ColorBlack(0), ColorGreen(), ColorGreen(), ColorBlack(0)); 
	pRender2D->DrawRectangle(TRectF(260.f, 35.f, 105.f, 105.f), TColor4(), (E_PRIMITIVE2D_FLAGS)(PF_FILL | PF_VERTICES_COLORS));

	pRender2D->DrawCircle(TPoint2(125.f, 50.f), 40, 32, ColorOrange(), PF_FILL);
	for (int i = 0; i < 6; ++i)
		pRender2D->DrawCircle(TPoint2(125.f, 50.f), 10 + i * 5, 4 + i * 2, ColorWhite(), PF_LINE);

	pRender2D->DrawEllipse(TPoint2(200.f, 250.f), TVector2(110.f, 34.f), 64, ColorViolet(), PF_FILL);

	pRender2D->SetLineWidth(3);
	pRender2D->DrawEllipse(TPoint2(200.f, 250.f), TVector2(100.f, 30.f), 32, ColorWhite(), PF_LINE);

	pRender2D->DrawEllipse(TPoint2(200.f, 250.f), TVector2(80.f, 24.f), 8, ColorFuchsia(), PF_FILL);
	pRender2D->DrawEllipse(TPoint2(200.f, 250.f), TVector2(30.f, 75.f), 64, ColorOrange(100), PF_FILL);

	const float star_x = 25.f, star_y = 250.f;
	const TVertex2 star_polygon[] = 
	{
		TVertex2(47.f + star_x, 20.f + star_y, 1.f/*u*/, 1.f/*w*/, 0.7f/*r*/, 0.7f/*g*/, 0.f/*b*/, 1.f/*a*/),
		TVertex2(60.f + star_x, 48.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(91.f + star_x, 48.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(67.f + star_x, 64.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(80.f + star_x, 94.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(48.f + star_x, 75.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(21.f + star_x, 92.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(30.f + star_x, 65.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(1.f  + star_x, 49.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f),
		TVertex2(37.f + star_x, 48.f + star_y, 1.f, 1.f, 0.7f, 0.7f, 0.f, 1.f)
	};
	pRender2D->DrawPolygon(NULL, star_polygon, 10, PF_FILL);
}

void DrawFont(const TRectF &screen)
{
	const char *txt;
	uint width, height;
	
	txt = "I'm rotating... +_+";
	pFontBold->GetTextDimensions(txt, width, height);
	pFontBold->Draw2D(screen.x + screen.width - width - 10.f, 200, txt, ColorWhite(), (float)(uiCounter % 360));

	txt = "I'm just right aligned text. -_-";
	pFont->GetTextDimensions(txt, width, height);
	pFont->Draw2DSimple((int)(screen.x + screen.width - width), 10, txt);

	txt = "I have a shadow! ;-)";
	pFontBold->GetTextDimensions(txt, width, height);
	pFontBold->Draw2DSimple((int)(screen.x + screen.width - width - 7.f), 53, txt, ColorBlack());
	pFontBold->Draw2DSimple((int)(screen.x + screen.width - width - 10.f), 50, txt);

	txt = "Cool colored me! ^_^";
	pFontBold->GetTextDimensions(txt, width, height);
	pRender2D->SetVerticesColors(ColorRed(), ColorGreen(), ColorRed(), ColorGreen());
	pFontBold->Draw2D(screen.x + screen.width - width - 10.f, 100.f, txt, ColorWhite(), 0.f, true);

	txt = "I'm randomly colored... >_<";
	pFont->Draw2D(screen.x + 5, 10, txt, stRandomCol);

	txt = "I'm per vertex colored! -_0";
	pRender2D->SetVerticesColors(ColorAqua(), ColorAqua(), ColorOrange(), ColorOrange());
	pFontBold->Draw2D(screen.x + 5, 50, txt, ColorWhite(), 0.f, true);

	txt = "А я могу говорить по-русски! [:-|";
	pFont->Draw2DSimple((int)(screen.x + 5.f), 100, txt, ColorRed());

	txt = "I'm scaling... o_O";
	pFontBold->SetScale(fabs(sinf((float)uiCounter / 50.f)) * 2.f);
	pFontBold->Draw2DSimple((int)(screen.x + 5.f), 200, txt);
	pFontBold->SetScale(1.f);

	txt = "I am just very brutal..!";
	pFontHard->GetTextDimensions(txt, width, height);
	pFontHard->Draw2DSimple((int)(screen.x + (screen.width - width) / 2.f), 300, txt, ColorOfficialOrange());
}

void DrawSprites(const TRectF &screen)
{
	pTexGrass->Draw2D((int)screen.x, (int)screen.y, (int)screen.width, (int)screen.height);
	
	pTexGirl->Draw2DSimple((int)screen.x + 400, (int)screen.y + 30, (uiCounter / 5) % 16); // simpliest way to draw animated sprite

	// draw tank	
	uint width, height;
	
	pRender2D->SetScale(TPoint2(2.f, 2.f)); // scale X and Y axes by two

	pTexTankBody->GetDimensions(width, height);
	pRender2D->DrawTexture(pTexTankBody, TPoint2(screen.x + 50.f, screen.y + 150.f), TVector2((float)width, (float)height), 0.f,
		(E_EFFECT2D_FLAGS)(EF_ALPHA_TEST /* better than blending if we want to have sharp edges */ | EF_SCALE));	

	pTexTankTurret->GetDimensions(width, height);
	pRender2D->SetRotationPoint(TPoint2(48.f, 64.f)); // rotate turret around it's base center, not the center of the image
	TPoint2 turret_center = TPoint2(screen.x + 50.f - 10.f, screen.y + 150.f + 18.f);
	float turret_angle = -30.f + sinf(uiCounter / 15.f) * 20.f;
	pRender2D->DrawTexture(pTexTankTurret, turret_center, TVector2((float)width, (float)height), turret_angle, (E_EFFECT2D_FLAGS)(EF_ALPHA_TEST | EF_SCALE | EF_ROTATION_POINT));

	// draw light at the end of tank cannon
	pRender2D->SetBlendMode(BE_ADD);
	pTexLight->GetFrameSize(width, height);
	pRender2D->DrawTextureSprite(pTexLight, turret_center + TPoint2(cosf(turret_angle * TO_RAD) * 165.f, sinf((turret_angle - 15.f) * TO_RAD) * 165.f),
		TVector2((float)width, (float)height), (uiCounter / 2) % 14, 0.f, EF_BLEND);
	pRender2D->SetBlendMode(BE_NORMAL); // switch back blending mode to common one
}

void DrawAdvanced(const TRectF &screen)
{
	pTexBg->Draw2D((int)screen.x, (int)screen.y, (uint)screen.width, (uint)screen.height);
	
	pRender2D->DrawTexture(pTexPlanetRenderIn, TPoint2(screen.x + screen.width - 160.f, screen.y + 20.f), TVector2(128.f, 128.f), 0.f,
		(E_EFFECT2D_FLAGS)(EF_BLEND | EF_FLIP_VERTICALLY /* because render targets are flipped */ ));
	
	pRender2D->SetBlendMode(BE_ADD);

	pRender2D->BeginBatch(false); // not to triangulate complex polygon every frame we can batch it this way and triangulation will be done only after update tick
	pRender2D->DrawPolygon(pTexAnimWater[uiCounter / 5 % 11], &waterWaves[0], waterWaves.size(), PF_FILL);
	pRender2D->EndBatch();

	uint w, h;
	pTexJellyFish->GetDimensions(w, h);
	const float x_offset = fabs(cosf(uiCounter / 20.f)) * 50.f, y_offset = sinf(uiCounter / 10.f) * 75.f;
	const TPoint2 jellyf_pos = TPoint2(screen.x + 175.f, screen.y + 200.f);

	pRender2D->SetVerticesOffsets(TPoint2(), TPoint2(), TPoint2(x_offset, -y_offset), TPoint2(-x_offset, -y_offset));
	pRender2D->SetColorMix(ColorViolet());
	pRender2D->DrawTexture(pTexJellyFish, jellyf_pos, TVector2(w / 2.f, h / 2.f), -45.f, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_VERTICES_OFFSETS | EF_COLOR_MIX));
	
	pTexLightRound->GetDimensions(w, h);
	pRender2D->DrawTexture(pTexLightRound, jellyf_pos - TPoint2(90.f, 15.f), TVector2((float)w, (float)h), 0.f, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_COLOR_MIX));
}

void DGLE_API Render(void *pParameter)
{
	TRectF section_rect;
	uint txt_w, txt_h;

	pRender2D->Begin2D();

	// First section with primitives. //
	section_rect = TRectF(0.f, 0.f, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
	pRender->EnableScissor(section_rect); // here and below scissor test is used to prevent drawing something to another section of the screen

	DrawPrimitives(section_rect);
	pRender2D->DrawRectangle(section_rect); // here and below draw white frame around current section

	const char txt_sec_1[] = "Primitives";
	pFont->GetTextDimensions(txt_sec_1, txt_w, txt_h);
	pFont->Draw2DSimple((SCREEN_WIDTH / 2 - txt_w) / 2, SCREEN_HEIGHT / 2 - txt_h, txt_sec_1); // here and below draw section label near the end of each section

	// Second section with font rendering. //
	section_rect = TRectF(SCREEN_WIDTH / 2.f, 0.f, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
	pRender->EnableScissor(section_rect);

	DrawFont(section_rect);
	pRender2D->DrawRectangle(section_rect);

	const char txt_sec_2[] = "Fonts";
	pFont->GetTextDimensions(txt_sec_2, txt_w, txt_h);
	pFont->Draw2DSimple(SCREEN_WIDTH / 2 + (SCREEN_WIDTH / 2  - txt_w) / 2, SCREEN_HEIGHT / 2 - txt_h, txt_sec_2);

	// Third section with sprites. //
	section_rect = TRectF(0.f, SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
	pRender->EnableScissor(section_rect);
	
	DrawSprites(section_rect);
	pRender2D->DrawRectangle(section_rect);

	const char txt_sec_3[] = "Sprites";
	pFont->GetTextDimensions(txt_sec_3, txt_w, txt_h);
	pFont->Draw2DSimple((SCREEN_WIDTH / 2 - txt_w) / 2, SCREEN_HEIGHT - txt_h, txt_sec_3);

	// Fourth section with advanced techniques. //
	section_rect = TRectF(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);
	pRender->EnableScissor(section_rect);

	DrawAdvanced(section_rect);
	pRender2D->DrawRectangle(section_rect);

	const char txt_sec_4[] = "Advanced";
	pFont->GetTextDimensions(txt_sec_4, txt_w, txt_h);
	pFont->Draw2DSimple(SCREEN_WIDTH / 2 + (SCREEN_WIDTH / 2  - txt_w) / 2, SCREEN_HEIGHT - txt_h, txt_sec_4);

	pRender->DisableScissor(); // The way to turn off scissor test, you must do it manually before the end of the frame.

	pRender2D->End2D();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (GetEngine(DLL_PATH, pEngineCore))
	{
		if (SUCCEEDED(pEngineCore->InitializeEngine(NULL, APP_CAPTION, TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, false, MM_4X /* multisampling will smooth lines and primitive edges */ ))))
		{
			pEngineCore->AddProcedure(EPT_INIT,	&Init);
			pEngineCore->AddProcedure(EPT_UPDATE, &Update);
			pEngineCore->AddProcedure(EPT_RENDER, &Render);

			pEngineCore->StartEngine();
		}

		FreeEngine();
	}
	else
		MessageBox(NULL, "Couldn't load \"" DLL_PATH "\"!", APP_CAPTION, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

	return 0;
}