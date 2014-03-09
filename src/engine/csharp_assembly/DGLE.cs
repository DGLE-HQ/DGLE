/**
\file		DGLE.cs
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		XX.XX.2012 (c)Korotkov Andrey

\brief		Main DGLE engine header.

To use engine you should just include this header to your project.
This header is a part of DGLE SDK.
*/

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

/** Engine's main namespace. */

namespace DGLE
{

    //Engine Base interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("DFB1F52B-D906-4108-AD6F-3144E224688A")]
    public interface IDGLE_Base
    {
        void GetGUID(out Guid guid);
        void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
    }

    //Engine SubSystem interface//

    public enum E_ENGINE_SUB_SYSTEM
    {
        ESS_RENDER = 0,
        ESS_INPUT,
        ESS_SOUND,
        ESS_RESOURCE_MANAGER,
        ESS_FILE_SYSTEM,
        ESS_CORE_RENDERER
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("C682F875-E0BD-4af9-B79C-E209850025F8")]
    public interface IEngineSubSystem : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
    };

    //Engine Plugin interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("B94E0E40-8885-41dd-8BC5-4CD663AE5709")]
    public interface IPlugin : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void GetPluginInfo(out TPluginInfo stInfo);
        void GetPluginInterfaceName([MarshalAs(UnmanagedType.LPStr)] string pcName, out uint uiCharsCount);
    };



    //Engine Subsystem Plugin interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("27908E31-8D8E-4076-AE33-087A1BE5DCB3")]
    /** Base interface of any engine core subsystem plugin. */
    public interface ISubSystemPlugin : IPlugin
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IPlugin
        new void GetPluginInfo(out TPluginInfo stInfo);
        new void GetPluginInterfaceName(IntPtr pcName, out uint uiCharsCount);
        #endregion

        void GetSubSystemInterface(out IEngineSubSystem prSubSystem);
    };



    //Engine Base Object interface//

    public enum E_ENGINE_OBJECT_TYPE
    {
        EOT_UNKNOWN = 0,
        EOT_TEXTURE,
        EOT_MATERIAL,
        EOT_LIGHT,
        EOT_MESH,
        EOT_MODEL,
        EOT_BITMAP_FONT,
        EOT_SOUND_SAMPLE,
        EOT_MUSIC,

        EOT_EMPTY
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("C010239A-6457-40f5-87EF-FAA3156CE6E2")]
    public interface IEngineBaseObject : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void Free();
        void GetType(out E_ENGINE_OBJECT_TYPE eObjType);
        void GetUnknownType(out uint uiObjUnknownType);
    };

    //Events Interfaces//

    public enum E_EVENT_TYPE
    {
        ET_UNKNOWN = 0,
        ET_BEFORE_INITITIALIZATION,
        ET_BEFORE_RENDER,
        ET_AFTER_RENDER,
        ET_ON_PROFILER_DRAW,
        ET_ON_WINDOW_MESSAGE,
        ET_ON_GET_SUBSYSTEM,
        ET_ON_ENGINE_FATAL_MESSAGE,
        ET_ON_CONSOLE_WRITE,
        ET_ON_FULLSCREEN,
        ET_ON_PER_SECOND_TIMER
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("6DFEF982-AADF-42e9-A369-378BDB31404A")]
    public interface IBaseEvent : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void GetEventType(out E_EVENT_TYPE eEvType);
        void GetUnknownEventType(out uint uiUnknEvType);
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("EB735739-3D12-4522-B6D7-EEE3225DF934")]
    public interface IEvBeforeInitialization : IBaseEvent
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IBaseEvent
        new void GetEventType(out E_EVENT_TYPE eEvType);
        new void GetUnknownEventType(out uint uiUnknEvType);
        #endregion

        void SetEngParameters(ref TEngineWindow stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags);
        void GetEngParameters(out TEngineWindow stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags);
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("9E35969A-B0D4-4E5A-A89B-1A5AAD057028")]
    public interface IEvConsoleWrite : IBaseEvent
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IBaseEvent
        new void GetEventType(out E_EVENT_TYPE eEvType);
        new void GetUnknownEventType(out uint uiUnknEvType);
        #endregion

        void GetText([MarshalAs(UnmanagedType.LPStr)] string pcTxt, out uint uiCharsCount, [MarshalAs(UnmanagedType.U1)] out bool bToPrevLine);
    };


    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("DAA4E3BC-C958-4def-B603-F63EEC908226")]
    public interface IEvFatalMessage : IBaseEvent
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IBaseEvent
        new void GetEventType(out E_EVENT_TYPE eEvType);
        new void GetUnknownEventType(out uint uiUnknEvType);
        #endregion

        void GetMessageTxt([MarshalAs(UnmanagedType.LPStr)] string pcTxt, out uint uiCharsCount);
        void FreezeEngine([MarshalAs(UnmanagedType.U1)] bool bFreeze);
        void ForceNoMessage();
        void ForceIgnoreError();
    };


    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("8D718E48-581D-4cbb-9C40-C04998106F8D")]
    public interface IEvWindowMessage : IBaseEvent
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IBaseEvent
        new void GetEventType(out E_EVENT_TYPE eEvType);
        new void GetUnknownEventType(out uint uiUnknEvType);
        #endregion

        void GetMessage(out TWindowMessage stWinMsg);
		void SetMessage(ref TWindowMessage stWinMsg);
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("2B6D2547-716E-490c-B1F1-422CB428738F")]
    public interface IEvGetSubSystem : IBaseEvent
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IBaseEvent
        new void GetEventType(out E_EVENT_TYPE eEvType);
        new void GetUnknownEventType(out uint uiUnknEvType);
        #endregion

        void GetSubSystemType(out E_ENGINE_SUB_SYSTEM eSubSystem);
        void OverrideSubSystem(IEngineSubSystem pSubSystem);
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("CEC9184C-74D9-4739-BF48-BB800467665B")]
    public interface IEvGoFullScreen : IBaseEvent
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IBaseEvent
        new void GetEventType(out E_EVENT_TYPE eEvType);
        new void GetUnknownEventType(out uint uiUnknEvType);
        #endregion

        void GetResolution(out uint uiScreenWidth, out uint uiScreenHeight, [MarshalAs(UnmanagedType.U1)] out bool bGoFullScreen);
        void SetResolution(uint uiScreenWidth, uint uiScreenHeight);
    };


    //Main Engine System//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("371B1338-BB25-4B8C-BD6A-BCDF241CC52C")]
    public interface IEngineCallback : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(out Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void Initialize();
        void Free();
        void Update(UInt64 ui64DeltaTime);
        void Render();
        void OnEvent(E_EVENT_TYPE eEventType, IBaseEvent pEvent);
    };


    public enum E_ENGINE_PROCEDURE_TYPE
    {
        EPT_UPDATE = 0,
        EPT_RENDER,
        EPT_INIT,
        EPT_FREE
    };

    public enum E_LOG_TYPE
    {
        LT_INFO = 0,
        LT_WARNING,
        LT_ERROR,
        LT_FATAL
    };

    public enum E_ENGINE_INIT_FLAGS
    {
        EIF_DEFAULT                = 0x00000000,
        EIF_CATCH_UNHANDLED        = 0x00000001,
        EIF_FORCE_NO_SOUND         = 0x00000002,
        EIF_LOAD_ALL_PLUGINS       = 0x00000004,
        EIF_FORCE_LIMIT_FPS        = 0x00000010,
        EIF_FORCE_16_BIT_COLOR     = 0x00000020,
        EIF_ENABLE_FLOATING_UPDATE = 0x00000040,
        EIF_PER_FRAME_UPDATE       = 0x00000100,
        EIF_FORCE_NO_WINDOW        = 0x00000200,
        EIF_NO_SPLASH              = 0x10000000
    };

    public delegate void DSubscriber(IntPtr pParam);
    public delegate bool DboolSubscriber(IntPtr pParam);
    public delegate void DListenerProc(IntPtr pParam, IBaseEvent pEvent);
    public delegate void DFSDeleteCallback(IntPtr pParam, IFileSystem pVFS);
    public delegate void DSoundStreamCallback(IntPtr pParam, UInt32 ui32DataPos, IntPtr pBufferData, uint uiBufferSize); // is it ok ? // phomm

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("111BB884-2BA6-4e84-95A5-5E4700309CBA")]
    public interface IEngineCore : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void LoadSplashPicture([MarshalAs(UnmanagedType.LPStr)] string pcBmpFileName);
        void AddPluginToInitializationList([MarshalAs(UnmanagedType.LPStr)] string pcFileName);

        void InitializeEngine(IntPtr stHandle, [MarshalAs(UnmanagedType.LPStr)] string pcApplicationName, ref TEngineWindow stWindowParam, UInt32 uiUpdateInterval = 33, E_ENGINE_INIT_FLAGS eInitFlags = E_ENGINE_INIT_FLAGS.EIF_DEFAULT);
        void SetUpdateInterval(UInt32 uiUpdateInterval);
        void StartEngine();
        void QuitEngine();

        void ConnectPlugin([MarshalAs(UnmanagedType.LPStr)] string pcFileName, out IPlugin prPlugin);
        void DisconnectPlugin(IPlugin pPlugin);
        void GetPlugin([MarshalAs(UnmanagedType.LPStr)] string pcPluginName, out IPlugin prPlugin);

        void AddEngineCallback(IEngineCallback pEngineCallback);
        void RemoveEngineCallback(IEngineCallback pEngineCallback);

        void AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, [MarshalAs(UnmanagedType.FunctionPtr)] DSubscriber pProc, IntPtr pParam);
        void RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, [MarshalAs(UnmanagedType.FunctionPtr)] DSubscriber pProc, IntPtr pParam);

        void CastEvent(E_EVENT_TYPE eEventType, IBaseEvent pEvent);
        void AddEventListener(E_EVENT_TYPE eEventType, [MarshalAs(UnmanagedType.FunctionPtr)] DListenerProc pListenerProc, IntPtr pParam);
        void RemoveEventListener(E_EVENT_TYPE eEventType, [MarshalAs(UnmanagedType.FunctionPtr)] DListenerProc pListenerProc, IntPtr pParam);

        void GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, out IEngineSubSystem prSubSystem);

        void RenderFrame();
        void RenderProfilerTxt([MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor);
        void GetInstanceIdx(out uint uiIdx);
        void GetTimer(out UInt64 uiTick);
        void GetSystemInfo(out TSystemInfo stSysInfo);
        void GetCurrentWindow(out TEngineWindow stWin);
        void GetFPS(out uint uiFPS);
        void GetLastUpdateDeltaTime(out UInt64 ui64DeltaTime);
        void GetElapsedTime(out UInt64 ui64ElapsedTime);

        void GetWindowHandle(out IntPtr stHandle);

        void ChangeWindowMode(ref TEngineWindow stNewWin);
        void GetDesktopResolution(out uint uiWidth, out uint uiHeight);
        void AllowPause([MarshalAs(UnmanagedType.U1)] bool bAllow);

        void WriteToLog([MarshalAs(UnmanagedType.LPStr)] string pcTxt);
        void WriteToLogEx([MarshalAs(UnmanagedType.LPStr)] string pcTxt, E_LOG_TYPE eType, [MarshalAs(UnmanagedType.LPStr)] string pcSrcFileName, int iSrcLineNumber);

        void ConsoleVisible([MarshalAs(UnmanagedType.U1)] bool bIsVisible);
        void ConsoleWrite([MarshalAs(UnmanagedType.LPStr)] string pcTxt, [MarshalAs(UnmanagedType.U1)] bool bWriteToPreviousLine = false);
        void ConsoleExecute([MarshalAs(UnmanagedType.LPStr)] string pcCommandTxt);
        void ConsoleRegisterCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommandName, [MarshalAs(UnmanagedType.LPStr)] string pcCommandHelp, [MarshalAs(UnmanagedType.FunctionPtr)] DboolSubscriber pProc, IntPtr pParam);
        void ConsoleRegisterVariable([MarshalAs(UnmanagedType.LPStr)] string pcCommandName, [MarshalAs(UnmanagedType.LPStr)] string pcCommandHelp, ref int piVar, int iMinValue, int iMaxValue, [MarshalAs(UnmanagedType.FunctionPtr)] DboolSubscriber pProc, IntPtr pParam);
        void ConsoleUnregister([MarshalAs(UnmanagedType.LPStr)] string pcCommandName);

        void GetVersion([MarshalAs(UnmanagedType.LPStr)] string pcBuffer, out uint uiBufferSize);
    };

    //Resource Manager SubSystem//

    public enum E_TEXTURE_DATA_FORMAT
    {
        TDF_RGB8 = 0,
        TDF_RGBA8,
        TDF_ALPHA8,
        TDF_BGR8,
        TDF_BGRA8,
        TDF_DXT1,
        TDF_DXT5,
        TDF_DEPTH_COMPONENT24,
        TDF_DEPTH_COMPONENT32
    };

    public enum E_TEXTURE_CREATION_FLAGS
    {
        TCF_DEFAULT = 0x00000000,
        TCF_PIXEL_ALIGNMENT_1 = 0x00000001,//use only if your texture input data is not 4 byte aligned
        TCF_MIPMAPS_PRESENTED = 0x00000002 //all mip levels must be presented
    };


    public enum E_TEXTURE_LOAD_FLAGS
    {
        TLF_FILTERING_NONE = 0x00000001,
        TLF_FILTERING_BILINEAR = 0x00000002,
        TLF_FILTERING_TRILINEAR = 0x00000004,
        TLF_FILTERING_ANISOTROPIC = 0x00000008,

        TLF_DECREASE_QUALITY_MEDIUM = 0x00000020,
        TLF_DECREASE_QUALITY_HIGH = 0x00000040,

        TLF_COMPRESS = 0x00000100,

        TLF_COORDS_REPEAT = 0x00001000,
        TLF_COORDS_CLAMP = 0x00002000,
        TLF_COORDS_MIRROR_REPEAT = 0x00004000,
        TLF_COORDS_MIRROR_CLAMP = 0x00008000,

        TLF_GENERATE_MIPMAPS = 0x00040000,

        TLF_ANISOTROPY_2X = 0x00100000,
        TLF_ANISOTROPY_4X = 0x00200000,
        TLF_ANISOTROPY_8X = 0x00400000,
        TLF_ANISOTROPY_16X = 0x00800000,

        TEXTURE_LOAD_DEFAULT_2D = TLF_FILTERING_BILINEAR | TLF_COORDS_CLAMP,
        TEXTURE_LOAD_DEFAULT_3D = TLF_FILTERING_TRILINEAR | TLF_GENERATE_MIPMAPS | TLF_COORDS_REPEAT,

        RES_LOAD_DEFAULT = 0x00000000
    };

    public enum E_MESH_CREATION_FLAGS
    {
        MCF_ONLY_DEFAULT_DATA = 0x00000000,//vertex and indexes arrays must be presented
        MCF_NORMALS_PRESENTED = 0x00000001,
        MCF_TEXTURE_COORDS_PRESENTED = 0x00000002,
        MCF_TANGENT_SPACE_PRESENTED = 0x00000004,
        MCF_VERTEX_DATA_INTERLEAVED = 0x00000008
    };

    public enum E_MESH_MODEL_LOAD_FLAGS
    {
        MMLF_FORCE_SOFTWARE_BUFFER = 0x00000001,
        MMLF_DYNAMIC_BUFFER = 0x00000002,
        MMLF_FORCE_MODEL_TO_MESH = 0x00000004
    };

    enum E_SOUND_SAMPLE_LOAD_FLAGS
    {
        SSLF_LOAD_AS_MUSIC = 0x00000001
    };


    public delegate bool DLoadProc(ref IFile pFile, ref IEngineBaseObject prObj, uint uiLoadFlags);

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("139505B6-5EFC-4f02-A5E8-18CD1FBD69E3")]
    public interface IResourceManager : IEngineSubSystem
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngineSubSystem
        new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
        #endregion

        void CreateTexture(out ITexture prTex, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATION_FLAGS eCreationFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, [MarshalAs(UnmanagedType.LPStr)] string pcName = "", [MarshalAs(UnmanagedType.U1)] bool bAddResource = true);
        void CreateMaterial(out IMaterial prMaterial, [MarshalAs(UnmanagedType.LPStr)] string pcName = "", bool bAddResource = true);
        void CreateLight(out ILight prLight, [MarshalAs(UnmanagedType.LPStr)] string pcName = "", bool bAddResource = true);
        void CreateMesh(out IMesh prMesh, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATION_FLAGS eCreationFlags, E_MESH_MODEL_LOAD_FLAGS eLoadFlags, [MarshalAs(UnmanagedType.LPStr)] string pcName = "", [MarshalAs(UnmanagedType.U1)] bool bAddResource = true);
        void CreateModel(out IModel prModel, [MarshalAs(UnmanagedType.LPStr)] string pcName, bool bAddResource = true);
        void CreateSound(out ISoundSample prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, UInt32 ui32DataSize, [MarshalAs(UnmanagedType.LPStr)] string pcName = "", [MarshalAs(UnmanagedType.U1)] bool bAddResource = true);

        void RegisterFileFormat([MarshalAs(UnmanagedType.LPStr)] string pcExtension, E_ENGINE_OBJECT_TYPE eObjType, [MarshalAs(UnmanagedType.LPStr)] string pcDescription, [MarshalAs(UnmanagedType.FunctionPtr)] DLoadProc pLoadProc, IntPtr pParam);
        void UnregisterFileFormat([MarshalAs(UnmanagedType.LPStr)] string pcExtension);
        void RegisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject pObj);
        void UnregisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject pObj);
        void GetRegisteredExtensions(IntPtr pcTxt, out uint uiCharsCount);
        void GetExtensionDescription([MarshalAs(UnmanagedType.LPStr)] string pcExtension, IntPtr pcTxt, out uint uiCharsCount);
        void GetExtensionType([MarshalAs(UnmanagedType.LPStr)] string pcExtension, out E_ENGINE_OBJECT_TYPE eType);

        void GetResourceByName([MarshalAs(UnmanagedType.LPStr)] string pcName, out IEngineBaseObject prObj);
        void GetResourceByIndex(uint uiIdx, out IEngineBaseObject prObj);
        void GetResourceName(IEngineBaseObject pObj, [MarshalAs(UnmanagedType.LPStr)] string pcName, out uint uiCharsCount);
        void GetDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, out IEngineBaseObject prObj);
        void GetResourcesCount(uint uiCount);

        void Load([MarshalAs(UnmanagedType.LPStr)] string pcFileName, out IEngineBaseObject prObj, uint uiLoadFlags, [MarshalAs(UnmanagedType.LPStr)] string pcName = "");
        void LoadEx(IFile pFile, out IEngineBaseObject prObj, uint uiLoadFlags, [MarshalAs(UnmanagedType.LPStr)] string pcName = "");

        void FreeResource(ref IEngineBaseObject pEngObj);
        void AddResource([MarshalAs(UnmanagedType.LPStr)] string pcFileName, IEngineBaseObject pObj);
        void RemoveResource(IEngineBaseObject pObj, [MarshalAs(UnmanagedType.U1)] out bool bCanDelete);
    };

    //Render SubSystem//

    public enum E_GET_POINT3_MODE
    {
        GP3M_FROM_DEPTH_BUFFER = 0,
        GP3M_FROM_FAR_PLANE,
        GP3M_FROM_NEAR_PLANE
    };

    //Flags for Effects Blend mode
    public enum E_BLENDING_EFFECT
    {
        BE_NORMAL = 0,
        BE_ADD,
        BE_MULT,
        BE_BLACK,
        BE_WHITE,
        BE_MASK
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("EA03C661-A334-4225-B5DB-4C45452CCC41")]
    public interface IRender : IEngineSubSystem
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngineSubSystem
        new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
        #endregion

        void SetClearColor(ref TColor4 stColor);
        void GetClearColor(out TColor4 stColor);
        void ClearColorBuffer();

        void Unbind(E_ENGINE_OBJECT_TYPE eType); //use EOT_UNKNOWN to unbind all
        void EnableScissor(ref TRectF stArea);
        void DisableScissor();
        void GetScissor([MarshalAs(UnmanagedType.U1)] out bool bEnabled, out TRectF stArea);
        void SetRenderTarget(ITexture pTargetTex = null);
        void GetRenderTarget(out ITexture prTargetTex);


        void GetRender2D(out IRender2D prRender2D);
        void GetRender3D(out IRender3D prRender3D);
    };

    //Render2D interface//

    //Flags for Primitives
    public enum E_PRIMITIVE2D_FLAGS
    {
        PF_DEFAULT              = 0x00000000,
        PF_LINE                 = 0x00000000,
        PF_FILL                 = 0x00000001,
        PF_VERTICES_COLORS      = 0x00000002,
        PF_SMOOTH               = 0x00000004
    };

    //Flags for Effects
    public enum E_EFFECT2D_FLAGS
    {
        EF_DEFAULT              = 0x00000000,
        EF_NONE                 = 0x00000001,
        EF_ALPHA_TEST           = 0x00000002,
        EF_BLEND                = 0x00000004,
        EF_FLIP_HORIZONTALLY    = 0x00000008,
        EF_FLIP_VERTICALLY      = 0x00000010,
        EF_COLOR_MIX            = 0x00000020,
        EF_SCALE                = 0x00000040,
        EF_VERTICES_OFFSETS     = 0x00000080,
        EF_VERTICES_COLORS      = 0x00000100,
        EF_ROTATION_POINT       = 0x00000200,
        EF_TILE_TEXTURE         = 0x00000400
    };

    public enum E_BATCH_MODE2D
    {
        BM_AUTO = 0,
        BM_DISABLED,
        BM_ENABLED_UPDATE_EVERY_TICK,
        BM_ENABLED_UPDATE_EVERY_FRAME
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("F5F3257A-F8B8-4d91-BA67-451167A8D63F")]
    public interface IRender2D : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        //Note: Only one pair Begin2D-End2D per frame is allowed or batching must be disabled.
        void Begin2D();
        void End2D();
        void BatchRender(E_BATCH_MODE2D eMode);
        void InvalidateBatchData();
        void BeginBatch([MarshalAs(UnmanagedType.U1)] bool bUpdateEveryFrame = false);
        void EndBatch();
        void NeedToUpdateBatchData([MarshalAs(UnmanagedType.U1)] out bool bNeedUpdate);
        void SetResolutionCorrection(UInt32 uiResX, UInt32 uiResY, [MarshalAs(UnmanagedType.U1)] bool bConstantProportions = true); //Set resx and resy to current screen size to turn off correction
        void ResolutionCorrectToAbsolute(ref TPoint2 stLogicCoord, out TPoint2 stAbsoluteCoord);
        void AbsoluteToResolutionCorrect(ref TPoint2 stAbsoluteCoord, out TPoint2 stLogicCoord);
        void SetCamera(ref TPoint2 stCenter, float fAngle, ref TPoint2 stScale);
        void ResetCamera();
        void UnprojectCameraToScreen(ref TPoint2 stCameraCoord, out TPoint2 stScreenCoord);
        void ProjectScreenToCamera(ref TPoint2 stScreenCoord, out TPoint2 stCameraCoord);
        void CullBoundingBox(ref TRectF stBBox, float fAngle, [MarshalAs(UnmanagedType.U1)] out bool bCull);

        // 2D Primitives
        void SetLineWidth(UInt32 uiWidth);
        void DrawPoint(ref TPoint2 stCoords, ref TColor4 stColor, UInt32 uiSize = 1);
        void DrawLine(ref TPoint2 stCoords1, ref TPoint2 stCoords2, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags = E_PRIMITIVE2D_FLAGS.PF_DEFAULT);
        void DrawRectangle(ref TRectF stRect, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags);
        void DrawCircle(ref TPoint2 stCoords, UInt32 uiRadius, UInt32 uiQuality, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags = E_PRIMITIVE2D_FLAGS.PF_DEFAULT);
        void DrawEllipse(ref TPoint2 stCoords, ref TPoint2 stRadius, UInt32 uiQuality, ref TColor4 stColor, E_PRIMITIVE2D_FLAGS eFlags = E_PRIMITIVE2D_FLAGS.PF_DEFAULT);
        void DrawPolygon(ITexture pTexture, ref TVertex2[] pstVertices, UInt32 uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags = E_PRIMITIVE2D_FLAGS.PF_DEFAULT);

        // 2D Sprites		
        void DrawTexture(ITexture pTexture, ref TPoint2 stCoords, ref TPoint2 stDimensions, float fAngle = 0f, E_EFFECT2D_FLAGS flags = E_EFFECT2D_FLAGS.EF_DEFAULT);
        void DrawTextureCropped(ITexture pTexture, ref TPoint2 stCoords, ref TPoint2 stDimensions, ref TRectF stTexCropRect, float fAngle = 0f, E_EFFECT2D_FLAGS eFlags = E_EFFECT2D_FLAGS.EF_DEFAULT);
        void DrawTextureSprite(ITexture pTexture, ref TPoint2 stCoords, ref TPoint2 stDimensions, UInt32 uiFrameIndex, float fAngle = 0f, E_EFFECT2D_FLAGS eFlags = E_EFFECT2D_FLAGS.EF_DEFAULT);

        // Extra
        void DrawTriangles(ITexture pTexture, ref TVertex2[] pstVertices, UInt32 uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags = E_PRIMITIVE2D_FLAGS.PF_DEFAULT);
        void DrawMesh(IMesh pMesh, ITexture pTexture, ref TPoint2 stCoords, ref TPoint3 stDimensions, ref TPoint3 stAxis, float fAngle = 0f, E_EFFECT2D_FLAGS eFlags = E_EFFECT2D_FLAGS.EF_DEFAULT, [MarshalAs(UnmanagedType.U1)] bool bClip = true, float fFovY = 90f, [MarshalAs(UnmanagedType.U1)] bool bClearDepthBuffer = false);

        //Advanced
        void Draw(ITexture pTexture, ref TDrawDataDesc stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, ref TRectF stAABB, E_EFFECT2D_FLAGS eFlags);
        void DrawBuffer(ITexture pTexture, ICoreGeometryBuffer pBuffer, ref TRectF stAABB, E_EFFECT2D_FLAGS eFlags);
        void DrawBuffer3D(ITexture pTexture, ICoreGeometryBuffer pBuffer, E_EFFECT2D_FLAGS eFlags, ref TMatrix4x4 stTransform, ref TPoint3 stCenter, ref TPoint3 stExtents, [MarshalAs(UnmanagedType.U1)] bool bClip, float fFovY);

        //Effects
        void SetRotationPoint(ref TPoint2 stCoords);//In texture coord system
        void SetScale(ref TPoint2 stScale);
        void SetColorMix(ref TColor4 stColor);
        void SetBlendMode(E_BLENDING_EFFECT eMode = E_BLENDING_EFFECT.BE_NORMAL);
        void SetVerticesOffsets(ref TPoint2 stCoords1, ref TPoint2 stCoords2, ref TPoint2 stCoords3, ref TPoint2 stCoords4);
        void SetVerticesColors(ref TColor4 stColor1, ref TColor4 stColor2, ref TColor4 stColor3, ref TColor4 stColor4);

        void GetRotationPoint(out TPoint2 stCoords);
        void GetScale(out TPoint2 stScale);
        void GetColorMix(out TColor4 stColor);
        void GetBlendMode(out E_BLENDING_EFFECT eMode);
        void GetVerticesOffsets(out TPoint2 stCoords1, out TPoint2 stCoords2, out TPoint2 stCoords3, out TPoint2 stCoords4);
        void GetVerticesColors(out TColor4 stColor1, out TColor4 stColor2, out TColor4 stColor3, out TColor4 stColor4);

    };

    //Render3D interface//

    public enum E_PUSH_STATES_FLAGS
    {
        PSF_ALL = 0x00000000,
        PSF_MATRIX = 0x00000001,
        PSF_STATES = 0x00000002
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("5275F43A-4FF9-48b2-B88E-B2F842461AB3")]
    public interface IRender3D : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void SetPerspective(float fFovAngle, float fZNear, float fZFar);
        void GetPerspective(out float fFovAngle, out float fZNear, out float fZFar);

        void SetColor(ref TColor4 stColor);
        void GetColor(out TColor4 stColor);

        void BindTexture(ITexture pTex, uint uiTextureLayer);
        void GetTexture(out ITexture prTex, uint uiTextureLayer);

        void GetMaxLightsPerPassCount(out uint uiCount);
        void UpdateLight(ILight pLight);

        void BindMaterial(IMaterial pMat);
        void GetMaterial(out IMaterial prMat);

        void ToggleBlending([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void IsBlendingEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);
        void SetBlendMode(E_BLENDING_EFFECT eMode = E_BLENDING_EFFECT.BE_NORMAL);
        void GetBlendMode(out E_BLENDING_EFFECT eMode);

        void ToggleAlphaTest([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetAlphaTreshold(float fValue);
        void IsAlphaTestEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled); 
        void GetAlphaTreshold(out float fValue);

        void ClearDepthBuffer();
        void ToggleDepthTest([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void IsDepthTestEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);

        void ToggleBackfaceCulling([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void IsBackfaceCullingEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);

        void Draw(ref TDrawDataDesc stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount);
        void DrawBuffer(ICoreGeometryBuffer pBuffer);

        void ToggleFog([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetLinearFogBounds(float fStart, float fEnd);
        void SetFogColor(ref TColor4 stColor);
        void SetFogDensity(float fDensity);
        void IsFogEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);
        void GetFogColor(out TColor4 stColor);
        void GetLinearFogBounds(out float fStart, out float fEnd);
        void GetFogDensity(out float fDensity);

        void SetMatrix(ref TMatrix4x4 stMatrix);
        void MultMatrix(ref TMatrix4x4 stMatrix);
        void PushMatrix();
        void PopMatrix();
        void GetMatrix(out TMatrix4x4 stMatrix);

        void DrawAxis(float fSize = 1f, [MarshalAs(UnmanagedType.U1)] bool bNoDepthTest = false);

        void ResetStates();
        void PushStates();
        void PopStates();

        void GetPoint3(ref TPoint2 stPointOnScreen, out TPoint3 stResultPoint, E_GET_POINT3_MODE eFlag = E_GET_POINT3_MODE.GP3M_FROM_DEPTH_BUFFER);
        void GetPoint2(ref TPoint3 stPoint, out TPoint2 stResultPointOnScreen);

        void SetupFrustum();
        void CullPoint(ref TPoint3 stCoords, [MarshalAs(UnmanagedType.U1)] out bool bCull);
        void CullSphere(ref TPoint3 stCenter, float radius, [MarshalAs(UnmanagedType.U1)] out bool bCull);
        void CullBox(ref TPoint3 stCenter, ref TPoint3 stExtents, [MarshalAs(UnmanagedType.U1)] out bool bCull);

        void ToggleLighting([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetGlobalAmbientLighting(ref TColor4 stColor);
        void IsLightingEnabled([MarshalAs(UnmanagedType.U1)] out bool bEnabled);
        void GetGlobalAmbientLighting(out TColor4 stColor);
    };

    //Light interface//

    public enum E_LIGHT_TYPE
    {
        LT_DIRECTIONAL,
        LT_POINT,
        LT_SPOT
    }

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("EB73AC84-A465-4554-994D-8BED29744C9D")]
    public interface ILight : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void SetEnabled([MarshalAs(UnmanagedType.U1)] bool bEnabled);
        void SetColor(ref TColor4 stColor);
        void SetPosition(ref TPoint3 stPos);
        void SetDirection(ref TPoint3 stDir);
        void SetRange(float fRange);
        void SetIntensity(float fIntensity);
        void SetSpotAngle(float fAngle);

        void SetType(E_LIGHT_TYPE eType);

        void GetEnabled([MarshalAs(UnmanagedType.U1)]out bool bEnabled);
        void GetColor(out TColor4 stColor);
        void GetPosition(out TPoint3 stPos);
        void GetDirection(out TPoint3 stDir);
        void GetRange(out float fRange);
        void GetIntensity(out float fIntensity);
        void GetSpotAngle(out float fAngle);

        void GetType(out E_LIGHT_TYPE eType);

        void Update();
    };

    //Texture interface//  


    //public interface ICoreTexture
    //{ };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("85BDDBC2-F126-4cae-946D-7D6B079E5CCE")]
    public interface ITexture : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void GetDimensions(out UInt32 irWidth, out UInt32 irHeight);
        void SetFrameSize(UInt32 uiFrameWidth, UInt32 uiFrameHeight);
        void GetFrameSize(out UInt32 uiFrameWidth, out UInt32 uiFrameHeight);
        void FramesCount(out uint uiCount);

        void GetCoreTexture(out ICoreTexture prCoreTex);

        void Draw2DSimple(Int32 iX, Int32 iY, UInt32 uiFrameIndex);
        void Draw2D(Int32 iX, Int32 iY, UInt32 uiWidth, UInt32 uiHeight, float fAnglef, UInt32 uiFrameIndex);
        void Draw3D(UInt32 uiFrameIndex = 0);

        void Bind(UInt32 uiTextureLayer);
    };

    //Material interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("B6506749-BB41-423d-B6C0-982081EF63F9")]
    public interface IMaterial : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void SetDiffuseColor(ref TColor4 stColor);
        void SetSpecularColor(ref TColor4 stColor);
        void SetShininess(float fShininess);
        void SetDiffuseTexture(ITexture pTexture);
        void SetBlending([MarshalAs(UnmanagedType.U1)] bool bEnabled, E_BLENDING_EFFECT eMode);
        void SetAlphaTest([MarshalAs(UnmanagedType.U1)] bool bEnabled, float fTreshold);

        void GetDiffuseColor(out TColor4 stColor);
        void GetSpecularColor(out TColor4 stColor);
        void GetShininess(out float fShininess);
        void GetDiffuseTexture(out ITexture prTexture);
        void GetBlending([MarshalAs(UnmanagedType.U1)] out bool bEnabled, out E_BLENDING_EFFECT eMode);
        void GetAlphaTest([MarshalAs(UnmanagedType.U1)] out bool bEnabled, out float fTreshold);

        void Bind();
    };


    //BitmapFont interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("0B03E8D7-23A3-4c79-9E82-5BC6E50E1EBA")]
    public interface IBitmapFont : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void GetTexture(out ITexture prTexture);
        void SetScale(float fScale);
        void GetScale(out float fScale);
        void GetTextDimensions([MarshalAs(UnmanagedType.LPStr)] string pcTxt, out UInt32 uiWidth, out UInt32 uiHeight);
        void Draw2DSimple(int iX, int iY, [MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor);
        void Draw2D(float fX, float fY, [MarshalAs(UnmanagedType.LPStr)] string pcTxt, ref TColor4 stColor, float fAngle = 0, [MarshalAs(UnmanagedType.U1)] bool bVerticesColors = false);
        void Draw3D([MarshalAs(UnmanagedType.LPStr)] string pcTxt);
    };

    //3D Objects interfaces//

    //Mesh interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("85E360A8-07B3-4f22-AA29-07C7FC7C6893")]
    public interface IMesh : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void Draw();
        void GetCenter(out TPoint3 stCenter);
        void GetExtents(out TPoint3 stExtents);
        void GetTrianglesCount(out UInt32 uiCnt);
        void GetGeometryBuffer(out ICoreGeometryBuffer prBuffer);
        void SetGeometryBuffer(ICoreGeometryBuffer pBuffer, [MarshalAs(UnmanagedType.U1)] bool bFreeCurrentBuffer);
        void RecalculateNormals([MarshalAs(UnmanagedType.U1)] bool bInvert = false);
        void RecalculateTangentSpace();
        void RecalculateBounds();
        void TransformVertices(ref TMatrix4x4 stTransMatrix);
        void Optimize();
        void GetOwner(out IModel prMMesh);
        void SetOwner(IModel prMMesh);

    };

    //Model interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("6107C296-FC07-48d1-B6A7-F88CC2DAE897")]
    public interface IModel : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void Draw();
        void DrawMesh(uint uiMeshIdx);
        void GetCenter(out TPoint3 stCenter);
        void GetExtents(out TPoint3 stExtents);
        void MeshsCount(out uint uiCount);
        void GetMesh(uint uiIdx, out IMesh prMesh);
        void SetModelMaterial(IMaterial pMaterial);
        void GetModelMaterial(out IMaterial prMaterial);
        void SetMeshMaterial(uint uiMeshIdx, IMaterial pMaterial);
        void GetMeshMaterial(uint uiMeshIdx, out IMaterial prMaterial);
        void AddMesh(IMesh pMesh);
        void RemoveMesh(IMesh pMesh);
        void ReplaceMesh(uint uiMeshIdx, IMesh pMesh);

    };

    //Postprocess interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("DC6E4812-6B06-4de8-9DEF-2A13740BC45E")]
    public interface IPostprocess : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void Free();

        void SetTargets(ITexture pSrc, ITexture pDst);
        void SetBlurAmount(float fBlur);
        void SetBloomAmount(float fBloom);
        void SetBloomThreshold(float fThreshold);
        void ToggleMonochrome([MarshalAs(UnmanagedType.U1)] bool bEnable);
        void Perform();
    };

    //Input Subsystem//

    public enum E_INPUT_CONFIGURATION_FLAGS
    {
        ICF_DEFAULT = 0x00000000,
        ICF_EXCLUSIVE = 0x00000001,
        ICF_HIDE_CURSOR = 0x00000002,
        ICF_CURSOR_BEYOND_SCREEN = 0x00000004
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("64DAAF7F-F92C-425f-8B92-3BE40D8C6666")]
    public interface IInput : IEngineSubSystem
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngineSubSystem
        new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
        #endregion

        void Configure(E_INPUT_CONFIGURATION_FLAGS eFlags = E_INPUT_CONFIGURATION_FLAGS.ICF_DEFAULT);

        void GetMouseStates(out TMouseStates stMStates);
        void GetKey(E_KEYBOARD_KEY_CODES eKeyCode, [MarshalAs(UnmanagedType.U1)] out bool bPressed);
        void GetKeyName(E_KEYBOARD_KEY_CODES eKeyCode, [MarshalAs(UnmanagedType.LPStr)] out char cASCIICode);

        void BeginTextInput([MarshalAs(UnmanagedType.LPStr)] string pcBuffer, uint uiBufferSize);
        void EndTextInput();

        void GetJoysticksCount(out uint uiCount);
        void GetJoystickName(uint uiJoyId, IntPtr pcName, out uint uiCharsCount);
        void GetJoystickStates(uint uiJoyId, out TJoystickStates stJoyStates);
    };

    //Sound SubSystem interfaces//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("054C07EE-2724-42f2-AC2B-E81FCF5B4ADA")]
    public interface ISound : IEngineSubSystem
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngineSubSystem
        new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
        #endregion

        /*Dummy*/
        void SetMasterVolume(UInt32 uiVolume);
        void MasterPause([MarshalAs(UnmanagedType.U1)] bool bPaused);
        void StopAllChannels();
        void GetMaxChannelsCount(out uint uiCount);
        void GetFreeChannelsCount(out uint uiCount);
        void ReleaseChannelsByData([MarshalAs(UnmanagedType.LPArray)] byte[] pData);
        void CreateChannel(out ISoundChannel prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, [MarshalAs(UnmanagedType.U1)] bool bStereo, [MarshalAs(UnmanagedType.LPArray)] byte[] pData, UInt32 ui32DataSize); //Data not copied!
        // is this translation ok ? // phomm
        void CreateStreamableChannel(out ISoundChannel prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, [MarshalAs(UnmanagedType.U1)]  bool bStereo, UInt32 ui32DataSize, [MarshalAs(UnmanagedType.FunctionPtr)] DSoundStreamCallback pStreamCallback, IntPtr pParameter);

    };

    //SoundSample interface//

    public enum E_SOUND_CHANNEL_EFFECTS
    {
        SCE_NONE        = 0x00000000,
        SCE_CHORUS      = 0x00000001,
        SCE_COMPRESSOR  = 0x00000002,
        SCE_DISTORTION  = 0x00000004,
        SCE_ECHO        = 0x00000008,
        SCE_FLANGER     = 0x00000010,
        SCE_GARGLE      = 0x00000020,
        SCE_REVERB      = 0x00000040
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("DE6F7CDD-8262-445c-8D20-68E3324D99A6")]
    public interface ISoundChannel : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void Play([MarshalAs(UnmanagedType.U1)] bool bLooped);
        void Pause();
        void Stop();
        void IsPlaying([MarshalAs(UnmanagedType.U1)]out bool bIsPlaying);
        void SetVolume(UInt32 uiVolume); //from 0 to 100
        void GetVolume(out UInt32 uiVolume);
        void SetPan(int iPan); //from -100 to 100
        void GetPan(out int iPan);
        void SetSpeed(uint uiSpeed); // in percents
        void GetSpeed(out uint uiSpeed);
        void SetCurrentPosition(uint uiPos);
        void GetCurrentPosition(out uint uiPos);
        void GetLength(out uint uiLength);
        void IsStreamable([MarshalAs(UnmanagedType.U1)] out bool bStreamable);
        void Unaquire();
    };

    public enum E_SOUND_SAMPLE_PARAMS
    {
        SSP_NONE    = 0x00000000,
        SSP_LOOPED  = 0x00000001,
    };

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("30DD8C94-D3FA-40cf-9C49-649211424919")]
    public interface ISoundSample : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void Play();
        void PlayEx(out ISoundChannel pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags = E_SOUND_SAMPLE_PARAMS.SSP_NONE); //pSndChnl must be checked on nul
    };

    public interface IMusic : IEngineBaseObject
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngBaseObj
        new void Free();
        new void GetType(out E_ENGINE_OBJECT_TYPE eEngObjType);
        new void GetUnknownType(out uint uiObjUnknownType);
        #endregion

        void Play([MarshalAs(UnmanagedType.U1)]bool bLooped = true);
        void Pause([MarshalAs(UnmanagedType.U1)]bool bPaused);
        void Stop();
        void IsPlaying([MarshalAs(UnmanagedType.U1)]out bool bIsPlaying);
        void SetVolume(UInt32 uiVolume);
        void GetVolume(out UInt32 uiVolume);
        void SetCurrentPosition(UInt32 uiPos);
        void GetCurrentPosition(out UInt32 uiPos);
        void GetLength(out UInt32 uiLength);
    };


    //FileSystem interfaces//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("4850286F-4770-4bcf-A90A-33D7BE41E686")]
    public interface IMainFileSystem : IEngineSubSystem
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        #region IEngineSubSystem
        new void GetType(out E_ENGINE_SUB_SYSTEM eSubSystemType);
        #endregion

        void LoadFile([MarshalAs(UnmanagedType.LPStr)] string pcFileName, out IFile prFile);// c:\data.zip|img.jpg
        void GetVirtualFileSystem([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension, out IFileSystem prVFS);
        void RegisterVirtualFileSystem([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension, [MarshalAs(UnmanagedType.LPStr)] string pcDescription, IFileSystem prVFS, [MarshalAs(UnmanagedType.FunctionPtr)] DFSDeleteCallback pProc, IntPtr pParameter);
        void UnregisterVirtualFileSystem([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension);
        void GetRegisteredVirtualFileSystems(IntPtr pcTxt, out uint uiCharsCount);
        void GetVirtualFileSystemDescription([MarshalAs(UnmanagedType.LPStr)] string pcVFSExtension, IntPtr pcTxt, out uint uiCharsCount);
    };

    public enum E_FIND_FLAGS
    {
        FF_RECURSIVE = 1
    };

    public enum E_FILE_SYSTEM_OPEN_FLAGS
    {
        FSOF_READ = 0x00000001,
        FSOF_WRITE = 0x00000002,
        FSOF_TRUNC = 0x00000004,
        FSOF_BINARY = 0x00000008
    };

    public enum E_FILE_SYSTEM_SEEK_FLAG
    {
        FSSF_BEGIN = 0,
        FSSF_CURRENT,
        FSSF_END
    };

    //File interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("AE6E8AE7-3E5B-4bc4-A512-42E1CF1DF005")]
    public interface IFile : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void Read(IntPtr pBuffer, uint uiCount, out UInt32 uiRead);
        void Write(IntPtr pBuffer, uint uiCount, out uint uiWritten);
        void Seek(UInt32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, out UInt32 ui32Position);
        void GetSize(out UInt32 ui32Size);
        void IsOpen([MarshalAs(UnmanagedType.U1)] out bool bOpened);
        void GetName(IntPtr pcName, out uint uiCharsCount);
        void GetPath(IntPtr pcPath, out uint uiCharsCount);
        void Free();
    };

    //FileIterator interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("5D73F249-0E74-4cc5-9646-270CB1E22750")]
    public interface IFileIterator : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void FileName([MarshalAs(UnmanagedType.LPStr)] string pcName, out uint uiLength);
        void Next();
        void Free();
    };

    //FileSystem interface//

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("2DAE578E-9636-4fae-BABB-7D835EEA7518")]
    public interface IFileSystem : IDGLE_Base
    {
        #region IDGLE_Base
        new void GetGUID(ref Guid guid);
        new void ExecuteCommand(uint uiCmd, ref TVariant stVar);
        new void ExecuteTextCommand([MarshalAs(UnmanagedType.LPStr)] string pcCommand, ref TVariant stVar);
        new void ExecuteTextCommandEx([MarshalAs(UnmanagedType.LPStr)] string pcCommand, IntPtr pcResult, out uint uiCharsCount);
        #endregion

        void OpenFile([MarshalAs(UnmanagedType.LPStr)] string pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, out IFile prFile);
        void DeleteFile([MarshalAs(UnmanagedType.LPStr)] string pcName);
        void FileExists([MarshalAs(UnmanagedType.LPStr)] string pcName, [MarshalAs(UnmanagedType.U1)] out bool bExists);
        void Find([MarshalAs(UnmanagedType.LPStr)] string pcMask, E_FIND_FLAGS eFlags, out IFileIterator prIterator);
    };

    //Initialization and Releasing Engine//

    public static class Engine
    {
        public enum E_GET_ENGINE_FLAGS
        {
            GEF_DEFAULT = 0x00000000,
            GEF_FORCE_SINGLE_THREAD = 0x00000001,
            GEF_FORCE_NO_LOG_FILE = 0x00000002,
            GEF_FORCE_QUIT = 0x00000004
        };

        delegate void GetEngineDelegate(out IEngineCore pEngineCore, E_GET_ENGINE_FLAGS eFlags, byte ubtSDKVer);
        delegate void FreeEngineDelegate(IEngineCore pEngineCore);

        static GetEngineDelegate pGetEngine;
        static FreeEngineDelegate pFreeEngine;
        static IntPtr hServer = IntPtr.Zero;

        public const string _DGLE_VER_ = "2:0.3.0";
        public const byte _DGLE_SDK_VER_ = 1;
        public const byte _DGLE_PLUGIN_SDK_VER_ = _DGLE_SDK_VER_;

        public static bool GetEngine([MarshalAs(UnmanagedType.LPStr)] string pcDllFileName, out IEngineCore pEngineCore, E_GET_ENGINE_FLAGS eFlags)
        {
            pEngineCore = null;

            if (hServer == IntPtr.Zero)
            {
                if (hServer == IntPtr.Zero)
                {
                    hServer = Imports.LoadLibrary(pcDllFileName);
                    if (hServer == IntPtr.Zero) return false;
                }
                if (pGetEngine == null && pFreeEngine == null)
                {
                    IntPtr pGetEnginePointer = Imports.GetProcAddress(hServer, "CreateEngine");
                    pGetEngine = Marshal.GetDelegateForFunctionPointer(pGetEnginePointer, typeof(GetEngineDelegate)) as GetEngineDelegate;
                    IntPtr pFreeEnginePointer = Imports.GetProcAddress(hServer, "FreeEngine");
                    pFreeEngine = Marshal.GetDelegateForFunctionPointer(pFreeEnginePointer, typeof(FreeEngineDelegate)) as FreeEngineDelegate;
                    if (pGetEngine == null || pFreeEngine == null)
                    {
                        Imports.FreeLibrary(hServer);
                        hServer = IntPtr.Zero;
                        return false;
                    }
                }

                CreateEngine(out pEngineCore, eFlags);
                return (pEngineCore != null);
            }
            else
                return false;
        }

        public static bool CreateEngine(out IEngineCore pEngineCore, E_GET_ENGINE_FLAGS eFlags)
        {
            if (pGetEngine == null)
            {
                pEngineCore = null;
                return false;
            }
            pGetEngine(out pEngineCore, eFlags, _DGLE_SDK_VER_);
            return true;
        }

        public static bool FreeEngine(IEngineCore pEngineCore, bool bFreeDLL)
        {
            if (pEngineCore != null)
                pFreeEngine(pEngineCore);
            if (bFreeDLL && hServer != IntPtr.Zero)
            {
                Imports.FreeLibrary(hServer);
                hServer = IntPtr.Zero;
                pGetEngine = null;
                pFreeEngine = null;
            }
            return true;
        }

        public static bool GetEngine([MarshalAs(UnmanagedType.LPStr)] string pcDllFileName, out IEngineCore pEngineCore) { return GetEngine(pcDllFileName, out pEngineCore, E_GET_ENGINE_FLAGS.GEF_DEFAULT); }
        public static void FreeEngine() { if (hServer != IntPtr.Zero) { Imports.FreeLibrary(hServer); hServer = IntPtr.Zero; } }

        static class Imports
        {
            [DllImport("kernel32.dll")]
            public static extern IntPtr LoadLibrary(string lpFileName);

            [DllImport("kernel32.dll", CharSet = CharSet.Ansi, ExactSpelling = true)]
            public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

            [DllImport("kernel32.dll", SetLastError = true)]
            public static extern bool FreeLibrary(IntPtr hModule);
        }
    }
}