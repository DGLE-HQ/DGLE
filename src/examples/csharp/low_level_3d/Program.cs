using System;
using DGLE;
using System.Windows.Forms;

namespace LowLevel3D
{
    public class Program
    {
        const string APP_CAPTION = "Low Level 3D";
        const string DLL_PATH = @"..\..\..\..\windows\DGLE.dll";
        const string RESOURCE_PATH = @"..\..\..\..\..\resources\";

        const uint SCREEN_WIDTH = 1024u;
        const uint SCREEN_HEIGHT = 768u;

        IEngineCore pEngineCore;
        ICoreRenderer pCoreRenderer;
        IRender3D pRender3D;

        ITexture pTexGrass, pTexTree1, pTexTree2, pTexTree3, pTexOwl, pTexZard;
        IMesh pMeshTree1, pMeshTree2, pMeshTree3;
        IModel pModelZard;
        IBitmapFont pFont;

        private bool owlGoLeft = false;
        private float owlX = -2f;

        private uint uiCounter, lol;

        // Float array with four vertices describing a plane in 3D space.
        // These four vertices will generate two triangles, we will use CRDM_TRIANGLE_STRIP mode to do it.

        readonly float[] c_afPlane = 
        {
	         //vertices
	        /*v1 = */  -0.5f,  -0.5f,  0f, /*v2 = */  0.5f,  -0.5f,  0f,
	        /*v3 = */  -0.5f,   0.5f,  0f, /*v4 = */  0.5f,   0.5f,  0f,
	        // normals
	        /*n1 = */  0f,  0f,  1f, /*n2 = */  0f,  0f,  1f,
	        /*n3 = */  0f,  0f,  1f, /*n4 = */  0f,  0f,  1f,
	        // texture vertices
	        /*t1 = */  0f,  1f, /*t2 = */  1f,  1f,
	        /*t3 = */  0f,  0f, /*t4 = */  1f,  0f
        };


        TDrawDataDesc desc;
        IntPtr PlaneDataPtr;

        void Init(IntPtr pParam)
        {
            IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_CORE_RENDERER, out pSubSys);
            pCoreRenderer = (ICoreRenderer)pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out pSubSys);
            IRender pRender = (IRender)pSubSys;
            pRender.GetRender3D(out pRender3D);
            TColor4 c = TColor4.ColorGray();
            pRender.SetClearColor(ref c);

            IResourceManager pResMan;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
            pResMan = (IResourceManager)pSubSys;

            IEngineBaseObject pBaseObj;

            const uint load_3d_flag = (uint)(E_TEXTURE_LOAD_FLAGS.TLF_FILTERING_ANISOTROPIC |
                       E_TEXTURE_LOAD_FLAGS.TLF_ANISOTROPY_4X | E_TEXTURE_LOAD_FLAGS.TLF_GENERATE_MIPMAPS);

            pResMan.Load(RESOURCE_PATH + "fonts\\Times_New_Roman_18_Bold.dft", out pBaseObj,
                (uint)E_BITMAP_FONT_LOAD_FLAGS.BFLF_GENERATE_MIPMAPS);
            pFont = (IBitmapFont)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "textures\\grass.jpg", out pBaseObj, load_3d_flag |
                (uint)E_TEXTURE_LOAD_FLAGS.TLF_COORDS_REPEAT /* cause we will tile this texture */);
            pTexGrass = (ITexture)pBaseObj;

            pResMan.Load(RESOURCE_PATH + "sprites\\cartoon_owl.png", out pBaseObj, load_3d_flag | (uint)E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            pTexOwl = (ITexture)pBaseObj;
            pTexOwl.SetFrameSize(48, 128);

            pResMan.Load(RESOURCE_PATH + "meshes\\trees\\tree_1.png", out pBaseObj, load_3d_flag | (uint)E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            pTexTree1 = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "meshes\\trees\\tree_1.dmd", out pBaseObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH);
            pMeshTree1 = (IMesh)pBaseObj;

            pResMan.Load(RESOURCE_PATH + "meshes\\trees\\tree_2.png", out pBaseObj, load_3d_flag | (uint)E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            pTexTree2 = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "meshes\\trees\\tree_2.dmd", out pBaseObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH);
            pMeshTree2 = (IMesh)pBaseObj;

            pResMan.Load(RESOURCE_PATH + "meshes\\trees\\tree_3.png", out pBaseObj, load_3d_flag | (uint)E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            pTexTree3 = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "meshes\\trees\\tree_3.dmd", out pBaseObj, (uint)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH);
            pMeshTree3 = (IMesh)pBaseObj;

            pResMan.Load(RESOURCE_PATH + "meshes\\zard\\zard_diff.dds", out pBaseObj, load_3d_flag | (uint)E_TEXTURE_LOAD_FLAGS.TLF_COORDS_CLAMP);
            pTexZard = (ITexture)pBaseObj;
            pResMan.Load(RESOURCE_PATH + "meshes\\zard\\zard_walk.dmd", out pBaseObj, 0);
            pModelZard = (IModel)pBaseObj;

            // add some fog to the scene
            pRender3D.SetFogColor(ref c);
            pRender3D.SetLinearFogBounds(1.5f, 4f);
            pRender3D.ToggleFog(true);

            PlaneDataPtr = Unmanaged.New<float>(c_afPlane.Length);
            PlaneDataPtr.Copy(c_afPlane);
            
            desc = new TDrawDataDesc
            (
               PlaneDataPtr,
               12 * sizeof(float),
               24 * sizeof(float),
               false
            );

        }

        void Free(IntPtr pParam)
        {
            PlaneDataPtr.Free();
        }

        void Update(IntPtr pParam)
        {
            if (owlX > 2.5f)
                owlGoLeft = true;
            else
                if (owlX < -2.5f)
                    owlGoLeft = false;
                    
            owlX += (owlGoLeft ? -1f : 1f) * 0.0075f;

            ++uiCounter;
        }

        void Render(IntPtr pParam)
        {
            // Attention!
            // The main thing you should keep in the mind is that matrix multiplication order should be
            // reversed when passing matrix to pRender3D->SetMatrix or pRender3D->MultMatrix methods.

            // camera setup
            // here and below matrix multiplication order is reversed
            TMatrix4x4 set = TMatrix4x4.MatrixRotate((float)(Math.Sin(uiCounter / 150f) * 65f), new TPoint3(0f, 1f, 0f)) *
                    TMatrix4x4.MatrixRotate(30f, new TPoint3(1f, 0f, 0f)) *
                    TMatrix4x4.MatrixTranslate(new TPoint3(0f, 0f, -2.5f)) *
                    TMatrix4x4.MatrixIdentity;

            pRender3D.SetMatrix(ref set);

            // Draw entire scene.

            // draw tiled grass floor

            // Some low-level things will be shown below, mainly for education purpose.
            // Of course, there is a way to do the same thing without using low-level API.

            pRender3D.PushMatrix(); // save current matrix

            // multiplicates current matrix with given one
            TMatrix4x4 mult = TMatrix4x4.MatrixScale(new TPoint3(8f, 8f, 8f)) *
                              TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
            pRender3D.MultMatrix(ref mult);

            set = TMatrix4x4.MatrixScale(new TPoint3(8f, 8f, 8f));

            pCoreRenderer.SetMatrix(ref set, E_MATRIX_TYPE.MT_TEXTURE); // a simple way to tile texture

            // Here is the way for instant rendering of any custom geometry.

            pTexGrass.Bind(0); // current texture setup
            pRender3D.Draw(ref desc, E_CORE_RENDERER_DRAW_MODE.CRDM_TRIANGLE_STRIP, 4);

            set = TMatrix4x4.MatrixIdentity;

            pCoreRenderer.SetMatrix(ref set, E_MATRIX_TYPE.MT_TEXTURE); // return texture matrix to its normal state

            pRender3D.PopMatrix(); // return previous matrix

            // Ok, that's all with low-level things in this example.

            // turn off backface culling because of trees leaves (they will look better) and sprites rendering (we want txt and owl to be visible from both sides)
            pRender3D.ToggleBackfaceCulling(false);

            // draw some trees

            // turn on alpha test for correct rendering of trees leaves
            pRender3D.ToggleAlphaTest(true);

            // precalculate similar for all trees transformation part to make it faster
            TMatrix4x4 tree_rotate_and_scale = TMatrix4x4.MatrixScale(new TPoint3(2f, 2f, 2f)) *
                TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));

            // some copy-pasted code for each tree to draw all of them

            pRender3D.PushMatrix();
            mult = tree_rotate_and_scale * TMatrix4x4.MatrixTranslate(new TPoint3(2.3f, 0f, 1.2f));
            pRender3D.MultMatrix(ref mult);
            pTexTree1.Bind(0); // way to set current texture
            pMeshTree1.Draw();
            pRender3D.PopMatrix();

            pRender3D.PushMatrix();
            mult = tree_rotate_and_scale * TMatrix4x4.MatrixTranslate(new TPoint3(-2.15f, 0f, -1.75f));
            pRender3D.MultMatrix(ref mult);
            pTexTree1.Bind(0);
            pMeshTree1.Draw();
            pRender3D.PopMatrix();

            pRender3D.PushMatrix();
            mult = tree_rotate_and_scale * TMatrix4x4.MatrixTranslate(new TPoint3(-0.5f, 0f, -1f));
            pRender3D.MultMatrix(ref mult);
            pTexTree2.Bind(0);
            pMeshTree2.Draw();
            pRender3D.PopMatrix();

            pRender3D.PushMatrix();
            mult = tree_rotate_and_scale * TMatrix4x4.MatrixTranslate(new TPoint3(0.75f, 0f, 0.1f));
            pRender3D.MultMatrix(ref mult);
            pTexTree2.Bind(0);
            pMeshTree2.Draw();
            pRender3D.PopMatrix();

            pRender3D.PushMatrix();
            mult = tree_rotate_and_scale * TMatrix4x4.MatrixTranslate(new TPoint3(0.5f, 0f, -1.5f));
            pRender3D.MultMatrix(ref mult);
            pTexTree3.Bind(0);
            pMeshTree3.Draw();
            pRender3D.PopMatrix();

            pRender3D.PushMatrix();
            mult = tree_rotate_and_scale * TMatrix4x4.MatrixTranslate(new TPoint3(-0.75f, 0f, 0.25f));
            pRender3D.MultMatrix(ref mult);
            pTexTree3.Bind(0);
            pMeshTree3.Draw();
            pRender3D.PopMatrix();

            pRender3D.ToggleAlphaTest(false); // we don't need alphatest anymore

            // we use blending for further font and sprite rendering
            pRender3D.ToggleBlending(true);

            // draw text
            pRender3D.PushMatrix();

            const string txt = "The very basics of the 3D graphics.";
            const float txt_scale = 0.005f;

            uint w, h;
            pFont.GetTextDimensions(txt, out w, out h);

            mult = TMatrix4x4.MatrixScale(new TPoint3(txt_scale, txt_scale, txt_scale)) *
                // tex is drawing in rather huge coordinates, so we will downscale it
                             TMatrix4x4.MatrixTranslate(new TPoint3(0f, h / 2f * txt_scale, 1.25f)); // move text up on half of it's height and little forward

            pRender3D.MultMatrix(ref mult);

            TColor4 c = TColor4.ColorOfficialOrange();
            pRender3D.SetColor(ref c); // set current color

            pFont.Draw3D(txt);

            c = TColor4.ColorWhite();
            pRender3D.SetColor(ref c); // return color back to white

            pRender3D.PopMatrix();

            // draw owl animated sprite as billboard
            pRender3D.PushMatrix();

            // here is a way to make a billboard sprite in 3D
            TMatrix4x4 cur_matrix = new TMatrix4x4();
            pRender3D.GetMatrix(out cur_matrix); // first we must get current matrix

            // now we set new current matrix
            TMatrix4x4 matr = TMatrix4x4.MatrixScale(new TPoint3((owlGoLeft ? -1f : 1f), 1f, 1f)) *
                // the way to mirror sprite picture
                              TMatrix4x4.MatrixBillboard( // this function will remove any rotation from given matrix
                              TMatrix4x4.MatrixScale(new TPoint3(0.35f, 0.35f, 0.35f)) *
                              TMatrix4x4.MatrixTranslate(new TPoint3(owlX, 1f, -0.35f)) *
                              cur_matrix);
            pRender3D.SetMatrix(ref matr);

            pTexOwl.Draw3D((uiCounter / 2) % 15); // fast way to render texture as square plane with size 1.0

            pRender3D.PopMatrix();

            pRender3D.ToggleBlending(false);
            pRender3D.ToggleBackfaceCulling(true); // turn backface culling back on here

            // draw monster
            pRender3D.PushMatrix();

            TPoint3 extents = new TPoint3();
            pModelZard.GetExtents(out extents); // retrieves dimensions of the model (ex. extents.x is a half of the models width)

            // here we will let monster go circles
            const float speed = 1.2f;
            float angle = uiCounter / 75f;
            TPoint3 circle_center = new TPoint3(1f, 0f, 0.25f);

            mult = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f)) *
                TMatrix4x4.MatrixRotate(angle * 180f / (float)Math.PI, new TPoint3(0f, -1f, 0f)) *
                TMatrix4x4.MatrixTranslate(new TPoint3((float)(Math.Cos(angle) * speed), extents.z,
                    (float)(Math.Sin(angle) * speed)) + circle_center);

            pRender3D.MultMatrix(ref mult);

            pTexZard.Bind(0);

            // this model uses frame based animation, we just need to switch between meshes
            uint meshes_count;
            pModelZard.MeshsCount(out meshes_count);
            pModelZard.DrawMesh(uiCounter % meshes_count);

            pRender3D.PopMatrix();
        }
        
        void Run()
        {
            if (Engine.GetEngine(DLL_PATH, out pEngineCore))
            {
                TEngineWindow win = new TEngineWindow(SCREEN_WIDTH, SCREEN_HEIGHT, false, true, E_MULTISAMPLING_MODE.MM_4X, E_ENG_WINDOW_FLAGS.EWF_ALLOW_SIZEING);
                pEngineCore.InitializeEngine(IntPtr.Zero, APP_CAPTION, ref win, 33, E_ENGINE_INIT_FLAGS.EIF_DEFAULT);

                pEngineCore.ConsoleVisible(true);

                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, new DSubscriber(Init), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_FREE, new DSubscriber(Free), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, new DSubscriber(Update), IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, new DSubscriber(Render), IntPtr.Zero);

                pEngineCore.StartEngine();
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
