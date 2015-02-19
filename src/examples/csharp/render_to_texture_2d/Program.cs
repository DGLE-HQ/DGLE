using System;
using System.Collections.Generic;
using DGLE;
using System.Windows.Forms;

namespace Demo
{
    class Program
    {

        const string AppCaption = "Render to Texture 2D";
        const string DLL_PATH = @"..\..\..\..\windows\DGLE.dll";
        const string RESOURCE_PATH = @"..\..\..\..\..\resources\";
        
        const uint ScreenWidth = 1024u;
        const uint ScreenHeight = 768u;

        private TPoint2 ScreenSize = new TPoint2(ScreenWidth, ScreenHeight);

        IEngineCore pEngineCore;
        private IRender pRender;
        private IRender2D pRender2D;
        private IInput pInput;

        ITexture pTexLight, pTexBg, pTexBug, pTexBugCorpse, pTexSmoke, pTexBlood;
        ITexture[] pTexTarget = new ITexture[3];
        ISoundSample pSndDeath;

        private Random rand = new Random();

        private uint counter = 0;

        private TPoint2 mousePos;

        class TBug
        {
            public TPoint2 pos;
            public float angle;
            public int lifes;

            public TBug(TPoint2 pos, float ang)
            {
                this.pos = pos;
                lifes = 100;
                angle = ang;
            }
        }

        List<TBug> bugs = new List<TBug>();

        void Init(IntPtr pParam)
        {
            IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out pSubSys);
            pRender = (IRender)pSubSys;
            pRender.GetRender2D(out pRender2D);

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out pSubSys);
            pInput = (IInput)pSubSys;
            pInput.Configure(E_INPUT_CONFIGURATION_FLAGS.ICF_HIDE_CURSOR);

            IResourceManager pResMan;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;

            pResMan.Load(RESOURCE_PATH + "sounds\\bug_death.wav", out pBaseObj, 0);
            pSndDeath = (ISoundSample)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "sprites\\light.jpg", out pBaseObj, 0);
            pTexLight = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "sprites\\smoke.png", out pBaseObj, 0);
            pTexSmoke = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "sprites\\bug.png", out pBaseObj, 0);
            pTexBug = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "sprites\\bug_corpse.png", out pBaseObj, 0);
            pTexBugCorpse = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "sprites\\blood.png", out pBaseObj, 0);
            pTexBlood = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "textures\\cartoon_grass.tga", out pBaseObj, (uint)(E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR |
                        E_TEXTURE_LOAD_FLAGS.TLF_COORDS_REPEAT));
            pTexBg = (ITexture)pBaseObj;

            pTexBug.SetFrameSize(52, 42);

            for (uint i = 0; i < 3; i++)
            {
                pResMan.CreateTexture(out pTexTarget[i], null, ScreenWidth, ScreenHeight,
                    E_TEXTURE_DATA_FORMAT.TDF_RGB8, E_TEXTURE_CREATE_FLAGS.TCF_DEFAULT,
                    E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR | E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            }
            Clear();
        }

        void Clear()
        {
            bugs.Clear();

            for (var i = 0; i < 100; i++)
            {
                bugs.Add(new TBug(new TPoint2(rand.Next((int)ScreenWidth), rand.Next((int)ScreenHeight)),
                    rand.Next(360)));
            }

            // render clear background image to texture for the first time
            for (uint i = 0; i < 2; i++)
            {
                pRender.SetRenderTarget(pTexTarget[i]);
                TPoint2 pos = new TPoint2();
                pRender2D.DrawTexture(pTexBg, ref pos, ref ScreenSize, 0f, E_EFFECT2D_FLAGS.EF_BLEND |
                    E_EFFECT2D_FLAGS.EF_TILE_TEXTURE);
            }

            pRender.SetRenderTarget(null);
        }

        void Free(IntPtr pParam)
        {

        }

        void Update(IntPtr pParam)
        {
            // We must switch between two textures, because we can't call DrawTexture for texture which is current render target.

            // Firstly rendering previous background texture with new bugs corpses and trails.
            TPoint2 pos = new TPoint2();
            TPoint2 dim;
            TColor4 c;
            uint co = counter % 2;
            pRender.SetRenderTarget(pTexTarget[co]);
            pRender2D.DrawTexture(pTexTarget[co == 1 ? 0 : 1], ref pos, ref ScreenSize, 0,
                E_EFFECT2D_FLAGS.EF_FLIP_VERTICALLY);

            for (int index = 0; index < bugs.Count; index++)
            {
                var bug = bugs[index];
                if (bug.lifes < 0)
                {
                    pSndDeath.Play();

                    pos = bug.pos + new TPoint2(5f, 0f);
                    dim = new TPoint2(42f, 42f);
                    pRender2D.DrawTexture(pTexBugCorpse, ref pos, ref dim, 90f + bug.angle);
                    bugs.RemoveAt(index);
                }
            }

            // Secondly rendering background texture with moving bugs.
            pRender.SetRenderTarget(pTexTarget[2]);
            pos = new TPoint2();
            pRender2D.DrawTexture(pTexTarget[counter % 2], ref pos, ref ScreenSize, 0, E_EFFECT2D_FLAGS.EF_FLIP_VERTICALLY);
            dim = new TPoint2(52f, 42f);

            foreach (var bug in bugs)
            {
                TPoint2 old = bug.pos;

                bug.pos.x += (float)Math.Cos(bug.angle * Math.PI / 180f) * 0.5f;
                bug.pos.y += (float)Math.Sin(bug.angle * Math.PI / 180f) * 0.5f;

                if (bug.pos.x > ScreenWidth || bug.pos.y > ScreenHeight || bug.pos.x < 0f || bug.pos.y < 0f)
                {
                    bug.pos = old;
                    bug.angle = rand.Next(360);
                }

                if (mousePos.DistTo(bug.pos) < 50f)
                {
                    bug.lifes -= 2;                    
                }
                byte hp = (byte)(bug.lifes > 0 ? (bug.lifes * 2.5 > 256 ? 255: bug.lifes * 2.5) : 0);
                c = new TColor4(255, hp, hp, 255);
                pRender2D.SetColorMix(ref c);
                pRender2D.DrawTextureSprite(pTexBug, ref bug.pos, ref dim, ((uint)(bug.angle + counter / 4) % 8),
                    90f + bug.angle, E_EFFECT2D_FLAGS.EF_BLEND | E_EFFECT2D_FLAGS.EF_COLOR_MIX);
            }

            pRender.SetRenderTarget(null);
            // process user input here
            TMouseStates ms;
            pInput.GetMouseStates(out ms);
            mousePos = new TPoint2(ms.iX, ms.iY);

            bool prsd;
            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_ESCAPE, out prsd);
            if (prsd)
            {
                pEngineCore.QuitEngine();
                return;
            }

            pInput.GetKey(E_KEYBOARD_KEY_CODES.KEY_SPACE, out prsd);
            if (prsd)
            {
                Clear();
                counter = 0;
                return;
            }

            ++counter;
        }

        void Render(IntPtr pParam)
        {
            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_NORMAL);
            TPoint2 pos = mousePos - new TPoint2(200f, 200f);
            TPoint2 dim = new TPoint2(400f, 400f);
            pRender2D.DrawTexture(pTexLight, ref pos, ref dim, 0, E_EFFECT2D_FLAGS.EF_BLEND);

            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_MASK);
            pos = new TPoint2();
            pRender2D.DrawTexture(pTexTarget[2], ref pos, ref ScreenSize, 0, E_EFFECT2D_FLAGS.EF_BLEND |
                E_EFFECT2D_FLAGS.EF_FLIP_VERTICALLY);
            pRender2D.SetBlendMode(E_BLENDING_EFFECT.BE_NORMAL);
        }

        void Run()
        {
            if (Engine.GetEngine(DLL_PATH, out pEngineCore))
            {
                var win = new TEngineWindow(ScreenWidth, ScreenHeight, false);
                pEngineCore.InitializeEngine(IntPtr.Zero, AppCaption, ref win);

                pEngineCore.ConsoleVisible(false);

                DSubscriber dInit = Init;
                DSubscriber dFree = Free;
                DSubscriber dUpdate = Update;
                DSubscriber dRender = Render;
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, dInit, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, dFree, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, dUpdate, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, dRender, IntPtr.Zero);

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