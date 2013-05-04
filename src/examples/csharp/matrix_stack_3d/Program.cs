using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

using DGLE;

namespace matrix_stack_3d
{
    class Program
    {
        const string APP_CAPTION = "MatrixStack3D";
        const string DLL_PATH = @"..\..\..\..\DGLE.dll";
        const string RESOURCE_PATH = @"..\..\..\..\..\resources\";
        const uint SCREEN_WIDTH = 1280u;
        const uint SCREEN_HEIGHT = 800u;

        IEngineCore pEngineCore = null;
        IRender3D pRender3D = null;
        IRender p_render = null;
        ITexture pTexFloor = null;
        ITexture pTexLight = null;
        IModel pMdlDesk = null;
        IModel pMdlLamp = null;
        IModel pMdlChair = null;
        IModel pMdlMusicBox = null;
        IModel pSnowGlobe = null;
        IModel pModelChurch = null;
        ILight pLightDirect = null;
        ILight pLightSpot = null;
        TTransformStack transform;

        uint uiCounter = 1500;

        DSubscriber DInit = null;
        DSubscriber DProcess = null;
        DSubscriber DRender = null;
        Random rand;

        void Init(IntPtr pParam)
        {
            TMatrix4x4 mat = TMatrix4x4.MatrixIdentity;
            transform = new TTransformStack(ref mat);
            rand = new Random();

            IEngineSubSystem p_sub_sys = null;

            IResourceManager p_res_man = null;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out p_sub_sys);
            p_res_man = (IResourceManager)p_sub_sys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RENDER, out p_sub_sys);
            p_render = (IRender)p_sub_sys;
            p_render.GetRender3D(out pRender3D);
            IEngineBaseObject p_obj = null;
            p_res_man.Load(RESOURCE_PATH + "textures\\floor.dds", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            pTexFloor = (ITexture)p_obj;
            p_res_man.Load(RESOURCE_PATH + "sprites\\light.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_2D);
            pTexLight = (ITexture)p_obj;

            // some global lighting

            pRender3D.ToggleLighting(true);
            TColor4 col = TColor4.ColorBlack();
            pRender3D.SetGlobalAmbientLighting(ref col); // turn off ambient lighting

            // setup lights

            // use single directional light to simulate ambient lighting
            p_res_man.CreateLight(out pLightDirect);
            pLightDirect.SetType(E_LIGHT_TYPE.LT_DIRECTIONAL);
            col = TColor4.ColorGray();
            pLightDirect.SetColor(ref col); // dim light
            TPoint3 p3 = new TPoint3(-0.5f, 0.5f, 0.75f);
            pLightDirect.SetDirection(ref p3);
            pLightDirect.SetEnabled(true);

            // Position is ignored for direction lights but is used by engine for debug drawing.
            // Use "rnd3d_draw_lights 1" console command to debug lights.
            p3 = new TPoint3(0f, 7.5f, 0f);
            pLightDirect.SetPosition(ref p3);

            // create light for the table-lamp
            p_res_man.CreateLight(out pLightSpot);
            pLightSpot.SetType(E_LIGHT_TYPE.LT_SPOT);
            col = TColor4.ColorYellow();
            pLightSpot.SetColor(ref col);
            pLightSpot.SetSpotAngle(100f);
            p3 = new TPoint3(0.15f, 0f, -1f);
            pLightSpot.SetDirection(ref p3);
            pLightSpot.SetEnabled(true);

            // create and setup materials and load models

            ITexture p_tex;
            IMaterial p_mat;

            // desk
            p_res_man.CreateMaterial(out p_mat);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\desk\\desk_diff.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            p_tex = (ITexture)p_obj;
            p_mat.SetDiffuseTexture(p_tex);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\desk\\desk.dmd", out p_obj, 0);
            pMdlDesk = (IModel)p_obj;
            pMdlDesk.SetModelMaterial(p_mat);

            // table-lamp
            p_res_man.CreateMaterial(out p_mat);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\table_lamp\\lamp_diff.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            p_tex = (ITexture)p_obj;
            p_mat.SetDiffuseTexture(p_tex);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\table_lamp\\lamp.dmd", out p_obj, 0);
            pMdlLamp = (IModel)p_obj;
            pMdlLamp.SetModelMaterial(p_mat);

            // chair
            p_res_man.CreateMaterial(out p_mat);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\chair\\chair_diff.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            p_tex = (ITexture)p_obj;
            p_mat.SetDiffuseTexture(p_tex);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\chair\\chair.dmd", out p_obj, 0);
            pMdlChair = (IModel)p_obj;
            pMdlChair.SetModelMaterial(p_mat);

            // music box
            p_res_man.CreateMaterial(out p_mat);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\music_box\\mbox_d.dds", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            p_tex = (ITexture)p_obj;
            p_mat.SetDiffuseTexture(p_tex);
            p_res_man.Load(RESOURCE_PATH + "meshes\\furniture\\music_box\\music_box.dmd", out p_obj, 0);
            pMdlMusicBox = (IModel)p_obj;
            pMdlMusicBox.SetModelMaterial(p_mat);

            // church
            p_res_man.Load(RESOURCE_PATH + "meshes\\church\\church.dmd", out p_obj, 0);
            pModelChurch = (IModel)p_obj;
            pModelChurch.SetModelMaterial(p_mat);

            p_res_man.CreateMaterial(out p_mat);
            col = TColor4.ColorSilver();
            p_mat.SetDiffuseColor(ref col);
            col = TColor4.ColorWhite();
            p_mat.SetSpecularColor(ref col);
            p_mat.SetShininess(25f);
            pModelChurch.SetMeshMaterial(0, p_mat);

            p_res_man.CreateMaterial(out p_mat);
            p_res_man.Load(RESOURCE_PATH + "meshes\\church\\church_roof.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            p_tex = (ITexture)p_obj;
            p_mat.SetDiffuseTexture(p_tex);
            pModelChurch.SetMeshMaterial(1, p_mat);

            p_res_man.CreateMaterial(out p_mat);
            p_res_man.Load(RESOURCE_PATH + "meshes\\church\\church_main.jpg", out p_obj, (uint)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
            p_tex = (ITexture)p_obj;
            p_mat.SetDiffuseTexture(p_tex);
            pModelChurch.SetMeshMaterial(2, p_mat);

            // snow globe
            p_res_man.Load(RESOURCE_PATH + "meshes\\snow_globe.dmd", out p_obj, 0);
            pSnowGlobe = (IModel)p_obj;
            p_res_man.CreateMaterial(out p_mat);
            col = TColor4.ColorWhite();
            p_mat.SetDiffuseColor(ref col);
            pSnowGlobe.SetMeshMaterial(0, p_mat);

            p_res_man.CreateMaterial(out p_mat);
            col = TColor4.ColorBrown();
            p_mat.SetDiffuseColor(ref col);
            col = TColor4.ColorWhite();
            p_mat.SetSpecularColor(ref col);
            p_mat.SetShininess(25f);
            pSnowGlobe.SetMeshMaterial(2, p_mat);

            p_res_man.CreateMaterial(out p_mat);
            col = TColor4.ColorWhite(100);
            p_mat.SetDiffuseColor(ref col);
            col = TColor4.ColorWhite();
            p_mat.SetSpecularColor(ref col);
            p_mat.SetShininess(50f);
            p_mat.SetBlending(true, E_BLENDING_EFFECT.BE_NORMAL);

            // When material with blending is set model will sort mesh order for meshes with blending to be the last.
            pSnowGlobe.SetMeshMaterial(1, p_mat);

            // We will use black fog to simulate darkness.
            col = TColor4.ColorBlack();
            pRender3D.SetFogColor(ref col);
            pRender3D.SetLinearFogBounds(12.5f, 20f);
            pRender3D.ToggleFog(true);

        }


        void Process(IntPtr pParam)
        {
            if (uiCounter % 25 == 0)
                pLightSpot.SetEnabled(rand.Next(100) < 75);
            ++uiCounter;
        }

        void Render(IntPtr pParam)
        {
            // camera setup
            TMatrix4x4 mat = TMatrix4x4.MatrixIdentity;
            transform.Clear(ref mat);
            mat = TMatrix4x4.MatrixTranslate(new TPoint3(0f, -4.5f, -4f - (float)Math.Abs(Math.Sin(uiCounter / 250f)) * 5f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(15f + (float)Math.Cos(uiCounter / 200f) * 10f, new TPoint3(1f, 0f, 0f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(uiCounter / 5f, new TPoint3(0f, 1f, 0f));
            transform.MultLocal(ref mat);

            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);

            // set light in the world coordinate system
            pLightDirect.Update();

            // draw floor

            p_render.Unbind(E_ENGINE_OBJECT_TYPE.EOT_MATERIAL); // unbind last material from previous frame

            for (int i = -5; i < 5; ++i)
                for (int j = -5; j < 5; ++j)
                {
                    transform.Push();
                    mat = TMatrix4x4.MatrixTranslate(new TPoint3(i * 5f, 0f, j * 5f));
                    transform.MultLocal(ref mat);
                    mat = TMatrix4x4.MatrixScale(new TPoint3(5f, 5f, 5f));
                    transform.MultLocal(ref mat);
                    mat = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
                    transform.MultLocal(ref mat);
                    mat = transform.Top;
                    pRender3D.SetMatrix(ref mat);
                    pTexFloor.Draw3D();

                    transform.Pop();
                }

            // draw desk

            TPoint3 desk_extents, extents;
            pMdlDesk.GetExtents(out desk_extents);
            TPoint3 desk_pos = new TPoint3(0f, desk_extents.z * 10f, 0f);

            transform.Push();
            mat = TMatrix4x4.MatrixTranslate(desk_pos);
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixScale(new TPoint3(10f, 10f, 10f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(180f, new TPoint3(0f, 0f, 1f));
            transform.MultLocal(ref mat);
            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);
            pMdlDesk.Draw();
            transform.Pop();

            // draw chair
            pMdlChair.GetExtents(out extents);
            transform.Push();
            mat = TMatrix4x4.MatrixTranslate(new TPoint3(-1.5f, extents.z * 5f, 4.5f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixScale(new TPoint3(5f, 5f, 5f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(65f, new TPoint3(0f, 0f, 1f));
            transform.MultLocal(ref mat);
            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);
            pMdlChair.Draw();
            transform.Pop();

            // draw music box
            pMdlMusicBox.GetExtents(out extents);
            transform.Push();
            mat = TMatrix4x4.MatrixTranslate(desk_pos + new TPoint3(-2.75f, extents.y * 6f, -1f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixScale(new TPoint3(3f, 3f, 3f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(25f, new TPoint3(0f, 0f, 1f));
            transform.MultLocal(ref mat);
            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);
            pMdlMusicBox.Draw();
            transform.Pop();

            // draw table-lamp
            pMdlLamp.GetExtents(out extents);
            transform.Push();
            mat = TMatrix4x4.MatrixTranslate(desk_pos + new TPoint3(3.75f, extents.z * 8f - 0.2f, -1f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixScale(new TPoint3(4f, 4f, 4f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(-150f, new TPoint3(0f, 0f, 1f));
            transform.MultLocal(ref mat);
            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);
            pMdlLamp.Draw();

            // here we set light position in the coordinate space of the lamp (object space)
            TPoint3 light_pos = new TPoint3(0.35f, 0f, extents.z - 0.15f);
            pLightSpot.SetPosition(ref light_pos);
            pLightSpot.Update();

            // draw light halo
            bool is_light_active;
            pLightSpot.GetEnabled(out is_light_active);

            if (is_light_active)
            {
                pRender3D.ToggleLighting(false);
                pRender3D.SetBlendMode(E_BLENDING_EFFECT.BE_ADD);
                pRender3D.ToggleBlending(true);
                TColor4 col = TColor4.ColorYellow(200);
                pRender3D.SetColor(ref col);
                mat = TMatrix4x4.MatrixTranslate(light_pos + new TPoint3(0f, 0f, -0.1f));
                transform.MultLocal(ref mat);
                mat = TMatrix4x4.MatrixScale(new TPoint3(0.25f, 0.25f, 0.25f));
                transform.MultLocal(ref mat);
                mat = TMatrix4x4.MatrixBillboard(transform.Top);
                pRender3D.SetMatrix(ref mat);
                pTexLight.Draw3D();
                pRender3D.ToggleLighting(true);
            }

            transform.Pop();

            // draw snow globe and church
            pSnowGlobe.GetExtents(out extents);
            transform.Push();
            mat = TMatrix4x4.MatrixTranslate(desk_pos + new TPoint3(1f, extents.z * 4f + 0.75f, 0.5f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixScale(new TPoint3(2f, 2f, 2f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixRotate(-90f, new TPoint3(1f, 0f, 0f));
            transform.MultLocal(ref mat);

            transform.Push();
            mat = TMatrix4x4.MatrixTranslate(new TPoint3(0f, 0f, 0.125f));
            transform.MultLocal(ref mat);
            mat = TMatrix4x4.MatrixScale(new TPoint3(0.7f, 0.7f, 0.7f));
            transform.MultLocal(ref mat);
            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);
            pModelChurch.Draw();
            transform.Pop();

            mat = transform.Top;
            pRender3D.SetMatrix(ref mat);
            pSnowGlobe.Draw();

            transform.Pop();

        }

        public void Run()
        {
            if (Engine.GetEngine(DLL_PATH, out pEngineCore))
            {
                TEngineWindow win = TEngineWindow.Default;
                pEngineCore.InitializeEngine(IntPtr.Zero, APP_CAPTION, ref win, 33u, E_ENGINE_INIT_FLAGS.EIF_DEFAULT);
                // You can do some initialization here

                pEngineCore.ConsoleVisible(true);

                DInit = Init;
                DProcess = Process;
                DRender = Render;

                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_INIT, DInit, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_UPDATE, DProcess, IntPtr.Zero);
                pEngineCore.AddProcedure(E_ENGINE_PROCEDURE_TYPE.EPT_RENDER, DRender, IntPtr.Zero);

                //
                pEngineCore.StartEngine(); //Entering engine loop
            }
            else
                MessageBox.Show("Couldn't find " + Path.GetFullPath(DLL_PATH) + "!");
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
