using System;
using DGLE;
using System.Windows.Forms;

namespace Sample_3Din2D
{
    class Program
    {
        #region various initialization 
        const string APP_CAPTION = "Sample 3DIn2D";
        const string DLL_PATH = @"..\..\DGLE.dll";

        const string RESOURCE_PATH = @"..\..\..\resources\";
        
        const uint SCREEN_WIDTH =1024u;
        const uint SCREEN_HEIGHT = 768u;

        IEngineCore pEngineCore = null;
        IRender pRender = null;
        static IRender2D pRender2D = null;
        ISoundSample pSound = null;
        ISoundChannel pSoundChannel = null;
        IEngineSubSystem p_sub_sys = null;
        IResourceManager pResMan = null;
        IInput pInput = null;

        int counter = 1;
        public static Random rnd = new Random();
        TPoint2 stMouseOnScreen, stMouseInCamera;
        TMouseStates mouse;

        // delegates, they are here to be not collected by GC
        DSubscriber DInit = null;
        DSubscriber DFree = null;
        DSubscriber DUpdate = null;
        DSubscriber DRender = null;

        // declarations for work with texs and meshes
        ITexture[] pTextures;
        ITexture[] pShadows;
        IMesh[] pMeshes;
        const int TexCount = 9;
        const int MeshCount = 5;
        const int ShadowCount = 6;
        string[] TexNames = new string[TexCount] 
        {
            "meshes\\trees\\tree_1.png",
            "meshes\\trees\\tree_2.png",
            "meshes\\trees\\tree_3.png",
            "meshes\\house\\tex_diffuse.jpg",
            "meshes\\copter\\copter.png",
            "meshes\\copter\\propeller.png",
            "sprites\\zombie.png",
            "grass.jpg",
            "textures\\stone.tga"
        };

        string[] MeshNames = new string[MeshCount] 
        {
            "meshes\\trees\\tree_1.dmd",
            "meshes\\trees\\tree_2.dmd",
            "meshes\\trees\\tree_3.dmd",
            "meshes\\house\\house.dmd",
            "meshes\\copter\\copter.dmd"
        };
        #endregion
        
        // encapsulates data for helicopter "physics"
        class Copter
        {
            public float fCopterAngle;
            public TPoint2 stCopterPos;
            public TPoint2 vint_pos;
            public TPoint2 vint_dim;
            public MyMesh CopterMesh;
            public ITexture RotorTex;
            public ITexture RotorShadow;

            public Copter(MyMesh copterMesh, ITexture rotorTex, ITexture rotorShadow)
            {
                stCopterPos = new TPoint2(400, 200);
                fCopterAngle = 0f;
                CopterMesh = copterMesh;
                RotorTex = rotorTex;
                RotorShadow = rotorShadow;
                vint_dim = new TPoint2(400f, 400f);
            }

            public void Update()
            {
                fCopterAngle += 0.5f;
                double DegAng = TO_RAD(fCopterAngle);
                stCopterPos += new TPoint2((float)Math.Cos(DegAng) * 4, (float)Math.Sin(DegAng) * 6);
                CopterMesh.P2coord = stCopterPos;
                CopterMesh.Angle = fCopterAngle;
                vint_pos = new TPoint2((float)(stCopterPos.x - 200f + Math.Cos(TO_RAD(fCopterAngle)) * 80f), (float)(stCopterPos.y - 200f + Math.Sin(TO_RAD(fCopterAngle)) * 80f));
            }

            public void Render(float counter)
            {
                CopterMesh.Draw();                
                pRender2D.DrawTexture(RotorTex, ref vint_pos, ref vint_dim, counter * 25f);
                // make blinky rotor shadow
                TColor4 col = TColor4.ColorBlack((byte)(counter % 3 == 2 ? 64 : 16));
                pRender2D.SetColorMix(ref col);
                TPoint2 p1 = vint_pos + new TPoint2(100f, 100f);
                pRender2D.DrawTexture(RotorShadow, ref p1, ref vint_dim, 0f, (E_EFFECT2D_FLAGS.EF_COLOR_MIX | E_EFFECT2D_FLAGS.EF_BLEND));
            }
        }

        class Zombie
        {
            public TPoint2 stZombiePos = new TPoint2(rnd.Next(100), rnd.Next(400));
            public float fZombieAngle = 0f, fZombieAnimFrame = 0f;
            public bool bFrameForward = true;
            public ITexture tex;
            TPoint2 pd = new TPoint2(128, 128);

            public Zombie(ITexture tex)
            {
                this.tex = tex;
            }
            public void Update(TPoint2 stMouseInCamera)
            {
                if (bFrameForward)
                    fZombieAnimFrame += 0.3f;
                else
                    fZombieAnimFrame -= 0.3f;

                if (bFrameForward && (int)fZombieAnimFrame == 15)
                    bFrameForward = false;
                else
                    if (!bFrameForward && (int)fZombieAnimFrame == 0)
                        bFrameForward = true;
                fZombieAngle = TO_DEG((float)Math.Atan2(stMouseInCamera.y - (stZombiePos.y + 64f), stMouseInCamera.x - (stZombiePos.x + 64f)));

                stZombiePos.x += (float)Math.Cos(TO_RAD(fZombieAngle)) * 0.75f;
                stZombiePos.y += (float)Math.Sin(TO_RAD(fZombieAngle)) * 0.75f;
            }

            public void Render()
            {
                TPoint2 shd_pos= stZombiePos + new TPoint2(10f, 10f);
                pRender2D.DrawTextureSprite(tex, ref shd_pos, ref pd, (uint)fZombieAnimFrame, fZombieAngle + 90f, (E_EFFECT2D_FLAGS.EF_COLOR_MIX | E_EFFECT2D_FLAGS.EF_BLEND));
                pRender2D.DrawTextureSprite(tex, ref stZombiePos, ref pd, (uint)fZombieAnimFrame, fZombieAngle + 90f);
            }
        }

        // encapsulates job on 3d-objects
        class MyMesh
        {
            public TPoint2 P2coord;
            public TPoint3 P3Dim;
            public TPoint3 P3Axis;
            public float Angle;
            public IMesh Mesh;
            public ITexture Tex;
            public ITexture ShadowTex;
            public bool bClearDepthBuf;
            int shift;

            public MyMesh(IMesh Amesh, ITexture Atex, ITexture ShadowTex, TPoint2 Acoord, TPoint3 ADim, int shift,  float AAngle = 0, bool bClearDepthBuf = false)
            {
                Mesh = Amesh;
                Tex = Atex;
                this.ShadowTex = ShadowTex;
                P2coord = Acoord;
                P3Dim = ADim;
                P3Axis = new TPoint3(0, 0, 1);
                Angle = AAngle;
                this.bClearDepthBuf = bClearDepthBuf;
                this.shift = shift;
            }
            public void Draw()
            {
                TPoint2 p1 = P2coord - new TPoint2(shift, shift);
                TPoint2 pdim = new TPoint2(P3Dim.x, P3Dim.y);
                pRender2D.DrawTexture(ShadowTex, ref p1, ref pdim, Angle, (E_EFFECT2D_FLAGS.EF_FLIP_VERTICALLY | E_EFFECT2D_FLAGS.EF_COLOR_MIX | E_EFFECT2D_FLAGS.EF_BLEND));
                pRender2D.DrawMesh(Mesh, Tex, ref P2coord, ref P3Dim, ref P3Axis, Angle, E_EFFECT2D_FLAGS.EF_DEFAULT, true, 90, bClearDepthBuf);
            }

        }

        // actual ingame objects
        Copter copter;
        MyMesh[] MyMeshes;
        Zombie zombie;
        


        void Init(IntPtr pParam)
        {
            // get subsystems
            
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out p_sub_sys);
            pResMan = (IResourceManager)p_sub_sys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_INPUT, out p_sub_sys);
            pInput = (IInput)p_sub_sys;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out p_sub_sys);
            pRender = (IRender)p_sub_sys;
            pRender.GetRender2D(out pRender2D);

            // create arrays
            IEngineBaseObject pObj = null;
            pTextures = new ITexture[TexCount];
            pShadows = new ITexture[ShadowCount];
            pMeshes = new IMesh[MeshCount];

            // loading data
            pResMan.Load(RESOURCE_PATH + "sounds\\helicopter.wav", out pObj, 0);
            pSound = (ISoundSample)pObj;
            pSound.PlayEx(out pSoundChannel, E_SOUND_SAMPLE_PARAMS.SSP_LOOPED);

            for (int i = 0; i < TexCount; i++)
            {
                uint flags = TexNames[i].Contains("grass") ? (uint)(E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR | E_TEXTURE_LOAD_FLAGS.TLF_COORDS_REPEAT) : (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D;
                pResMan.Load(RESOURCE_PATH + TexNames[i], out pObj, flags);
                pTextures[i] = (ITexture)pObj;
            }

            pTextures[6].SetFrameSize(256, 256); // zombie sprite splitting

            for (int i = 0; i < MeshCount; i++)
            {
                pResMan.Load(RESOURCE_PATH + MeshNames[i], out pObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH);
                pMeshes[i] = (IMesh)pObj;
            }

            // render shadows
            for (int i = 0; i < MeshCount; i++)
            {
                RenderMeshToTexture(out pShadows[i], pMeshes[i], pTextures[i]);
            }

            // render rotor shadow
            pResMan.CreateTexture(out pShadows[5], null, 256, 256, E_TEXTURE_DATA_FORMAT.TDF_RGBA8, E_TEXTURE_CREATION_FLAGS.TCF_DEFAULT, E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR);
            pRender.SetRenderTarget(pShadows[5]);
            TPoint2 coords = new TPoint2(128f, 128f);
            TColor4 col = TColor4.ColorWhite();
            pRender2D.DrawCircle(ref coords, 100, 64, ref col, E_PRIMITIVE2D_FLAGS.PF_FILL);
            pRender.SetRenderTarget(null);


            // gather, fill and init 3d-objects data
            MyMeshes = new MyMesh[8] 
            {
                new MyMesh(pMeshes[0], pTextures[0], pShadows[0], new TPoint2( 900f, 500f), new TPoint3(400f, 400f, 500f), 225),
                new MyMesh(pMeshes[0], pTextures[0], pShadows[0], new TPoint2(-250f, 300f), new TPoint3(400f, 400f, 500f), 225),
                new MyMesh(pMeshes[0], pTextures[0], pShadows[0], new TPoint2( 800f, 200f), new TPoint3(400f, 400f, 400f), 225),
                new MyMesh(pMeshes[1], pTextures[1], pShadows[1], new TPoint2(   0f, 450f), new TPoint3(300f, 300f, 400f), 175),
                new MyMesh(pMeshes[1], pTextures[1], pShadows[1], new TPoint2(  50f, 750f), new TPoint3(300f, 300f, 300f), 175),
                new MyMesh(pMeshes[2], pTextures[2], pShadows[2], new TPoint2( 500f, 150f), new TPoint3(400f, 400f, 500f), 225),
                new MyMesh(pMeshes[3], pTextures[3], pShadows[3], new TPoint2( 180f, 150f), new TPoint3(400f, 400f, 600f), 200),
                new MyMesh(pMeshes[3], pTextures[3], pShadows[3], new TPoint2( 600f, 550f), new TPoint3(400f, 400f, 600f), 200, 90)
            };

            copter = new Copter(new MyMesh(pMeshes[4], pTextures[4], pShadows[4], new TPoint2(), new TPoint3(600, 600, 600), 200, 0, true), pTextures[5], pShadows[5]);
            zombie = new Zombie(pTextures[6]);
        }

        // This function creates texture and renders models shadow to it.
        void RenderMeshToTexture(out ITexture pTex, IMesh pMesh, ITexture pMeshTex)
        {
            pResMan.CreateTexture(out pTex, null, 256, 256, E_TEXTURE_DATA_FORMAT.TDF_RGBA8, E_TEXTURE_CREATION_FLAGS.TCF_DEFAULT, E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_BILINEAR);
            pRender.SetRenderTarget(pTex);
            TPoint2 coords = new TPoint2(128f, 128f);
            TPoint3 dims = new TPoint3(256f, 256f, 1f);
            TPoint3 axis = new TPoint3(0f, 0f, 1f);
            pRender2D.DrawMesh(pMesh, pMeshTex, ref coords, ref dims, ref axis);
            pRender.SetRenderTarget(null); // switch back to on-screen drawing
        }

        void Free(IntPtr pParam)
        {
            pSoundChannel.Unaquire();

            //It's better(not necessary) to free resources on exit
            for (int i = 0; i < TexCount; i++)
            {
                if (pTextures[i] != null) pTextures[i].Free();
            }
            for (int i = 0; i < MeshCount; i++)
            {
                if (pMeshes[i] != null) pMeshes[i].Free();
            }

            for (int i = 0; i < ShadowCount; i++)
            {
                if (pShadows[i] != null) pShadows[i].Free();
            }
        }

        static float TO_RAD(float angle)
        {
            return (float)Math.PI * angle / 180f;
        }

        static float TO_DEG(float angle)
        {
            return angle * (180f / (float)Math.PI);
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

            pInput.GetMouseStates(out mouse);
            stMouseOnScreen = new TPoint2((float)mouse.iX, (float)mouse.iY);
            // convert our on-screen mouse coordinates to camera space
            
            // process zombie movement and its sprite animation
            zombie.Update(stMouseInCamera);
            // move copter
            copter.Update();

            // Choper flies on screen between -60 and 856 coords so here's a way to convert its coords to [-100,100] range
            pSoundChannel.SetPan((int)Math.Round((copter.stCopterPos.x + 60 - 460) * 0.2));

            counter++;
        }

        void Render(IntPtr pParam)
        {
            pRender2D.Begin2D();

            TPoint2 pc;

            pc = new TPoint2((float)(350 + Math.Cos(counter / 200.0) * 200), (float)(350 + Math.Sin(counter / 200.0) * 80));
            TPoint2 ps = new TPoint2(1, 1);
            // make floating camera
            pRender2D.SetCamera(ref pc,//camera center
                (float)Math.Sin(counter / 50.0) * 15,//camera angle
                ref ps//camera scale
                );
pRender2D.ProjectScreenToCamera(ref stMouseOnScreen, out stMouseInCamera);

            pc = new TPoint2(-750f, -750f);
            ps = new TPoint2(2000f, 2000f);
            pRender2D.DrawTexture(pTextures[7], ref pc, ref ps, 0f, E_EFFECT2D_FLAGS.EF_TILE_TEXTURE);

             //draw stone layer with per vertex blending
            TColor4 white = TColor4.ColorWhite();
            TColor4 col = TColor4.ColorWhite(0);
            TPoint2 p1 = new TPoint2(0f, 225f);
            uint x, y;
            pTextures[8].GetDimensions(out x, out y);
            TPoint2 pdim = new TPoint2((float)x, (float)y);
            pRender2D.SetVerticesColors(ref col, ref white, ref white, ref col);
            pRender2D.DrawTexture(pTextures[8], ref p1, ref pdim, 0f, (E_EFFECT2D_FLAGS.EF_VERTICES_COLORS | E_EFFECT2D_FLAGS.EF_BLEND));

            pRender2D.SetVerticesColors(ref white, ref col, ref col, ref white);
            p1 = new TPoint2(0f + 256f, 225f);
            pRender2D.DrawTexture(pTextures[8], ref p1, ref pdim, 0f, (E_EFFECT2D_FLAGS.EF_VERTICES_COLORS | E_EFFECT2D_FLAGS.EF_BLEND));

            pRender2D.SetVerticesColors(ref white, ref col, ref col, ref col);
            p1 = new TPoint2(0f + 256f, 225f + 256f);
            pRender2D.DrawTexture(pTextures[8], ref p1, ref pdim, 0f, (E_EFFECT2D_FLAGS.EF_VERTICES_COLORS | E_EFFECT2D_FLAGS.EF_BLEND));

            pRender2D.SetVerticesColors(ref col, ref white, ref col, ref col);
            p1 = new TPoint2(0f, 225f + 256f);
            pRender2D.DrawTexture(pTextures[8], ref p1, ref pdim, 0f, (E_EFFECT2D_FLAGS.EF_VERTICES_COLORS | E_EFFECT2D_FLAGS.EF_BLEND));

            //// set color mix to semi-transparent black for object shadows
            col = TColor4.ColorBlack(128);
            pRender2D.SetColorMix(ref col);

            for (int i = 0; i < MyMeshes.Length; i++) // all meshes with shadows except copter
            {
                MyMeshes[i].Draw();
            }

            zombie.Render();

            copter.Render((float) counter);

            pRender2D.End2D();
        }

        void Run()
        {
            if (Engine.GetEngine(DLL_PATH, out pEngineCore))
            {
                TEngineWindow win = new TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, true, E_MULTISAMPLING_MODE.MM_4X, E_ENG_WINDOW_FLAGS.EWF_ALLOW_SIZEING);
                pEngineCore.InitializeEngine(IntPtr.Zero, APP_CAPTION, ref win, 33, E_ENGINE_INIT_FLAGS.EIF_DEFAULT);
                // You can do some initialization here

                pEngineCore.ConsoleVisible(true);

                DInit = new DSubscriber(Init);
                DFree = new DSubscriber(Free);
                DUpdate = new DSubscriber(Update);
                DRender = new DSubscriber(Render);

                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, DInit, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, DFree, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, DUpdate, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, DRender, IntPtr.Zero);

                pEngineCore.StartEngine(); //Entering engine loop
            }
            else
                MessageBox.Show("Couldn't load \"DGLE.dll\"!", APP_CAPTION, MessageBoxButtons.OK, MessageBoxIcon.Error);
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