using System;
using System.Windows.Forms;
using DGLE;

namespace CSharpTest
{
    class Program
    {
        static IEngineCore core = null;
        static IResourceManager resman = null;
        static IRender render = null;
        static IInput input = null;

        static uint counter = 0;

        static DSubscriber delInit = null;
        static DSubscriber delFree = null;
        static DSubscriber delUpdate = null;
        static DSubscriber delRender = null;

        static void Init(IntPtr pParam)
        {
            IEngineSubSystem subSys = null;

            core.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out subSys);
            resman = (IResourceManager)subSys;

            core.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out subSys);
            render = (IRender)subSys;

            core.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out subSys);
            input = (IInput)subSys;
        }

        static void Free(IntPtr pParam)
        {

        }

        static void Update(IntPtr pParam)
        {
            bool keyPrsd;

            input.GetKey(E_KEYBOARD_KEY_CODES.KEY_ESCAPE, out keyPrsd);
            if (keyPrsd)
                core.QuitEngine();

            ++counter;
        }

        static void Render(IntPtr pParam)
        {

        }

        [STAThread]
        static void Main()
        {
            if (Engine.GetEngine("..\\DGLE.dll", out core))
            {
                TEngWindow eng_win = TEngWindow.Default;
                core.InitializeEngine(IntPtr.Zero, Application.ProductName, ref eng_win, 33, E_ENGINE_INIT_FLAGS.EIF_DEFAULT);

                core.ConsoleVisible(true);
                core.ConsoleExec("core_fps_in_caption 1");

                delInit = Init;
                delFree = Free;
                delUpdate = Update;
                delRender = Render;

                //pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, delInit, IntPtr.Zero);
                //pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, delFree, IntPtr.Zero);
                //pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, delUpdate, IntPtr.Zero);
                //pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, delRender, IntPtr.Zero);

                core.StartEngine();
                
                Engine.FreeEngine();
            }
            else
                MessageBox.Show("Couldn't load \"DGLE.dll\"!", Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
