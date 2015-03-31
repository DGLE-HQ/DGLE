using System;
using DGLE;
using System.Windows.Forms;

namespace CoreRendererAndOpenGL
{
    // OpenGL part is from TAO in adaptation of sinclairzx81
    // https://github.com/sinclairzx81/vector/tree/master/vector.gpu/OpenGL (all files except Glu.cs for this demo)
    using Vector.Gpu.OpenGL;
    
    class Program	{

		const string AppCaption = "CoreRenderer and OpenGL";
        const string DllPath = @"..\..\..\..\windows\DGLE.dll";
	    const string ResPath = @"..\..\..\..\..\resources\";

		const uint ScreenWidth = 800u;
		const uint ScreenHeight = 600u;
		
		IEngineCore pEngineCore;
        ICoreRenderer pCoreRenderer;
		
		ITexture pTex;
	    IMesh pMesh;

        private uint uiCounter = 0;
		
		void Init(IntPtr pParam)
		{
			IEngineSubSystem pSubSys;

            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_CORE_RENDERER, out pSubSys);
            pCoreRenderer = (ICoreRenderer)pSubSys;

            E_CORE_RENDERER_TYPE type;
            pCoreRenderer.GetRendererType(out type);
            if (type != E_CORE_RENDERER_TYPE.CRT_OPENGL_LEGACY)
                pEngineCore.WriteToLogEx("This example will work only with Legacy OpenGL renderer!", E_LOG_TYPE.LT_FATAL, "", 0);

		    IResourceManager pResMan;
            pEngineCore.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_RESOURCE_MANAGER, out pSubSys);
			pResMan = (IResourceManager)pSubSys;
			
			IEngineBaseObject pBaseObj = null;

            pResMan.Load(ResPath + "meshes\\torus.dmd", out pBaseObj, (int)E_MESH_MODEL_LOAD_FLAGS.MMLF_FORCE_MODEL_TO_MESH);
            pMesh = (IMesh)pBaseObj;
            pResMan.Load(ResPath + "textures\\stone.tga", out pBaseObj, (int)E_TEXTURE_LOAD_FLAGS.TEXTURE_LOAD_DEFAULT_3D);
			pTex = (ITexture)pBaseObj;            
		}
		
		void Free(IntPtr pParam)
		{
			
		}
		
		void Update(IntPtr pParam)
		{
            ++uiCounter;
		}
		
		void Render(IntPtr pParam)
		{
	        // retrieving OpenGL texture
	        ICoreTexture pCoreTex;
	        pTex.GetCoreTexture(out pCoreTex);
	        IOpenGLTextureContainer pGLTex;
            IBaseRenderObjectContainer pGLObj;
	        pCoreTex.GetBaseObject(out pGLObj);
            pGLTex = (IOpenGLTextureContainer)pGLObj;

	        uint GLTex; // OpenGL texture Id
            pGLTex.GetTexture(out GLTex);

            Gl.glEnable(Gl.GL_LIGHTING);
            Gl.glEnable(Gl.GL_LIGHT0);

            Gl.glBindTexture(Gl.GL_TEXTURE_2D, GLTex);

            Gl.glPushMatrix();

            Gl.glTranslatef(0f, 0f, -1.5f);
            Gl.glRotatef(uiCounter / 2f, 0.6f, 0.2f, 0.3f);

	        // We can also acess geometry buffer via pMesh->GetGeometryBuffer(p_geom_buff) and then p_geom_buff->GetBaseObject(...).
	        // But it is easier to use engine.
	        pMesh.Draw();

            Gl.glPopMatrix();

            Gl.glBindTexture(Gl.GL_TEXTURE_2D, 0);

            Gl.glDisable(Gl.GL_LIGHTING);
            Gl.glDisable(Gl.GL_LIGHT0);

	        // If you've corrupted engine pipeline somehow, you may want to invalidate state manager.
	        // And remember that IRender3D can also cache some states.
	        //pCoreRenderer->InvalidateStateFilter();
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