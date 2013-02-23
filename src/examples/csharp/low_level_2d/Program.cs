using System;
using System.Collections.Generic;
using System.Windows.Forms;

using DGLE;

namespace SimplestSharp
{
	public class Program
	{
		const string APP_CAPTION = "Low Level 2D";
		const string DLL_PATH = @"..\..\..\..\Bin\DGLE.dll";

		const string RESOURCE_PATH = @"..\..\..\..\resources\";

		IEngineCore pEngineCore = null;
		IRender2D pRender2D = null;
		IInput pInput = null;

		// Textures
		ITexture pBg = null, pSky = null, pFog = null, pTexGirl = null, pLight = null, pLightRound = null, pOwl = null, pVox = null;

		IBitmapFont pFont = null;

		ISoundSample pSndOwl = null, pForestAmbient = null;
		ISoundChannel pChannelAmbientLoop = null;

		string HELP_TEXT = "You can resize this window to see it's behavior or go fullscreen by pressing \"Alt + Enter\" keys.";

		// This example is made for 800X600 screen resolution.
		uint GAME_VP_WIDTH = 800u;
		uint GAME_VP_HEIGHT = 600u;

		// Screen resolution
		uint SCREEN_WIDTH = 1024u;
		uint SCREEN_HEIGHT = 768u;




		// Lights positions
		TPoint2[] lights = new TPoint2[5] 
		{
			new TPoint2(0f, 0f),
			new TPoint2(750f,315f),
			new TPoint2(1000f,450f),
			new TPoint2(1010f,650f),
			new TPoint2(150f,550f)
		};

		int counter = 800;
		uint prevWindowWidth, prevWindowHeight;

		TPoint2 cameraPosition;
		float cameraAngle = 0f, cameraScale = 1f;

		TPoint2 stMousePos;

		bool owlGoLeft = false;
		float owlX = -200f;


		void OnFullScreenEvent(IntPtr pParametr, IBaseEvent pEvent)
		{
			IEvGoFullScreen p_event = (IEvGoFullScreen)pEvent;

			uint res_width, res_height;
			bool go_fscreen;
			p_event.GetResolution(out res_width, out  res_height, out go_fscreen);

			if (go_fscreen)
			{
				prevWindowWidth = res_width;
				prevWindowHeight = res_height;

				pEngineCore.GetDesktopResolution(out  res_width, out res_height);
				p_event.SetResolution(res_width, res_height);
			}
			else
				p_event.SetResolution(prevWindowWidth, prevWindowHeight);
		}


		void Init(IntPtr pParam)
		{

			IEngineSubSystem p_sub_sys = null;

			IResourceManager p_res_man = null;
			pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out p_sub_sys);
			p_res_man = (IResourceManager)p_sub_sys;

			IRender p_render = null;
			pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out p_sub_sys);
			p_render = (IRender)p_sub_sys;
			p_render.GetRender2D(out pRender2D);

			IEngBaseObj p_obj = null;

			pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out p_sub_sys);
			pInput = (IInput)p_sub_sys;
			pInput.Configure(E_INPUT_CONFIGURATION_FLAGS.ICF_HIDE_CURSOR);

			p_res_man.GetDefaultResource(E_ENG_OBJ_TYPE.EOT_BITMAP_FONT, out  p_obj);
			pFont = (IBitmapFont)p_obj;


			// Background clear color setup.
			TColor4 tc = new TColor4(38, 38, 55, 255);
			p_render.SetClearColor(ref tc);

			// This example adapted only for 800X600 resolution, if even resolution will be higher it won't effect it.
			pRender2D.SetResolutionCorrection(GAME_VP_WIDTH, GAME_VP_HEIGHT, true);



			p_res_man.Load(RESOURCE_PATH + "sounds\\owl.wav", out p_obj, 0);
			pSndOwl = (ISoundSample)p_obj;

			p_res_man.Load(RESOURCE_PATH + "sounds\\forest_ambient.wav", out p_obj, 0);
			pForestAmbient = (ISoundSample)p_obj;
			pForestAmbient.PlayEx(out pChannelAmbientLoop, E_SOUND_SAMPLE_PARAMS.SSP_LOOPED);

			p_res_man.Load(RESOURCE_PATH + "sprites\\cartoon_forest_background.png", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pBg = (ITexture)p_obj;
			p_res_man.Load(RESOURCE_PATH + "sprites\\cartoon_cloudy_night_sky.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pSky = (ITexture)p_obj;
			p_res_man.Load(RESOURCE_PATH + "textures\\smoke.png", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pFog = (ITexture)p_obj;
			p_res_man.Load(RESOURCE_PATH + "textures\\light.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pLightRound = (ITexture)p_obj;

			p_res_man.Load(RESOURCE_PATH + "sprites\\vox.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pVox = (ITexture)p_obj;
			pVox.SetFrameSize(149, 149);

			p_res_man.Load(RESOURCE_PATH + "sprites\\cartoon_owl.png", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pOwl = (ITexture)p_obj;
			pOwl.SetFrameSize(48, 128);

			p_res_man.Load(RESOURCE_PATH + "sprites\\cartoon_anime_girl.png", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pTexGirl = (ITexture)p_obj;
			pTexGirl.SetFrameSize(55, 117);

			p_res_man.Load(RESOURCE_PATH + "sprites\\cartoon_mistery_light.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
			pLight = (ITexture)p_obj;
			pLight.SetFrameSize(64, 128);

		}

		void Free(IntPtr pParam)
		{
			//It's better(not necessary) to free resources on exit
			// It's a good idea to stop and unaquire looped sound channels before quit.
			pChannelAmbientLoop.Stop();
			pChannelAmbientLoop.Unaquire();
		}

		void Update(IntPtr pParam)
		{
			bool prsd;

			pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_ESCAPE, out prsd);

			if (prsd)
			{
				pEngineCore.QuitEngine();
				return;
			}

			TMouseStates states;
			pInput.GetMouseStates(out states);
			stMousePos = new TPoint2((float)states.iX, (float)states.iY);

			if (counter % 250 == 0)
			{
				ISoundChannel chan;
				pSndOwl.PlayEx(out chan, E_SOUND_SAMPLE_PARAMS.SSP_NONE);
				chan.SetPan((int)(owlX - cameraPosition.x));
				chan.Unaquire();
			}

			cameraPosition.x = (float)(GAME_VP_WIDTH / 2f + 250f + Math.Cos(counter / 200f) * 225f);
			cameraPosition.y = GAME_VP_HEIGHT / 2f + 300f;

			cameraAngle = (float)Math.Sin(counter / 200f) * 5f;
			cameraScale = (float)(1f + Math.Abs(Math.Sin(counter / 200f) * 0.25f));

			lights[0].x = (float)(650f - Math.Cos(counter / 100f) * 100f);
			lights[0].y = (float)(700f - Math.Sin(counter / 150f) * 50f);

			if (owlX > 1500f)
				owlGoLeft = true;
			else
				if (owlX < 100f)
					owlGoLeft = false;

			owlX += (owlGoLeft ? -1f : 1f) * 4f;

			++counter;

		}

		void Render(IntPtr pParam)
		{
			if (pRender2D == null) return;

			pRender2D.Begin2D();

			// Draw  sky

			TPoint2 ps = new TPoint2();
			TPoint2 pt = new TPoint2(GAME_VP_WIDTH, GAME_VP_HEIGHT);

			pRender2D.DrawTex(pSky, ref ps, ref pt, 0, E_EFFECT2D_FLAGS.EF_NONE);

			// Draw background

			ps = new TPoint2(cameraPosition.x / 1.5f, cameraPosition.y / 1.5f);
			pt = new TPoint2(cameraScale, cameraScale);

			pRender2D.SetCamera(ref ps, cameraAngle, ref pt);

			pRender2D.SetBlendMode(E_EFFECT2D_BLENDING_FLAGS.EBF_NORMAL);

			ps = new TPoint2(-200f, 150f);
			pt = new TPoint2(1399f, 517f);
			TRectF rect = new TRectF(0f, 905f, 1399f, 517f);


			pRender2D.DrawTexCropped(pBg, ref ps, ref pt, ref rect, 0.0f, E_EFFECT2D_FLAGS.EF_BLEND);

			// Draw moving fog on background

			ps = new TPoint2(cameraPosition.x / 1.2f, cameraPosition.y / 1.2f);
			pt = new TPoint2(cameraScale, cameraScale);
			pRender2D.SetCamera(ref ps, cameraAngle, ref pt);

			pRender2D.SetBlendMode(E_EFFECT2D_BLENDING_FLAGS.EBF_ADD);

			// Sometimes it's better to batch a lot of similar objects.

			pRender2D.BeginBatch();

			TColor4 col = new TColor4(255, 255, 255, 64);

			pRender2D.SetColorMix(ref col);

			for (uint i = 0; i < 22; ++i)
				for (uint j = 0; j < 2; ++j)
				{
					ps = new TPoint2((float)(500f + Math.Sin((float)i) * 600f - Math.Cos((float)counter / 50f) * 10f * (i % 4)),
							   (float)(475f + Math.Sin((float)j * 2.5f) * 150f - Math.Sin((float)counter / 50f) * 20f * (i % 3)));
					pt = new TPoint2(250f, 150f);
					pRender2D.DrawTex(pFog, ref ps, ref pt, 0f, (E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_COLORMIX));
				}

			pRender2D.EndBatch();

			// Draw foreground scene

			// Setup camera and blending
			pt = new TPoint2(cameraScale, cameraScale);
			pRender2D.SetCamera(ref cameraPosition, cameraAngle, ref pt);

			pRender2D.SetBlendMode(E_EFFECT2D_BLENDING_FLAGS.EBF_NORMAL);

			// Owl
			col = new TColor4(150, 150, 150, 255);
			pRender2D.SetColorMix(ref col); // make sprite little darker

			ps = new TPoint2(owlX, 425f);
			pt = new TPoint2(48f, 128f);
			E_EFFECT2D_FLAGS EEF = (E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_COLORMIX | (owlGoLeft ? E_EFFECT2D_FLAGS.EF_FLIPX : E_EFFECT2D_FLAGS.EF_DEFAULT));
			pRender2D.DrawTexSprite(pOwl, ref ps, ref pt, (uint)(counter / 3) % 15, 0f, EEF);

			// Draw tree
			ps = new TPoint2();
			pt = new TPoint2(1399f, 900f);
			rect = new TRectF(0f, 0f, 1399f, 900f);
			pRender2D.DrawTexCropped(pBg, ref ps, ref pt, ref rect, 0f, E_EFFECT2D_FLAGS.EF_BLEND);

			// Glowing disc under moving light

			pRender2D.SetBlendMode(E_EFFECT2D_BLENDING_FLAGS.EBF_ADD);

			ps = new TPoint2(-100f, 0f);
			pt = new TPoint2();
			pRender2D.SetVerticesOffset(ref ps, ref ps, ref pt, ref pt);
			col = new TColor4(65, 59, 193, 255);
			pRender2D.SetColorMix(ref col);
			ps = new TPoint2(lights[0].x - 32f, lights[0].y + 64f);
			pt = new TPoint2(256f, 256f);
			pRender2D.DrawTex(pLightRound, ref ps, ref pt, 0f, (E_EFFECT2D_FLAGS.EF_VERTICES_OFFSET | E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_COLORMIX));

			// Girl Shadow

			pRender2D.SetBlendMode(E_EFFECT2D_BLENDING_FLAGS.EBF_NORMAL);
			ps = new TPoint2(-150f + (float)Math.Cos((float)counter / 100f) * 100f, -55f);
			pt = new TPoint2(-50f + (float)Math.Cos((float)counter / 100f) * 100f, -55f);
			TPoint2 pp = new TPoint2(15f, 5f);
			pRender2D.SetVerticesOffset(ref ps, ref pt, ref pp, ref pp);
			col = new TColor4(0, 0, 0, 128);
			pRender2D.SetColorMix(ref col);
			ps = new TPoint2(550f, 725f);
			pt = new TPoint2(60f, 120f);
			pRender2D.DrawTexSprite(pTexGirl, ref ps, ref pt, (uint)(counter / 5) % 16, 0f, (E_EFFECT2D_FLAGS.EF_VERTICES_OFFSET | E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_COLORMIX));

			// Girl Sprite			
			pRender2D.DrawTexSprite(pTexGirl, ref ps, ref pt, (uint)(counter / 5) % 16, 0f, E_EFFECT2D_FLAGS.EF_BLEND);

			// Lights

			pRender2D.SetBlendMode(E_EFFECT2D_BLENDING_FLAGS.EBF_ADD);

			for (uint i = 0; i < 5; ++i)
			{
				ps = new TPoint2(lights[i].x, lights[i].y);
				pt = new TPoint2(64f, 128f);
				pRender2D.DrawTexSprite(pLight, ref ps, ref pt, (uint)(counter / 2) % 14, 0f, E_EFFECT2D_FLAGS.EF_BLEND);
			}

			pRender2D.ResetCamera();

			// We must calculate correct coordinates in game space because game resolution and screen resolution can be different.
			TPoint2 pos;
			pRender2D.CoordAbsoluteToResCorrect(ref stMousePos, out pos);
			ps = new TPoint2(pos.x - 37, pos.y - 37);
			pt = new TPoint2(75f, 75f);
			pRender2D.DrawTexSprite(pVox, ref ps, ref pt, (uint)(counter / 2) % 16, 0f, E_EFFECT2D_FLAGS.EF_BLEND);

			uint tw, th;
			pFont.GetTextDimensions(HELP_TEXT, out tw, out th);
			col = TColor4.White;
			pFont.Draw2D((float)((GAME_VP_WIDTH - tw) / 2), (float)(GAME_VP_HEIGHT - th), HELP_TEXT, ref col);

			pRender2D.End2D();

		}

		void Run()
		{
			if (Engine.GetEngine(DLL_PATH, out pEngineCore))
			{
				TEngWindow win = new TEngWindow(GAME_VP_WIDTH, GAME_VP_HEIGHT, false, true, 1, E_ENG_WINDOW_FLAGS.EWF_ALLOW_SIZEING);
				pEngineCore.InitializeEngine(IntPtr.Zero, APP_CAPTION, ref win, 33, E_ENGINE_INIT_FLAGS.EIF_DEFAULT);
				// You can do some initialization here

				pEngineCore.ConsoleVisible(true);

				pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, new DSubscriber(Init), IntPtr.Zero);
				pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, new DSubscriber(Free), IntPtr.Zero);
				pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, new DSubscriber(Update), IntPtr.Zero);
				pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, new DSubscriber(Render), IntPtr.Zero);

				pEngineCore.AddEventListener(E_EVENT_TYPE.ET_ON_FULLSCREEN, new DListenerProc(OnFullScreenEvent), IntPtr.Zero);

				pEngineCore.StartEngine(); //Entering engine loop
			}
			Engine.FreeEngine();
		}

		[STAThread]
		static void Main()
		{
			Program prog = new Program();
			prog.Run();
		}
	}
}
