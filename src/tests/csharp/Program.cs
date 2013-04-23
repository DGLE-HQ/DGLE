using System;
using System.Windows.Forms;
using DGLE;

namespace CSharpTest
{
    class Program
    {
        const uint SCREEN_WIDTH = 320;
        const uint SCREEN_HEIGHT = 240;

        static IEngineCore engCore = null;
        static IInput engInput = null;
        static IBitmapFont mainFont = null;

        static DSubscriber delInit = null;
        static DSubscriber delFree = null;
        static DSubscriber delUpdate = null;
        static DSubscriber delRender = null;

        static void Init(IntPtr pParam)
        {
            IEngineSubSystem subSys = null;

            IResourceManager resman;
            engCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out subSys);
            resman = (IResourceManager)subSys;

            IEngineBaseObject obj;
            resman.GetDefaultResource(E_ENGINE_OBJECT_TYPE.EOT_BITMAP_FONT, out obj);
            mainFont = (IBitmapFont)obj;

            engCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out subSys);
            engInput = (IInput)subSys;
        }

        static void Free(IntPtr pParam)
        {

        }

        static void Update(IntPtr pParam)
        {
            bool isKeyPressed;

            engInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_ESCAPE, out isKeyPressed);
            if (isKeyPressed)
                engCore.QuitEngine();
        }

        static void Render(IntPtr pParam)
        {
            string txt = "Hello, world!";
            TColor4 color = TColor4.ColorWhite();
            uint w, h;
            
            mainFont.GetTextDimensions(txt, out w, out h);
            mainFont.Draw2DSimple((int)((SCREEN_WIDTH - w) / 2), (int)((SCREEN_HEIGHT - h) / 2), txt, ref color);
        }

        [STAThread]
        static void Main()
        {
            if (Engine.GetEngine("..\\DGLE.dll", out engCore))
            {
                TEngineWindow engWin = new TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false);
                engCore.InitializeEngine(IntPtr.Zero, Application.ProductName, ref engWin, 33, E_ENGINE_INIT_FLAGS.EIF_DEFAULT);

                engCore.ConsoleVisible(true);

                delInit = Init;
                delFree = Free;
                delUpdate = Update;
                delRender = Render;

                engCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, delInit, IntPtr.Zero);
                engCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, delFree, IntPtr.Zero);
                engCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, delUpdate, IntPtr.Zero);
                engCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, delRender, IntPtr.Zero);

                engCore.StartEngine();
                
                Engine.FreeEngine();
            }
            else
                MessageBox.Show("Couldn't load \"DGLE.dll\"!", Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
