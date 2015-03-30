using System;
using DGLE;
using System.Windows.Forms;

namespace HelloWorld
{
	class Program	{
		
		const string AppCaption = "Hello World!";
        const string DllPath = @"..\..\..\..\windows\DGLE.dll";
	    const string ResPath = @"..\..\..\..\..\resources\";

		const uint ScreenWidth = 320u;
		const uint ScreenHeight = 240u;
		
		IEngineCore pEngineCore;
		IInput pInput;
		
		ITexture pTex, pTexSprite;
	    IBitmapFont pFont;

	    private uint counter = 0;
		
		void Init(IntPtr pParam)
		{
			IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out pSubSys);
            pInput = (IInput)pSubSys;

		    IResourceManager pResMan;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
			pResMan = (IResourceManager)pSubSys;
			
			IEngineBaseObject pBaseObj = null;

            pResMan.Load(ResPath + "sprites\\cartoon_cloudy_night_sky.jpg", out pBaseObj, 0);
			pTex = (ITexture)pBaseObj;
            pResMan.Load(ResPath + "sprites\\cartoon_owl.png", out pBaseObj, 0);
            pTexSprite = (ITexture)pBaseObj;
            pResMan.GetDefaultResource(E_ENGINE_OBJECT_TYPE.EOT_BITMAP_FONT, out pBaseObj);
            pFont = (IBitmapFont)pBaseObj;

            pTexSprite.SetFrameSize(48, 128);
		}
		
		void Free(IntPtr pParam)
		{
			
		}
		
		void Update(IntPtr pParam)
		{
		    bool isPressed;
            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_ESCAPE, out isPressed);

            if(isPressed)
                pEngineCore.QuitEngine();
		    ++counter;
		}
		
		void Render(IntPtr pParam)
		{
			pTex.Draw2D(0, 0, ScreenWidth, ScreenHeight, 0, 0);

		    uint width, height, framecount;

		    TColor4 c = TColor4.ColorWhite();

		    const string txt = AppCaption;
            pFont.GetTextDimensions(txt, out width, out height);
            pFont.Draw2DSimple((int) ((ScreenWidth - width) / 2), 
                (int) ((ScreenHeight - height)/2), txt, ref c);

            pTexSprite.GetFrameSize(out width, out height);
            pTexSprite.FramesCount(out framecount);
            pTexSprite.Draw2DSimple((int) ((ScreenWidth - width) / 2),
                5, (counter / 2) % framecount);
		}
		
		void Run()
		{
			if(Engine.GetEngine(DllPath, out pEngineCore))
			{
				var win = new TEngineWindow(ScreenWidth, ScreenHeight, false);
				pEngineCore.InitializeEngine(IntPtr.Zero, AppCaption, ref win);
				
				pEngineCore.ConsoleVisible(true);
				
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, new DSubscriber(Init), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, new DSubscriber(Free), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, new DSubscriber(Update), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, new DSubscriber(Render), IntPtr.Zero);
				
				pEngineCore.StartEngine();
			}
			else
				MessageBox.Show("Couldn't load \"DGLE.dll\"!", AppCaption, MessageBoxButtons.OK, MessageBoxIcon.Error);
			Engine.FreeEngine();
		}
		
		[STAThread]
		static void Main()
		{
			var prog = new Program();
			prog.Run();
		}
	}
}