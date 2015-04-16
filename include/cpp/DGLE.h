/**
\file		DGLE.h
\author		Korotkov Andrey aka DRON
\version	2:0.3.5
\date		XX.XX.XXXX (c)Korotkov Andrey

\brief		Main DGLE engine header.

To use engine you should just include this header to your project.
This header is a part of DGLE SDK.
*/

/** 
\mainpage	DGLE Main Help Page
 
\section	intro_sec Introduction

- DGLE is a powerful cross-platform engine for 2D/3D games and real-time visualizations. Young, strong and crazy!
- DGLE is an open source project and free for use (under the terms of license, see below).
- DGLE is based on widely spread open standards, formats and APIs such as OpenGL, OpenAL, OGG Vorbis, Lua, Box2D, Bullet physics, Mono & GTK# e.t.c..

\image html banner.jpg

This file is official engine documentation for C++ programmers (DGLE users).
This documentation is also suitable for any supported programming language.

\section	aim_sec The aim of the project

The goal of the project is to provide developers with flexible and extendable cross-platform easy-to-learn professional technology,
capable of building any 2D/3D games, real-time visualizations, scientific applications etc. It should be easy to make great projects
only by using editors and scripts or go deeper and use your programming skills of your favorite language to create really exciting projects.
Users can also add new formats and functionality by creating new plugins for engine and share them with others over the web or just get
plugins already made by others. When project is complete it can be easily build for every popular platform.

\section gstart_sec Getting started

Simple steps to make your first DGLE application.

- 1. Include this header ("DGLE.h") to your project.
- 2. Connect DGLE namespace to your project (ex. "using namespace DGLE;").
- 3. Paste "DGLE_DYNAMIC_FUNC" macros to your main source file (ex. "main.cpp").
- 4. Declare pointer to IEngineCore class like "IEngineCore *pEngineCore".
- 5. Call "GetEngine" function to retrieve IEngineCore pointer. For dynamic library it should be like this "GetEngine("DGLE.dll", pEngineCore);".
- 6. Now you can use IEngineCore methods. For example, you can initialize engine like this "pEngineCore->InitializeEngine(NULL, "HelloWorld");".
- 7. After engine initialization you should call "pEngineCore->StartEngine();" to start engine.
- 8. When you are done with engine don't forget to call "FreeEngine()" routine before exit.

Please see "HelloWorld" sample code at the "Examples" tab of this help file.

You can browse ".\src\examples" folder or go to "Examples" tab of this manual for further information.

\section license_sec License

DGLE is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DGLE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.

\section dev_letter_sec The letter from the developers

If you are using DGLE in your project and you like it. Please do not remove DGLE splash screen
and paste the link to the official DGLE website in your program credits section or elsewhere.
It is the easiest way you can say "Thank you for your work!" to DGLE project developers.
Also we would be happy for any donations to support further project development.

\section additional_sec Additional help and information

Visit official DGLE engine website http://dglengine.org/ for additional information and support.

Feel free to write directly to project leader on e-mail hello@dglengine.org.

\example HelloWorld.cpp
This is simplest DGLE application for Windows.
\note To make this sample work you should copy "DGLE.h" header to your project source directory and place DGLE library file (ex. "DGLE.dll" for Windows) in your project output folder. 

*/

#ifndef DGLE_HEADER
#define DGLE_HEADER

//Compiler compatibility tweaks//

#if defined(_MSC_VER)

#	define FORCE_INLINE __forceinline
#	define ENUM_FORWARD_DECLARATION(name) name

#elif defined(__BORLANDC__)

#	define FORCE_INLINE __inline

#	ifdef __CODEGEARC__
#		define ENUM_FORWARD_DECLARATION(name) name : uint32
#	else
#		define ENUM_FORWARD_DECLARATION(name) name
#	endif

#	define sqrtf sqrt
#	define sinf sin
#	define cosf cos
#	define atan2f atan2
#	define acosf acos

#endif

//Engine version defines//

/** Defines DGLE version string. 
	\warning Do not edit!
*/
#define _DGLE_VER_ "2:0.3.5"
 
/** Defines DGLE version integer. 
	\warning Do not edit!
*/
#define _DGLE_SDK_VER_ 1

/** Defines the current version of plugin SDK. 
	\warning Do not edit!
*/
#define _DGLE_PLUGIN_SDK_VER_ _DGLE_SDK_VER_

//Platform definition macros//

#if defined(_WIN32) || defined(_WIN64)

//Platform Windows//

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#include <Windows.h>

/** Internal engine define, shows that target platform is Windows.*/
#define PLATFORM_WINDOWS

/** If defined, all interfaces will be derived from IUnknown for compatibility with Microsoft COM technology. */
#define DGLE_USE_COM

/** Define calling convention used by engine. */
#define DGLE_API APIENTRY

/** If defined, all structures will be aligned by 1 byte. */
#define STRUCT_ALIGNMENT_1

#else//_WIN32 or _WIN64

//Unknown platform//
#error Unknown platform!

#endif

#include "DGLE_Types.h"

/** Engines main namespace. */
namespace DGLE
{

//Engine Base interface//

	// {DFB1F52B-D906-4108-AD6F-3144E224688A}
	static const GUID IID_IDGLE_Base = 
	{ 0xdfb1f52b, 0xd906, 0x4108, { 0xad, 0x6f, 0x31, 0x44, 0xe2, 0x24, 0x68, 0x8a } };

	/** Engine base fundamental interface.
		Any engine interface must be derived from this interface.
		\attention On Windows platform IDGLE_Base is derived from IUnknown for more flexibility and compatibility,
		but DGLE doesn't provides real COM technology. The reference counter is always 1,
		"Release" and "AddRef" methods are dummies, "QueryInterface" can return pointer only
		to IUnknown or to the last interface in the inheritance chain.
	*/
	class IDGLE_Base
#if defined(PLATFORM_WINDOWS) && defined(DGLE_USE_COM)
		: public IUnknown
#endif
	{
	public:
		/** Returns unique identifier of the last interface in the inheritance chain.
			\param[out] guid Unique interface identifier.
			\return Always returns DGLE_Types.h::S_OK.
		 */		
		virtual DGLE_RESULT DGLE_API GetGUID(GUID &guid) = 0;
		
		/** Executes some command using its index or bitmask. Commands are specific for concrete interface. All commands should be described in documentation.
			\param[in] uiCmd Command index or some bitmask. These values must be gotten from documentation.
			\param[in, out] stVar Variant with additional command parameters and for storing command result.
			\return E_NOTIMPL indicates that interface has not got any commands.
			\note If command returns any TVariant with allocated data inside then command with index -1 should delete any allocated memory inside interface.
		*/
		virtual DGLE_RESULT DGLE_API ExecuteCommand(uint uiCmd, TVariant &stVar) = 0;
			
		/** Executes some text command and returns result as variant. Commands are specific for concrete interface. All commands should be described in documentation.
			\param[in] pcCommand Pointer to allocated string with command.
			\param[in, out] stVar Variant with additional command parameters and for storing command result.
			\return E_NOTIMPL indicates that interface has not got any commands.
		*/
		virtual DGLE_RESULT DGLE_API ExecuteTextCommand(const char *pcCommand, TVariant &stVar) = 0;

		/** Executes some text command and returns result as string. Commands are specific for concrete interface. All commands should be described in documentation.
			\param[in] pcCommand Pointer to allocated string with command.
			\param[out] pcResult Pointer to allocated string to accept command result.
			\param[in, out] uiCharsCount Count of the chars in allocated result string.
			\return E_INVALIDARG must be returned if allocated string is too small, uiCharsCount will contain required string length. E_NOTIMPL indicates that interface has not got any commands.
			\note If pcResult for command is NULL then uiCharsCount will contain recommended result string length and command should not be executed.
		*/
		virtual DGLE_RESULT DGLE_API ExecuteTextCommandEx(const char *pcCommand, char *pcResult, uint &uiCharsCount) = 0;
	};

//Engine SubSystem Interface//

	/** Engine subsystems types. */
	enum E_ENGINE_SUB_SYSTEM
	{
		ESS_RENDER = 0,			/**< Main rendering subsystem. \see IRender */
		ESS_INPUT,				/**< Input subsystem. \see IInput */
		ESS_SOUND,				/**< Sound subsystem. \see ISound */
		ESS_RESOURCE_MANAGER,	/**< Resource manager subsystem. \see IResourceManager */
		ESS_FILE_SYSTEM,		/**< Main file system(manager of virtual file systems). \see IMainFileSystem */
		ESS_CORE_RENDERER		/**< Low-level base rendering interface. \see ICoreRenderer */
	};

	// {C682F875-E0BD-4af9-B79C-E209850025F8}
	static const GUID IID_IEngineSubSystem = 
	{ 0xc682f875, 0xe0bd, 0x4af9, { 0xb7, 0x9c, 0xe2, 0x9, 0x85, 0x0, 0x25, 0xf8 } };

	/** Base interface of any engine subsystem. */
	class IEngineSubSystem : public IDGLE_Base
	{
	public:
		/** Returns type of subsystem. 
			\param[out] eSubSystemType Type of the subsystem to which you may cast this interface pointer.
			\return Always returns DGLE_Types.h::S_OK.	
		*/
		virtual DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType) = 0;
	};

//Engine Plugin Interface//

	// {B94E0E40-8885-41dd-8BC5-4CD663AE5709}
	static const GUID IID_IPlugin = 
	{ 0xb94e0e40, 0x8885, 0x41dd, { 0x8b, 0xc5, 0x4c, 0xd6, 0x63, 0xae, 0x57, 0x9 } };

	/** Base interface of any engine plugin. */
	class IPlugin : public IDGLE_Base
	{
	public:
		/** Returns structure with plugin description.
			\param[out] stInfo Structure in which plugin description will be stored.
		*/
		virtual DGLE_RESULT DGLE_API GetPluginInfo(TPluginInfo &stInfo) = 0;
		
		/** Returns the name of interface which plugin implements or empty string if it implements nothing.
			\param[out] pcName Pointer to allocated string.
			\param[in, out] uiCharsCount Count of the chars in allocated string.
			\return E_INVALIDARG must be returned if allocated string is too small.
			\note If pcName is NULL then uiCharsCount will contain length of the text to allocate.
		*/
		virtual DGLE_RESULT DGLE_API GetPluginInterfaceName(char* pcName, uint &uiCharsCount) = 0;
	};

//Engine Subsystem Plugin Interface//

	// {27908E31-8D8E-4076-AE33-087A1BE5DCB3}
	static const GUID IID_ISubSystemPlugin = 
	{ 0x27908e31, 0x8d8e, 0x4076, { 0xae, 0x33, 0x8, 0x7a, 0x1b, 0xe5, 0xdc, 0xb3 } };

	/** Base interface of any engine core subsystem plugin. */
	class ISubSystemPlugin : public IPlugin
	{
	public:
		/** Returns interface of subsystem implemented in this plugin.
			\param[out] prSubSystem Interface of the subsystem.
		*/
		virtual DGLE_RESULT DGLE_API GetSubSystemInterface(IEngineSubSystem *&prSubSystem) = 0;
	};

//Engine Base Object Interface//

	/** Types of engine objects. */
	enum E_ENGINE_OBJECT_TYPE
	{
		EOT_UNKNOWN = 0,		/**< Undefined or custom object type. */
		EOT_TEXTURE,			/**< Texture represents any basic raster data. \see ITexture*/
		EOT_MATERIAL,			/**< Material is a combination of textures, colors and other settings of how 3D object will be rendered in scene. \see IMaterial */
		EOT_LIGHT,				/**< Light is source of lighting for 3D. \see ILight */
		EOT_MESH,				/**< Mesh is an atomic basic geometry unit. \see IMesh */
		EOT_MODEL,				/**< Model is a composition of meshes with materials. Could contain animation and levels of detail. \see IModel*/
		EOT_BITMAP_FONT,		/**< Bitmap font is a simple 2D raster font for common purpose. \see IBitmapFont */
		EOT_SOUND_SAMPLE,		/**< Sound sample is a container of sound wave which could be streamed to sound device. \see ISoundSample */
		EOT_MUSIC,				/**< Music is some kind of large streamable sound sample with runtime hardware decoding. \see IMusic*/

		EOT_EMPTY				/**< For empty or dummy objects. \note This enum must be always last in the list. */
	};

	// {C010239A-6457-40f5-87EF-FAA3156CE6E2}
	static const GUID IID_IEngineBaseObject = 
	{ 0xc010239a, 0x6457, 0x40f5, { 0x87, 0xef, 0xfa, 0xa3, 0x15, 0x6c, 0xe6, 0xe2 } };

	/** Base interface of any engine object. 
		Engine objects are commonly loaded from files by Resource Manager subsystem.
		\see IResourceManager
	*/
	class IEngineBaseObject : public IDGLE_Base
	{
	public:
		/** Releases object and deallocates memory. Also removes it from IResourceManager lists. 
			After calling Free() method you can safely null the pointer to the object.
		*/
		virtual DGLE_RESULT DGLE_API Free() = 0;

		/** Returns type of object. 
			\param[out] eObjType Type of the object to which you may cast this interface pointer.
			\return Always returns DGLE_Types.h::S_OK.	
		*/
		virtual DGLE_RESULT DGLE_API GetType(E_ENGINE_OBJECT_TYPE &eObjType) = 0;

		/** In case object type is EOT_UNKNOWN, you can use this function to get specific object type id.
			\param[out] uiObjUnknownType Integer with unique object type index. Meaning of these indexes must be provided by the developer of specific object type.
			\return Returns DGLE_Types.h::S_FALSE if object is not of EOT_UNKNOWN type and DGLE_Types.h::S_OK otherwise.	
		*/
		virtual DGLE_RESULT DGLE_API GetUnknownType(uint &uiObjUnknownType) = 0;
	};

//Events Interfaces//

	/** Types of engine events. 
		\see IBaseEvent
	 */
	enum E_EVENT_TYPE
	{
		ET_UNKNOWN = 0,				/**< Undefined or custom event type. */ 
		ET_BEFORE_INITIALIZATION,	/**< Event occurs just before engine will call its initialization routines. \see IEvBeforeInitialization */ 
		ET_BEFORE_RENDER,			/**< Event occurs before every frame. */ 
		ET_AFTER_RENDER,			/**< Event occurs after every frame. */ 
		ET_ON_PROFILER_DRAW,		/**< It is a special event on which you can render some text information on screen. \note If you want to output some statistic or profiling information use this event and special RenderProfilerText method. \see IEngineCore::RenderProfilerText */ 
		ET_ON_WINDOW_MESSAGE,		/**< Event occurs every time when window receives message. Use this event to hook engine window messages. \see IEvWindowMessage */ 
		ET_ON_GET_SUBSYSTEM,		/**< Event occurs when someone calls IEngineCore::GetSubSystem method and you can substitute any subsystem by your own realization. \see IEvGetSubSystem */ 
		ET_ON_ENGINE_FATAL_MESSAGE,	/**< Event occurs on engine fatal error. \see IEvFatalMessage */
		ET_ON_CONSOLE_WRITE,		/**< Event occurs when some text is being outputted to the engine console. \see IEvConsoleWrite */
		ET_ON_FULLSCREEN,			/**< Event occurs when engine is switching to fullscreen mode or back to windowed from fullscreen. \see IEvGoFullScreen */
		ET_ON_PER_SECOND_TIMER		/**< Event occurs every second, just before engine recalculates its current per second metrics. */
	};

	// {6DFEF982-AADF-42e9-A369-378BDB31404A}
	static const GUID IID_IBaseEvent = 
	{ 0x6dfef982, 0xaadf, 0x42e9, { 0xa3, 0x69, 0x37, 0x8b, 0xdb, 0x31, 0x40, 0x4a } };

	/** Base interface of any engine event. 
		\see IEngineCore::AddEventListener, IEngineCore::RemoveEventListener
	 */
	class IBaseEvent : public IDGLE_Base
	{
	public:
		/** Returns type of event. 
			\param[out] eEvType Type of the event. You may cast this interface pointer to special event interface if such exists.
			\return Always returns DGLE_Types.h::S_OK.	
		*/
		virtual DGLE_RESULT DGLE_API GetEventType(E_EVENT_TYPE &eEvType) = 0;

		/** In case event type is ET_UNKNOWN, you can use this function to get specific event type id.
			\param[out] uiUnknEvType Integer with unique event type index. Meaning of these indexes must be provided by the developer of specific event type.
			\return Returns DGLE_Types.h::S_FALSE if event is not of ET_UNKNOWN type and DGLE_Types.h::S_OK otherwise.	
		*/
		virtual DGLE_RESULT DGLE_API GetUnknownEventType(uint &uiUnknEvType) = 0;
	};

	// {EB735739-3D12-4522-B6D7-EEE3225DF934}
	static const GUID IID_IEvBeforeInitialization = 
	{ 0xeb735739, 0x3d12, 0x4522, { 0xb6, 0xd7, 0xee, 0xe3, 0x22, 0x5d, 0xf9, 0x34 } };

	enum ENUM_FORWARD_DECLARATION(E_ENGINE_INIT_FLAGS);

	/** Event occurs just before engine will call its initialization routines.
		On this event you can hook engine init Parameters.
		\see ET_BEFORE_INITIALIZATION, IEngineCore::InitializeEngine, IBaseEvent
	 */
	class IEvBeforeInitialization : public IBaseEvent
	{
	public:
		/** Sets new engine initialization parameters.
			\param[in] stWindowParam New engine window structure to replace current.
			\param[in] eInitFlags New engine initialization flags to replace current.
		 */
		virtual DGLE_RESULT DGLE_API SetParameters(const TEngineWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags) = 0;

		/** Retrieves current engine initialization parameters. 
			\param[in] stWindowParam Current engine window structure.
			\param[in] eInitFlags Current engine initialization flags.
		 */
		virtual DGLE_RESULT DGLE_API GetParameters(TEngineWindow &stWindowParam, E_ENGINE_INIT_FLAGS eInitFlags) = 0;
	};
	
	// {9E35969A-B0D4-4E5A-A89B-1A5AAD057028}
	static const GUID IID_IEvConsoleWrite = 
	{ 0x9e35969a, 0xb0d4, 0x4e5a, { 0xa8, 0x9b, 0x1a, 0x5a, 0xad, 0x5, 0x70, 0x28 } };

	/** Event occurs when some text is being added to the engine console.
		\see ET_ON_CONSOLE_WRITE
	 */
	class IEvConsoleWrite : public IBaseEvent
	{
	public:
		/** Returns console text.
		 \param[out] pcTxt Pointer to allocated string.
		 \param[in, out] uiCharsCount Count of the chars in allocated string.
		 \param[out] bToPrevLine Should text replace previous console line or add new one.
		 \return E_INVALIDARG must be returned if allocated string is too small.
		 \note If pcTxt is NULL then uiCharsCount will contain the length of the text to allocate.
		 */		
		virtual DGLE_RESULT DGLE_API GetText(char *pcTxt, uint &uiCharsCount, bool &bToPrevLine) = 0;
	};

	// {DAA4E3BC-C958-4def-B603-F63EEC908226}
	static const GUID IID_IEvFatalMessage = 
	{ 0xdaa4e3bc, 0xc958, 0x4def, { 0xb6, 0x3, 0xf6, 0x3e, 0xec, 0x90, 0x82, 0x26 } };

	/** Event occurs on engine fatal error.
		Also application errors such as "Access Violation" are handled by this event if EIF_CATCH_UNHANDLED
		flag is set in IEngineCore::InitializeEngine initialization flags.
		By handling this event 
		\see ET_ON_ENGINE_FATAL_MESSAGE, LT_FATAL, IBaseEvent
	 */
	class IEvFatalMessage : public IBaseEvent
	{
	public:
		/** Returns the fatal message text.
		 \param[out] pcTxt Pointer to allocated string.
		 \param[in, out] uiCharsCount Count of the chars in allocated string.
		 \return E_INVALIDARG must be returned if allocated string is too small.
		 \note If pcTxt is NULL then uiCharsCount will contain the length of the text to allocate.
		 */		
		virtual DGLE_RESULT DGLE_API GetMessageText(char *pcTxt, uint &uiCharsCount) = 0;

		/** Suspends all engine threads and pauses all engine routines. 
			\param[in] bFreeze Suspends if true or resumes if false engine threads and routines.
		 */
		virtual DGLE_RESULT DGLE_API FreezeEngine(bool bFreeze) = 0;

		/** Forces engine not to show error message and console. 
			\note If you decided not to show error message you should inform user about error somehow.
		 */
		virtual DGLE_RESULT DGLE_API ForceNoMessage() = 0;

		/** Forces engine to ignore current error and tries to continue. 
			\warning Use with care.
		 */
		virtual DGLE_RESULT DGLE_API ForceIgnoreError() = 0;
	};

	// {8D718E48-581D-4cbb-9C40-C04998106F8D}
	static const GUID IID_IEvWindowMessage = 
	{ 0x8d718e48, 0x581d, 0x4cbb, { 0x9c, 0x40, 0xc0, 0x49, 0x98, 0x10, 0x6f, 0x8d } };

	/** Event occurs every time when window receives message. 
		Use this event to hook window messages.
		\see ET_ON_WINDOW_MESSAGE, IBaseEvent
	 */
	class IEvWindowMessage : public IBaseEvent
	{
	public:
		/** Retrieves window message.
			\param[out] stWinMsg Structure with current message information.
		 */
		virtual DGLE_RESULT DGLE_API GetMessage(TWindowMessage &stWinMsg) = 0;
	};

	// {2B6D2547-716E-490c-B1F1-422CB428738F}
	static const GUID IID_IEvGetSubSystem = 
	{ 0x2b6d2547, 0x716e, 0x490c, { 0xb1, 0xf1, 0x42, 0x2c, 0xb4, 0x28, 0x73, 0x8f } };

	/** Event occurs when someone calls IEngineCore::GetSubSystem method.
		You can substitute any subsystem by your own realization on this event.
		\see ET_ON_GET_SUBSYSTEM, IBaseEvent
	 */
	class IEvGetSubSystem : public IBaseEvent
	{
	public:
		/** Returns subsystem type which user is trying to retrieve. 
			\param[out] eSubSystem Type of retrieving subsystem.
		 */
		virtual DGLE_RESULT DGLE_API GetSubSystemType(E_ENGINE_SUB_SYSTEM &eSubSystem) = 0;

		/** Substitutes engine subsystem by custom one. 
			\param[in] pSubSystem Pointer to subsystem interface with which retrieving subsystem will be substituted.
		 */		
		virtual DGLE_RESULT DGLE_API OverrideSubSystem(IEngineSubSystem *pSubSystem) = 0;
	};

	// {CEC9184C-74D9-4739-BF48-BB800467665B}
	static const GUID IID_IEvGoFullScreen = 
	{ 0xcec9184c, 0x74d9, 0x4739, { 0xbf, 0x48, 0xbb, 0x80, 0x4, 0x67, 0x66, 0x5b } };

	/** Event occurs when engine is going fullscreen or go back to windowed mode from fullscreen mode.
		On this event you can adjust display resolution.
		\note If you want to prevent engine from going fullscreen mode on hotkey combination see EWF_RESTRICT_FULLSCREEN_HOTKEY flag.
		\see ET_ON_FULLSCREEN, IBaseEvent, EWF_RESTRICT_FULLSCREEN_HOTKEY
	 */
	class IEvGoFullScreen : public IBaseEvent
	{
	public:
		/** Get display resolution or window size (when switching from fullscreen mode) to be set by engine.
			\param[out] uiScreenWidth Display resolution width or window width in pixels.
			\param[out] uiScreenHeight Display resolution height or window height in pixels.
			\param[out] bGoFullScreen If true engine is switching to fullscreen mode or to windowed mode in other case.
		 */		
		virtual DGLE_RESULT DGLE_API GetResolution(uint &uiScreenWidth, uint &uiScreenHeight, bool &bGoFullScreen) = 0;

		/** Adjust display resolution or window size (when switching from fullscreen mode).
			\param[in] uiScreenWidth New display resolution width or window width in pixels.
			\param[in] uiScreenHeight New display resolution height or window height in pixels.
		*/
		virtual DGLE_RESULT DGLE_API SetResolution(uint uiScreenWidth, uint uiScreenHeight) = 0;
	};

//Main Engine System//

	// {371B1338-BB25-4B8C-BD6A-BCDF241CC52C}
	static const GUID IID_IEngineCallback = 
	{ 0x371b1338, 0xbb25, 0x4b8c, { 0xbd, 0x6a, 0xbc, 0xdf, 0x24, 0x1c, 0xc5, 0x2c } };

	class IEngineCallback : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API Initialize() = 0;
		virtual DGLE_RESULT DGLE_API Free() = 0;
		virtual DGLE_RESULT DGLE_API Update(uint uiDeltaTime) = 0;
		virtual DGLE_RESULT DGLE_API Render() = 0;
		virtual DGLE_RESULT DGLE_API OnEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent) = 0;
	};

	/** Type of engine callbacks. 
		IEngineCore can register callbacks of these types.
		\see IEngineCore::AddProcedure, IEngineCore::RemoveProcedure
	 */
	enum E_ENGINE_PROCEDURE_TYPE
	{
		EPT_UPDATE = 0,		/**< Procedure is called periodically(like on timer event). Interval of calling is set on engine initialization. In this procedure you should do any application computes. \see IEngineCore::InitializeEngine */
		EPT_RENDER,			/**< Procedure is called when engine decides to draw new frame. In this procedure you can call any rendering routines. */
		EPT_INIT,			/**< Procedure is called before engine will start its main loop. In this procedure you should load all resources needed by your application. */
		EPT_FREE			/**< Procedure is called before engine quits. In this procedure you should release all resources and free memory. */
	};

	/** Type of engine log message.
		\see IEngineCore::WriteToLogEx
	 */
	enum E_LOG_TYPE
	{
		LT_INFO = 0,	/**< Information message do not need user attention. */
		LT_WARNING,		/**< Information message do not need user attention but marks in engine log as warning and increments warnings counter. */
		LT_ERROR,		/**< Error message do not need user attention but marks in engine log as error and increments errors counter. Error message also includes information about source file and line. */
		LT_FATAL		/**< Fatal message displays user error message and then terminates engine with exit code 2. Also ET_ON_ENGINE_FATAL_MESSAGE event is generated. */
	};

	/** Engine initialization flags. 
		\see IEngineCore::InitializeEngine
	 */
	enum ENUM_FORWARD_DECLARATION(E_ENGINE_INIT_FLAGS)
	{
		EIF_DEFAULT					= 0x00000000,	/**< Use default settings. */
		EIF_CATCH_UNHANDLED			= 0x00000001,	/**< All user callbacks will be executed in safe mode and engine will catch any unhandled errors. Engine will convert cached errors to engine fatal errors. Also ET_ON_ENGINE_FATAL_MESSAGE event will be generated. */
		EIF_FORCE_NO_SOUND			= 0x00000002,	/**< Sound subsystem will not be initialized. */
		EIF_LOAD_ALL_PLUGINS		= 0x00000004,	/**< Engine will try to connect any found plugin files found in "plugins" folder near it. \note Ext plugin is connected automatically without this flag as well. */
		EIF_FORCE_LIMIT_FPS			= 0x00000010,	/**< Engine will limit its FPS (frames per second) not to overload CPU and overheat GPU. \note Recommended for casual games, 2D and simple 3D games and desktop applications. */
		EIF_FORCE_16_BIT_COLOR		= 0x00000020,	/**< Forces engine to use 16 bit color depth instead of 32 bit by default. \note Not recommended. */
		EIF_ENABLE_FLOATING_UPDATE	= 0x00000040,	/**< By default engine uses fixed update mechanism, this means that engine will try to keep fixed update time interval, whenever it's possible. When this flag is set the update routine simply will be called once when delta time between updates is greater than update interval (for example, even if delta time is twice greater than update interval), so you should use delta time value. \see EPT_UPDATE, IEngineCore::GetLastUpdateDeltaTime, EIF_ENABLE_PER_FRAME_UPDATE */
		EIF_ENABLE_PER_FRAME_UPDATE	= 0x00000100,	/**< By default engine uses fixed update mechanism, this means that engine will try to keep fixed update time interval, whenever it's possible. When this flag is set the update routine simply will be called once before rendering of every frame, so you should use IEngineCore::GetElapsedTime and IEngineCore::GetTimer to manage application logic by yourself. \note In this case IEngineCore::GetLastUpdateDeltaTime will always return zero values on high FPS rates, so it is recommended to turn VSync on. \see TEngineWindow, EPT_UPDATE, EIF_ENABLE_FLOATING_UPDATE */
		EIF_FORCE_NO_WINDOW			= 0x00000200,	/**< Engine will be initialized without window. There will be no rendering, input and update routines. Useful for tools and utilities. \warning You must call IEngineCore::StartEngine and IEngineCore::QuitEngine routines for correct engine initialization and finalization. */
		EIF_NO_SPLASH				= 0x10000000	/**< This flag will disable engine splash screen. Splash screen is displayed to the user while engine prepare itself and while user initialization procedure is being processed. \note Turning off splash screen is not recommended because the user could be confused while being waiting application execution. */
	};

	// {111BB884-2BA6-4e84-95A5-5E4700309CBA}
	static const GUID IID_IEngineCore = 
	{ 0x111bb884, 0x2ba6, 0x4e84, { 0x95, 0xa5, 0x5e, 0x47, 0x0, 0x30, 0x9c, 0xba } };

	/** Main engine interface.
		Pointer to this interface is retrieved directly from the DGLE library.
		\see DGLE_DYNAMIC_FUNC
	 */
	class IEngineCore : public IDGLE_Base
	{
	public:
		/** Set engine splash window picture.
		 \param[in] pcBmpFileName File name of the BMP file with picture to be set.
		 \note You can use this method only before calling InitializeEngine.
		 */
		virtual DGLE_RESULT DGLE_API LoadSplashPicture(const char *pcBmpFileName) = 0;	

		/** Adds plugin to engine initialization list. This means that plugin will be loaded on engine initialization. This is the only correct way to setup specific Render, Sound, Input or other system plugins.
		 \param[in] pcFileName File name of the plugin.
		 \note Standard extension plugin ("Ext") will be connected automatically (if found), so you don't need to add it to initialization list.
		 \see EIF_LOAD_ALL_PLUGINS
		 */
		virtual DGLE_RESULT DGLE_API AddPluginToInitializationList(const char *pcFileName) = 0;	

		/** Initialize engine and all of its subroutines. Also creates main engine window.
		 \param[in] tHandle Handle of some already created window control to render in or NULL in case to let engine create its own window.
		 \param[in] pcApplicationName Caption of main engine window.
		 \param[in] stWindowParam Structure with some window properties.
		 \param[in] uiUpdateInterval Interval in milliseconds between calling of user update routine. \see EPT_UPDATE
		 \param[in] eInitFlags Special engine configuration flags.
		 */
		virtual DGLE_RESULT DGLE_API InitializeEngine(TWindowHandle tHandle, const char *pcApplicationName, const TEngineWindow &stWindowParam = TEngineWindow(), uint uiUpdateInterval = 33, E_ENGINE_INIT_FLAGS eInitFlags = EIF_DEFAULT) = 0;
		
		/** Change interval of calling user update routine after engine has been started. \see EPT_UPDATE
		 \param[in] uiUpdateInterval Interval in milliseconds.
		 \see InitializeEngine
		 */
		virtual DGLE_RESULT DGLE_API SetUpdateInterval(uint uiUpdateInterval) = 0;	
		virtual DGLE_RESULT DGLE_API StartEngine() = 0;
		virtual DGLE_RESULT DGLE_API QuitEngine() = 0;

		virtual DGLE_RESULT DGLE_API ConnectPlugin(const char *pcFileName, IPlugin *&prPlugin) = 0;	
		virtual DGLE_RESULT DGLE_API DisconnectPlugin(IPlugin *pPlugin) = 0;
		virtual DGLE_RESULT DGLE_API GetPlugin(const char *pcPluginName, IPlugin *&prPlugin) = 0;

		virtual DGLE_RESULT DGLE_API AddEngineCallback(IEngineCallback *pEngineCallback) = 0;
		virtual DGLE_RESULT DGLE_API RemoveEngineCallback(IEngineCallback *pEngineCallback) = 0;

		virtual DGLE_RESULT DGLE_API AddProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter = NULL) = 0;
		virtual DGLE_RESULT DGLE_API RemoveProcedure(E_ENGINE_PROCEDURE_TYPE eProcType, void (DGLE_API *pProc)(void *pParameter), void *pParameter = NULL) = 0;

		virtual DGLE_RESULT DGLE_API CastEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent) = 0;
		virtual DGLE_RESULT DGLE_API AddEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListenerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter = NULL) = 0;
		virtual DGLE_RESULT DGLE_API RemoveEventListener(E_EVENT_TYPE eEventType, void (DGLE_API *pListenerProc)(void *pParameter, IBaseEvent *pEvent), void *pParameter = NULL) = 0;

		virtual DGLE_RESULT DGLE_API GetSubSystem(E_ENGINE_SUB_SYSTEM eSubSystem, IEngineSubSystem *&prSubSystem) = 0;

		virtual DGLE_RESULT DGLE_API RenderFrame() = 0;
		virtual DGLE_RESULT DGLE_API RenderProfilerText(const char *pcTxt, const TColor4 &stColor = ColorWhite()) = 0;
		virtual DGLE_RESULT DGLE_API GetInstanceIndex(uint &uiIdx) = 0;
		virtual DGLE_RESULT DGLE_API GetTimer(uint64 &uiTick) = 0;
		virtual DGLE_RESULT DGLE_API GetSystemInfo(TSystemInfo &stSysInfo) = 0;
		virtual DGLE_RESULT DGLE_API GetCurrentWindow(TEngineWindow &stWin) = 0;
		virtual DGLE_RESULT DGLE_API GetFPS(uint &uiFPS) = 0;
		virtual DGLE_RESULT DGLE_API GetLastUpdateDeltaTime(uint &uiDeltaTime) = 0;
		virtual DGLE_RESULT DGLE_API GetElapsedTime(uint64 &ui64ElapsedTime) = 0;
		virtual DGLE_RESULT DGLE_API GetWindowHandle(TWindowHandle &tHandle) = 0;

		virtual DGLE_RESULT DGLE_API ChangeWindowMode(const TEngineWindow &stNewWin) = 0;
		virtual DGLE_RESULT DGLE_API GetDesktopResolution(uint &uiWidth, uint &uiHeight) = 0;
		virtual DGLE_RESULT DGLE_API AllowPause(bool bAllow) = 0;

		virtual DGLE_RESULT DGLE_API WriteToLog(const char *pcTxt) = 0;
		virtual DGLE_RESULT DGLE_API WriteToLogEx(const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber) = 0;

		virtual DGLE_RESULT DGLE_API ConsoleVisible(bool bIsVisible) = 0;
		virtual DGLE_RESULT DGLE_API ConsoleWrite(const char *pcTxt, bool bWriteToPreviousLine = false) = 0;
		virtual DGLE_RESULT DGLE_API ConsoleExecute(const char *pcCommandTxt) = 0;
		virtual DGLE_RESULT DGLE_API ConsoleRegisterCommand(const char *pcCommandName, const char *pcCommandHelp, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam), void *pParameter = NULL) = 0; 
		virtual DGLE_RESULT DGLE_API ConsoleRegisterVariable(const char *pcCommandName, const char *pcCommandHelp, int *piVar, int iMinValue, int iMaxValue, bool (DGLE_API *pProc)(void *pParameter, const char *pcParam) = NULL, void *pParameter = NULL) = 0;
		virtual DGLE_RESULT DGLE_API ConsoleUnregister(const char *pcCommandName) = 0;

		virtual DGLE_RESULT DGLE_API GetVersion(char *pcBuffer, uint &uiBufferSize) = 0;
	};

//Resource Manager SubSystem//

	class IFile;
	class ITexture;
	class ILight;
	class IMaterial;
	class IModel;
	class IMesh;
	class ISoundSample;

	enum E_TEXTURE_DATA_FORMAT
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

	enum E_TEXTURE_CREATE_FLAGS
	{
		TCF_DEFAULT					= 0x00000000,
		TCF_PIXEL_ALIGNMENT_1		= 0x00000001,//use only if your texture input data is not 4 byte aligned
		TCF_MIPMAPS_PRESENTED		= 0x00000002 //all mip levels must be presented
	};

	enum E_TEXTURE_LOAD_FLAGS
	{
		TLF_FILTERING_NONE			= 0x00000001,
		TLF_FILTERING_BILINEAR		= 0x00000002,
		TLF_FILTERING_TRILINEAR		= 0x00000004,
		TLF_FILTERING_ANISOTROPIC	= 0x00000008,
	
		TLF_DECREASE_QUALITY_MEDIUM	= 0x00000020,
		TLF_DECREASE_QUALITY_HIGH	= 0x00000040,

		TLF_COMPRESS				= 0x00000100,

		TLF_COORDS_REPEAT			= 0x00001000,
		TLF_COORDS_CLAMP			= 0x00002000,
		TLF_COORDS_MIRROR_REPEAT	= 0x00004000,
		TLF_COORDS_MIRROR_CLAMP		= 0x00008000,

		TLF_GENERATE_MIPMAPS		= 0x00040000,

		TLF_ANISOTROPY_2X			= 0x00100000,
		TLF_ANISOTROPY_4X			= 0x00200000,
		TLF_ANISOTROPY_8X			= 0x00400000,
		TLF_ANISOTROPY_16X			= 0x00800000
	};

	enum E_BITMAP_FONT_LOAD_FLAGS
	{
		BFLF_FILTERING_NONE			= 0x00000001,
		BFLF_GENERATE_MIPMAPS		= 0x00000002,
		BFLF_FORCE_ALPHA_TEST_2D	= 0x00000004
	};

	enum E_MESH_CREATE_FLAGS
	{
		MCF_ONLY_DEFAULT_DATA		= 0x00000000,//vertex and index arrays must be presented
		MCF_NORMALS_PRESENTED		= 0x00000001,
		MCF_TEXTURE_COORDS_PRESENTED= 0x00000002,
		MCF_TANGENT_SPACE_PRESENTED	= 0x00000004,
		MCF_VERTEX_DATA_INTERLEAVED	= 0x00000008
	};

	enum E_MESH_MODEL_LOAD_FLAGS
	{
		MMLF_FORCE_SOFTWARE_BUFFER	= 0x00000001,
		MMLF_DYNAMIC_BUFFER			= 0x00000002,
		MMLF_FORCE_MODEL_TO_MESH	= 0x00000004
	};

	enum E_SOUND_SAMPLE_LOAD_FLAGS
	{
		SSLF_LOAD_AS_MUSIC			= 0x00000001
	};

	const uint TEXTURE_LOAD_DEFAULT_2D = (uint)(TLF_FILTERING_BILINEAR | TLF_COORDS_CLAMP);
	const uint TEXTURE_LOAD_DEFAULT_3D = (uint)(TLF_FILTERING_TRILINEAR | TLF_GENERATE_MIPMAPS | TLF_COORDS_REPEAT);

	const uint RES_LOAD_DEFAULT = 0x00000000;

	// {139505B6-5EFC-4f02-A5E8-18CD1FBD69E3}
	static const GUID IID_IResourceManager = 
	{ 0x139505b6, 0x5efc, 0x4f02, { 0xa5, 0xe8, 0x18, 0xcd, 0x1f, 0xbd, 0x69, 0xe3 } };

	class IResourceManager : public IEngineSubSystem
	{
	public:
		virtual DGLE_RESULT DGLE_API CreateTexture(ITexture *&prTex, const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_CREATE_FLAGS eCreateFlags, E_TEXTURE_LOAD_FLAGS eLoadFlags, const char *pcName = "", bool bAddResource = true) = 0;
		virtual DGLE_RESULT DGLE_API CreateMaterial(IMaterial *&prMaterial, const char *pcName = "", bool bAddResource = true) = 0;
		virtual DGLE_RESULT DGLE_API CreateLight(ILight *&prLight, const char *pcName = "", bool bAddResource = true) = 0;
		virtual DGLE_RESULT DGLE_API CreateMesh(IMesh *&prMesh, const uint8 *pData, uint uiDataSize, uint uiNumVerts, uint uiNumFaces, E_MESH_CREATE_FLAGS eCreateFlags, E_MESH_MODEL_LOAD_FLAGS eLoadFlags, const char *pcName = "", bool bAddResource = true) = 0; //pData could be NULL to create empty mesh, index buffer could be empty
		virtual DGLE_RESULT DGLE_API CreateModel(IModel *&prModel, const char *pcName = "", bool bAddResource = true) = 0;
		virtual DGLE_RESULT DGLE_API CreateSound(ISoundSample *&prSndSample, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize, const char *pcName = "", bool bAddResource = true) = 0;
	
		virtual DGLE_RESULT DGLE_API RegisterFileFormat(const char *pcExtension, E_ENGINE_OBJECT_TYPE eObjType, const char *pcDescription, bool (DGLE_API *pLoadProc)(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParameter), void *pParameter = NULL) = 0;
		virtual DGLE_RESULT DGLE_API UnregisterFileFormat(const char *pcExtension) = 0;
		virtual DGLE_RESULT DGLE_API RegisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *pObj) = 0;
		virtual DGLE_RESULT DGLE_API UnregisterDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *pObj) = 0;
		virtual DGLE_RESULT DGLE_API GetRegisteredExtensions(char *pcTxt, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API GetExtensionDescription(const char *pcExtension, char *pcTxt, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API GetExtensionType(const char *pcExtension, E_ENGINE_OBJECT_TYPE &eType) = 0;

		virtual DGLE_RESULT DGLE_API GetResourceByName(const char *pcName, IEngineBaseObject *&prObj) = 0;
		virtual DGLE_RESULT DGLE_API GetResourceByIndex(uint uiIdx, IEngineBaseObject *&prObj) = 0;
		virtual DGLE_RESULT DGLE_API GetResourceName(IEngineBaseObject *pObj, char *pcName, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API GetDefaultResource(E_ENGINE_OBJECT_TYPE eObjType, IEngineBaseObject *&prObj) = 0;
		virtual DGLE_RESULT DGLE_API GetResourcesCount(uint &uiCount) = 0;

		virtual DGLE_RESULT DGLE_API Load(const char *pcFileName, IEngineBaseObject *&prObj, uint uiLoadFlags = RES_LOAD_DEFAULT, const char *pcName = "") = 0;
		virtual DGLE_RESULT DGLE_API LoadEx(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags = RES_LOAD_DEFAULT, const char *pcName = "") = 0;
	
		virtual DGLE_RESULT DGLE_API FreeResource(IEngineBaseObject *&prObj) = 0;
		virtual DGLE_RESULT DGLE_API AddResource(const char *pcName, IEngineBaseObject *pObj) = 0;
		virtual DGLE_RESULT DGLE_API RemoveResource(IEngineBaseObject *pObj, bool &bCanDelete) = 0;
	};

//Render SubSystem//

	class IRender2D;
	class IRender3D;
	class ICoreGeometryBuffer;
	
	struct TDrawDataDesc;

	enum ENUM_FORWARD_DECLARATION(E_CORE_RENDERER_DRAW_MODE);

	enum E_GET_POINT3_MODE
	{
		GP3M_FROM_DEPTH_BUFFER = 0,
		GP3M_FROM_FAR_PLANE,
		GP3M_FROM_NEAR_PLANE
	};

	enum E_BLENDING_EFFECT
	{
		BE_NORMAL = 0,
		BE_ADD,
		BE_MULT,
		BE_BLACK,
		BE_WHITE,
		BE_MASK
	};

	// {EA03C661-A334-4225-B5DB-4C45452CCC41}
	static const GUID IID_IRender = 
	{ 0xea03c661, 0xa334, 0x4225, { 0xb5, 0xdb, 0x4c, 0x45, 0x45, 0x2c, 0xcc, 0x41 } };

	class IRender : public IEngineSubSystem
	{
	public:
		virtual DGLE_RESULT DGLE_API SetClearColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetClearColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API ClearColorBuffer() = 0;
		virtual DGLE_RESULT DGLE_API Unbind(E_ENGINE_OBJECT_TYPE eType) = 0; //use EOT_UNKNOWN to unbind all
		virtual DGLE_RESULT DGLE_API EnableScissor(const TRectF &stArea) = 0;
		virtual DGLE_RESULT DGLE_API DisableScissor() = 0; // when using in 2D must be always inside Begin2D - End2D block
		virtual DGLE_RESULT DGLE_API GetScissor(bool &bEnabled, TRectF &stArea) = 0;
		virtual DGLE_RESULT DGLE_API SetRenderTarget(ITexture *pTargetTex = NULL) = 0;
		virtual DGLE_RESULT DGLE_API GetRenderTarget(ITexture *&prTargetTex) = 0;
		
		virtual DGLE_RESULT DGLE_API GetRender2D(IRender2D *&prRender2D) = 0;
		virtual DGLE_RESULT DGLE_API GetRender3D(IRender3D *&prRender3D) = 0;
	};

	//Render2D interface//

	//Flags for Primitives
	enum E_PRIMITIVE2D_FLAGS
	{
		PF_DEFAULT			= 0x00000000,
		PF_LINE				= 0x00000000,
		PF_FILL				= 0x00000001, 
		PF_VERTICES_COLORS	= 0x00000002
	};

	//Flags for Effects
	enum E_EFFECT2D_FLAGS
	{
		EF_DEFAULT			= 0x00000000,
		EF_NONE				= 0x00000001,
		EF_ALPHA_TEST		= 0x00000002,
		EF_BLEND			= 0x00000004, 
		EF_FLIP_HORIZONTALLY= 0x00000008, 
		EF_FLIP_VERTICALLY	= 0x00000010, 
		EF_COLOR_MIX		= 0x00000020, 
		EF_SCALE			= 0x00000040,  
		EF_VERTICES_OFFSETS	= 0x00000080, 
		EF_VERTICES_COLORS	= 0x00000100, 
		EF_ROTATION_POINT	= 0x00000200,
		EF_TILE_TEXTURE		= 0x00000400
	};

	enum E_BATCH_MODE2D
	{
		BM_AUTO	= 0,
		BM_DISABLED,
		BM_ENABLED_UPDATE_EVERY_TICK,
		BM_ENABLED_UPDATE_EVERY_FRAME
	};

	// {F5F3257A-F8B8-4d91-BA67-451167A8D63F}
	static const GUID IID_IRender2D = 
	{ 0xf5f3257a, 0xf8b8, 0x4d91, { 0xba, 0x67, 0x45, 0x11, 0x67, 0xa8, 0xd6, 0x3f } };

	class IRender2D : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API Begin2D() = 0;
		virtual DGLE_RESULT DGLE_API End2D() = 0;
		virtual DGLE_RESULT DGLE_API BatchRender(E_BATCH_MODE2D eMode) = 0;
		virtual DGLE_RESULT DGLE_API InvalidateBatchData() = 0;
		virtual DGLE_RESULT DGLE_API BeginBatch(bool bUpdateEveryFrame = false) = 0;
		virtual DGLE_RESULT DGLE_API EndBatch() = 0;
		virtual DGLE_RESULT DGLE_API NeedToUpdateBatchData(bool &bNeedUpdate) = 0;
		virtual DGLE_RESULT DGLE_API SetResolutionCorrection(uint uiResX, uint uiResY, bool bConstantProportions = true) = 0; //Set resx and resy to current screen size to turn off correction
		virtual DGLE_RESULT DGLE_API ResolutionCorrectToAbsolute(const TPoint2 &stLogicCoord, TPoint2 &stAbsoluteCoord) = 0;
		virtual DGLE_RESULT DGLE_API AbsoluteToResolutionCorrect(const TPoint2 &stAbsoluteCoord, TPoint2 &stLogicCoord) = 0;
		virtual DGLE_RESULT DGLE_API SetCamera(const TPoint2 &stCenter, float fAngle = 0.f, const TVector2 &stScale = TVector2(1.f, 1.f)) = 0;
		virtual DGLE_RESULT DGLE_API ResetCamera() = 0;
		virtual DGLE_RESULT DGLE_API UnprojectCameraToScreen(const TPoint2 &stCameraCoord, TPoint2 &stScreenCoord) = 0;
		virtual DGLE_RESULT DGLE_API ProjectScreenToCamera(const TPoint2 &stScreenCoord, TPoint2 &stCameraCoord) = 0;
		virtual DGLE_RESULT DGLE_API CullBoundingBox(const TRectF &stBBox, float fAngle, bool &bCull) = 0;

		// 2D Primitives
		virtual DGLE_RESULT DGLE_API SetLineWidth(uint uiWidth) = 0;
		virtual DGLE_RESULT DGLE_API DrawPoint(const TPoint2 &stCoords, const TColor4 &stColor = ColorWhite(), uint uiSize = 1) = 0;
		virtual DGLE_RESULT DGLE_API DrawLine(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TColor4 &stColor = ColorWhite(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawRectangle(const TRectF &stRect, const TColor4 &stColor = ColorWhite(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawCircle(const TPoint2 &stCoords, uint uiRadius, uint uiQuality, const TColor4 &stColor = ColorWhite(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawEllipse(const TPoint2 &stCoords, const TVector2 &stRadius, uint uiQuality, const TColor4 &stColor = ColorWhite(), E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawPolygon(ITexture *pTexture, const TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;

		// 2D Sprites
		virtual DGLE_RESULT DGLE_API DrawTexture(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawTextureCropped(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, const TRectF &stTexCropRect, float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawTextureSprite(ITexture *pTexture, const TPoint2 &stCoords, const TVector2 &stDimensions, uint uiFrameIndex, float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT) = 0;

		// Extra
		virtual DGLE_RESULT DGLE_API DrawTriangles(ITexture *pTexture, const TVertex2 *pstVertices, uint uiVerticesCount, E_PRIMITIVE2D_FLAGS eFlags = PF_DEFAULT) = 0;
		virtual DGLE_RESULT DGLE_API DrawMesh(IMesh *pMesh, ITexture *pTexture, const TPoint2 &stCoords, const TVector3 &stDimensions, const TVector3 &stAxis = TVector3(), float fAngle = 0.f, E_EFFECT2D_FLAGS eFlags = EF_DEFAULT, bool bClip = true, float fFovY = 90.f, bool bClearDepthBuffer = false) = 0;

		//Advanced
		virtual DGLE_RESULT DGLE_API Draw(ITexture *pTexture, const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags) = 0;
		virtual DGLE_RESULT DGLE_API DrawBuffer(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, const TRectF &stAABB, E_EFFECT2D_FLAGS eFlags) = 0;
		virtual DGLE_RESULT DGLE_API DrawBuffer3D(ITexture *pTexture, ICoreGeometryBuffer *pBuffer, E_EFFECT2D_FLAGS eFlags, const TMatrix4x4 &stTransform, const TPoint3 &stCenter, const TVector3 &stExtents, bool bClip, float fFovY, bool bClearDepthBuffer) = 0;

		//Effects
		virtual DGLE_RESULT DGLE_API SetRotationPoint(const TPoint2 &stCoords) = 0;//In texture coord system
		virtual DGLE_RESULT DGLE_API SetScale(const TPoint2 &stScale) = 0;
		virtual DGLE_RESULT DGLE_API SetColorMix(const TColor4 &stColor = ColorWhite()) = 0;
		virtual DGLE_RESULT DGLE_API SetBlendMode(E_BLENDING_EFFECT eMode = BE_NORMAL) = 0;
		virtual DGLE_RESULT DGLE_API SetVerticesOffsets(const TPoint2 &stCoords1, const TPoint2 &stCoords2, const TPoint2 &stCoords3, const TPoint2 &stCoords4) = 0;
		virtual DGLE_RESULT DGLE_API SetVerticesColors(const TColor4 &stColor1, const TColor4 &stColor2, const TColor4 &stColor3, const TColor4 &stColor4) = 0;

		virtual DGLE_RESULT DGLE_API GetRotationPoint(TPoint2 &stCoords) = 0;
		virtual DGLE_RESULT DGLE_API GetScale(TPoint2 &stScale) = 0;
		virtual DGLE_RESULT DGLE_API GetColorMix(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetBlendMode(E_BLENDING_EFFECT &eMode) = 0;
		virtual DGLE_RESULT DGLE_API GetVerticesOffsets(TPoint2 &stCoords1, TPoint2 &stCoords2, TPoint2 &stCoords3, TPoint2 &stCoords4) = 0;
		virtual DGLE_RESULT DGLE_API GetVerticesColors(TColor4 &stColor1, TColor4 &stColor2, TColor4 &stColor3, TColor4 &stColor4) = 0;
	};

	//Render3D interface//

	// {5275F43A-4FF9-48b2-B88E-B2F842461AB3}
	static const GUID IID_IRender3D = 
	{ 0x5275f43a, 0x4ff9, 0x48b2, { 0xb8, 0x8e, 0xb2, 0xf8, 0x42, 0x46, 0x1a, 0xb3 } };

	class IRender3D : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API SetPerspective(float fFovAngle, float fZNear, float fZFar) = 0;
		virtual DGLE_RESULT DGLE_API GetPerspective(float &fFovAngle, float &fZNear, float &fZFar) = 0;

		virtual DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetColor(TColor4 &stColor) = 0;

		virtual DGLE_RESULT DGLE_API BindTexture(ITexture *pTex, uint uiTextureLayer) = 0;
		virtual DGLE_RESULT DGLE_API GetTexture(ITexture *&prTex, uint uiTextureLayer) = 0;

		virtual DGLE_RESULT DGLE_API GetMaxLightsPerPassCount(uint &uiCount) = 0;
		virtual DGLE_RESULT DGLE_API UpdateLight(ILight *pLight) = 0;

		virtual DGLE_RESULT DGLE_API BindMaterial(IMaterial *pMat) = 0;
		virtual DGLE_RESULT DGLE_API GetMaterial(IMaterial *&prMat) = 0;

		virtual DGLE_RESULT DGLE_API ToggleBlending(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API IsBlendingEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetBlendMode(E_BLENDING_EFFECT eMode = BE_NORMAL) = 0;
		virtual DGLE_RESULT DGLE_API GetBlendMode(E_BLENDING_EFFECT &eMode) = 0;

		virtual DGLE_RESULT DGLE_API ToggleAlphaTest(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetAlphaTreshold(float fTreshold) = 0;
		virtual DGLE_RESULT DGLE_API IsAlphaTestEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetAlphaTreshold(float &fTreshold) = 0;

		virtual DGLE_RESULT DGLE_API ClearDepthBuffer() = 0;
		virtual DGLE_RESULT DGLE_API ToggleDepthTest(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API IsDepthTestEnabled(bool &bEnabled) = 0;

		virtual DGLE_RESULT DGLE_API ToggleBackfaceCulling(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API IsBackfaceCullingEnabled(bool &bEnabled) = 0;

		virtual DGLE_RESULT DGLE_API Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount) = 0;
		virtual DGLE_RESULT DGLE_API DrawBuffer(ICoreGeometryBuffer *pBuffer) = 0;

		virtual DGLE_RESULT DGLE_API ToggleFog(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetLinearFogBounds(float fStart, float fEnd) = 0;
		virtual DGLE_RESULT DGLE_API SetFogColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API IsFogEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetLinearFogBounds(float &fStart, float &fEnd) = 0;
		virtual DGLE_RESULT DGLE_API GetFogColor(TColor4 &stColor) = 0;

		virtual DGLE_RESULT DGLE_API SetMatrix(const TMatrix4x4 &stMatrix) = 0;
		virtual DGLE_RESULT DGLE_API MultMatrix(const TMatrix4x4 &stMatrix) = 0;
		virtual DGLE_RESULT DGLE_API PushMatrix() = 0;
		virtual DGLE_RESULT DGLE_API PopMatrix() = 0;
		virtual DGLE_RESULT DGLE_API GetMatrix(TMatrix4x4 &stMatrix) = 0;

		virtual DGLE_RESULT DGLE_API DrawAxes(float fSize = 1.f, bool bNoDepthTest = false) = 0;

		virtual DGLE_RESULT DGLE_API ResetStates() = 0; 
		virtual DGLE_RESULT DGLE_API PushStates() = 0;
		virtual DGLE_RESULT DGLE_API PopStates() = 0;

		virtual DGLE_RESULT DGLE_API GetPoint3(const TPoint2 &stPointOnScreen, TPoint3 &stResultPoint, E_GET_POINT3_MODE eFlag = GP3M_FROM_DEPTH_BUFFER) = 0;
		virtual DGLE_RESULT DGLE_API GetPoint2(const TPoint3 &stPoint, TPoint2 &stResultPointOnScreen) = 0;

		virtual DGLE_RESULT DGLE_API SetupFrustum() = 0;
		virtual DGLE_RESULT DGLE_API CullPoint(const TPoint3 &stCoords, bool &bCull) = 0;
		virtual DGLE_RESULT DGLE_API CullSphere(const TPoint3 &stCenter, float fRadius, bool &bCull) = 0;
		virtual DGLE_RESULT DGLE_API CullBox(const TPoint3 &stCenter, const TVector3 &stExtents, bool &bCull) = 0;

		virtual DGLE_RESULT DGLE_API ToggleLighting(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetGlobalAmbientLighting(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API IsLightingEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetGlobalAmbientLighting(TColor4 &stColor) = 0;
	};

	//Light interface//

	enum E_LIGHT_TYPE
	{
		LT_DIRECTIONAL = 0,
		LT_POINT,
		LT_SPOT
	};

	// {EB73AC84-A465-4554-994D-8BED29744C9D}
	static const GUID IID_ILight = 
	{ 0xeb73ac84, 0xa465, 0x4554, { 0x99, 0x4d, 0x8b, 0xed, 0x29, 0x74, 0x4c, 0x9d } };

	class ILight: public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API SetEnabled(bool bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API SetColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API SetPosition(const TPoint3 &stPos) = 0;
		virtual DGLE_RESULT DGLE_API SetDirection(const TVector3 &stDir) = 0;
		virtual DGLE_RESULT DGLE_API SetRange(float fRange) = 0;
		virtual DGLE_RESULT DGLE_API SetIntensity(float fIntensity) = 0;
		virtual DGLE_RESULT DGLE_API SetSpotAngle(float fAngle) = 0;
		virtual DGLE_RESULT DGLE_API SetType(E_LIGHT_TYPE eType) = 0;

		virtual DGLE_RESULT DGLE_API GetEnabled(bool &bEnabled) = 0;
		virtual DGLE_RESULT DGLE_API GetColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetPosition(TPoint3 &stPos) = 0;
		virtual DGLE_RESULT DGLE_API GetDirection(TVector3 &stDir) = 0;
		virtual DGLE_RESULT DGLE_API GetRange(float &fRange) = 0;
		virtual DGLE_RESULT DGLE_API GetIntensity(float &fIntensity) = 0;
		virtual DGLE_RESULT DGLE_API GetSpotAngle(float &fAngle) = 0;
		virtual DGLE_RESULT DGLE_API GetType(E_LIGHT_TYPE &eType) = 0;

		virtual DGLE_RESULT DGLE_API Update() = 0;
	};

	//Texture interface//

	class ICoreTexture;

	// {85BDDBC2-F126-4cae-946D-7D6B079E5CCE}
	static const GUID IID_ITexture = 
	{ 0x85bddbc2, 0xf126, 0x4cae, { 0x94, 0x6d, 0x7d, 0x6b, 0x7, 0x9e, 0x5c, 0xce } };

	class ITexture : public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API GetDimensions(uint &uiWidth, uint &uiHeight) = 0;
		virtual DGLE_RESULT DGLE_API SetFrameSize(uint uiFrameWidth, uint uiFrameHeight) = 0;
		virtual DGLE_RESULT DGLE_API GetFrameSize(uint &uiFrameWidth, uint &uiFrameHeight) = 0;
		virtual DGLE_RESULT DGLE_API FramesCount(uint &uiCount) = 0;

		virtual DGLE_RESULT DGLE_API GetCoreTexture(ICoreTexture *&prCoreTex) = 0;

		virtual DGLE_RESULT DGLE_API Draw2DSimple(int iX, int iY, uint uiFrameIndex = 0) = 0;
		virtual DGLE_RESULT DGLE_API Draw2D(int iX, int iY, uint uiWidth, uint uiHeight, float fAngle = 0.f, uint uiFrameIndex = 0) = 0;
		virtual DGLE_RESULT DGLE_API Draw3D(uint uiFrameIndex = 0) = 0;

		virtual DGLE_RESULT DGLE_API Bind(uint uiTextureLayer = 0) = 0;
	};

	//Material interface//

	// {B6506749-BB41-423d-B6C0-982081EF63F9}
	static const GUID IID_IMaterial = 
	{ 0xb6506749, 0xbb41, 0x423d, { 0xb6, 0xc0, 0x98, 0x20, 0x81, 0xef, 0x63, 0xf9 } };

	class IMaterial: public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API SetDiffuseColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API SetSpecularColor(const TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API SetShininess(float fShininess) = 0;
		virtual DGLE_RESULT DGLE_API SetDiffuseTexture(ITexture *pTexture) = 0;
		virtual DGLE_RESULT DGLE_API SetBlending(bool bEnabled, E_BLENDING_EFFECT eMode) = 0;
		virtual DGLE_RESULT DGLE_API SetAlphaTest(bool bEnabled, float fTreshold) = 0;

		virtual DGLE_RESULT DGLE_API GetDiffuseColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetSpecularColor(TColor4 &stColor) = 0;
		virtual DGLE_RESULT DGLE_API GetShininess(float &fShininess) = 0;
		virtual DGLE_RESULT DGLE_API GetDiffuseTexture(ITexture *&prTexture) = 0;
		virtual DGLE_RESULT DGLE_API GetBlending(bool &bEnabled, E_BLENDING_EFFECT &eMode) = 0;
		virtual DGLE_RESULT DGLE_API GetAlphaTest(bool &bEnabled, float &fTreshold) = 0;

		virtual DGLE_RESULT DGLE_API Bind() = 0;
	};

	//BitmapFont interface//

	// {0B03E8D7-23A3-4c79-9E82-5BC6E50E1EBA}
	static const GUID IID_IBitmapFont = 
	{ 0xb03e8d7, 0x23a3, 0x4c79, { 0x9e, 0x82, 0x5b, 0xc6, 0xe5, 0xe, 0x1e, 0xba } };

	class IBitmapFont : public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API GetTexture(ITexture *&prTexture) = 0;
		virtual DGLE_RESULT DGLE_API SetScale(float fScale) = 0;
		virtual DGLE_RESULT DGLE_API GetScale(float &fScale) = 0;
		virtual DGLE_RESULT DGLE_API GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight) = 0;
		virtual DGLE_RESULT DGLE_API Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor = ColorWhite()) = 0;
		virtual DGLE_RESULT DGLE_API Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor = ColorWhite(), float fAngle = 0, bool bVerticesColors = false) = 0;
		virtual DGLE_RESULT DGLE_API Draw3D(const char *pcTxt) = 0;
	};

	//3D Objects interfaces//
	
	//Mesh interface//

	// {85E360A8-07B3-4f22-AA29-07C7FC7C6893}
	static const GUID IID_IMesh = 
	{ 0x85e360a8, 0x7b3, 0x4f22, { 0xaa, 0x29, 0x7, 0xc7, 0xfc, 0x7c, 0x68, 0x93 } };

	class IMesh : public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API Draw() = 0;
		virtual DGLE_RESULT DGLE_API GetCenter(TPoint3 &stCenter) = 0;
		virtual DGLE_RESULT DGLE_API GetExtents(TVector3 &stExtents) = 0;
		virtual DGLE_RESULT DGLE_API GetTrianglesCount(uint &uiCnt) = 0;
		virtual DGLE_RESULT DGLE_API GetGeometryBuffer(ICoreGeometryBuffer *&prBuffer) = 0;
		virtual DGLE_RESULT DGLE_API SetGeometryBuffer(ICoreGeometryBuffer *pBuffer, bool bFreeCurrentBuffer) = 0;
		virtual DGLE_RESULT DGLE_API RecalculateNormals(bool bInvert = false) = 0;
		virtual DGLE_RESULT DGLE_API RecalculateTangentSpace() = 0;
		virtual DGLE_RESULT DGLE_API RecalculateBounds() = 0;
		virtual DGLE_RESULT DGLE_API TransformVertices(const TMatrix4x4 &stTransMatrix) = 0;
		virtual DGLE_RESULT DGLE_API Optimize() = 0;
		virtual DGLE_RESULT DGLE_API GetOwner(IModel *&prModel) = 0;
		virtual DGLE_RESULT DGLE_API SetOwner(IModel *pModel) = 0;
	};

	//Multi mesh interface//

	// {6107C296-FC07-48d1-B6A7-F88CC2DAE897}
	static const GUID IID_IModel = 
	{ 0x6107c296, 0xfc07, 0x48d1, { 0xb6, 0xa7, 0xf8, 0x8c, 0xc2, 0xda, 0xe8, 0x97 } };

	class IModel : public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API Draw() = 0;
		virtual DGLE_RESULT DGLE_API DrawMesh(uint uiMeshIdx) = 0;
		virtual DGLE_RESULT DGLE_API GetCenter(TPoint3 &stCenter) = 0;
		virtual DGLE_RESULT DGLE_API GetExtents(TVector3 &stExtents) = 0;
		virtual DGLE_RESULT DGLE_API MeshesCount(uint &uiCount) = 0;
		virtual DGLE_RESULT DGLE_API GetMesh(uint uiMeshIdx, IMesh *&prMesh) = 0;
		virtual DGLE_RESULT DGLE_API SetModelMaterial(IMaterial *pMaterial) = 0;
		virtual DGLE_RESULT DGLE_API GetModelMaterial(IMaterial *&prMaterial) = 0;
		virtual DGLE_RESULT DGLE_API SetMeshMaterial(uint uiMeshIdx, IMaterial *pMaterial) = 0;
		virtual DGLE_RESULT DGLE_API GetMeshMaterial(uint uiMeshIdx, IMaterial *&prMaterial) = 0;
		virtual DGLE_RESULT DGLE_API AddMesh(IMesh *pMesh) = 0;
		virtual DGLE_RESULT DGLE_API RemoveMesh(IMesh *pMesh) = 0;
		virtual DGLE_RESULT DGLE_API ReplaceMesh(uint uiMeshIdx, IMesh *pMesh) = 0;
	};

//Input SubSystem//
	
	enum E_INPUT_CONFIGURATION_FLAGS
	{
		ICF_DEFAULT					= 0x00000000,
		ICF_EXCLUSIVE				= 0x00000001,
		ICF_HIDE_CURSOR				= 0x00000002,
		ICF_CURSOR_BEYOND_SCREEN	= 0x00000004
	};

	// {64DAAF7F-F92C-425f-8B92-3BE40D8C6666}
	static const GUID IID_IInput = 
	{ 0x64daaf7f, 0xf92c, 0x425f, { 0x8b, 0x92, 0x3b, 0xe4, 0xd, 0x8c, 0x66, 0x66 } };

	class IInput : public IEngineSubSystem
	{
	public:
		virtual DGLE_RESULT DGLE_API Configure(E_INPUT_CONFIGURATION_FLAGS eFlags = ICF_DEFAULT) = 0;

		virtual DGLE_RESULT DGLE_API GetMouseStates(TMouseStates &stMStates) = 0;
		virtual DGLE_RESULT DGLE_API GetKey(E_KEYBOARD_KEY_CODES eKeyCode, bool &bPressed) = 0;
		virtual DGLE_RESULT DGLE_API GetKeyName(E_KEYBOARD_KEY_CODES eKeyCode, uchar &cASCIICode) = 0;

		virtual DGLE_RESULT DGLE_API BeginTextInput(char* pcBuffer, uint uiBufferSize) = 0;
		virtual DGLE_RESULT DGLE_API EndTextInput() = 0;
		
		virtual DGLE_RESULT DGLE_API GetJoysticksCount(uint &uiCount) = 0;
		virtual DGLE_RESULT DGLE_API GetJoystickName(uint uiJoyId, char *pcName, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API GetJoystickStates(uint uiJoyId, TJoystickStates &stJoyStates) = 0;
	};

//Sound SubSystem interfaces//

	class ISoundChannel;

	// {054C07EE-2724-42f2-AC2B-E81FCF5B4ADA}
	static const GUID IID_ISound = 
	{ 0x54c07ee, 0x2724, 0x42f2, { 0xac, 0x2b, 0xe8, 0x1f, 0xcf, 0x5b, 0x4a, 0xda } };

	class ISound : public IEngineSubSystem
	{
	public:
		virtual DGLE_RESULT DGLE_API SetMasterVolume(uint uiVolume) = 0;
		virtual DGLE_RESULT DGLE_API MasterPause(bool bPaused) = 0;
		virtual DGLE_RESULT DGLE_API StopAllChannels() = 0;
		virtual DGLE_RESULT DGLE_API GetMaxChannelsCount(uint &uiCount) = 0;
		virtual DGLE_RESULT DGLE_API GetFreeChannelsCount(uint &uiCount) = 0;
		virtual DGLE_RESULT DGLE_API ReleaseChannelsByData(const uint8 *pData) = 0;	
		virtual DGLE_RESULT DGLE_API ReleaseChannelsByCallback(void (DGLE_API *pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize)) = 0;	
		// pData presents samples in 16-bit signed little-endian PCM format
		virtual DGLE_RESULT DGLE_API CreateChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, const uint8 *pData, uint32 ui32DataSize) = 0; //Data not copied!
		virtual DGLE_RESULT DGLE_API CreateStreamableChannel(ISoundChannel *&prSndChnl, uint uiSamplesPerSec, uint uiBitsPerSample, bool bStereo, uint32 ui32DataSize, void (DGLE_API *pStreamCallback)(void *pParameter, uint32 ui32DataPos, uint8 *pBufferData, uint uiBufferSize) /*callback is being called from separate thread*/, void *pParameter) = 0;
	};

	//SoundSample interface//

	// {DE6F7CDD-8262-445c-8D20-68E3324D99A6}
	static const GUID IID_ISoundChannel = 
	{ 0xde6f7cdd, 0x8262, 0x445c, { 0x8d, 0x20, 0x68, 0xe3, 0x32, 0x4d, 0x99, 0xa6 } };

	class ISoundChannel : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API Play(bool bLooped) = 0;
		virtual DGLE_RESULT DGLE_API Pause() = 0;
		virtual DGLE_RESULT DGLE_API Stop() = 0;
		virtual DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying) = 0;
		virtual DGLE_RESULT DGLE_API SetVolume(uint uiVolume) = 0; //from 0 to 100
		virtual DGLE_RESULT DGLE_API GetVolume(uint &uiVolume) = 0;
		virtual DGLE_RESULT DGLE_API SetPan(int iPan) = 0; //from -100 to 100
		virtual DGLE_RESULT DGLE_API GetPan(int &iPan) = 0;
		virtual DGLE_RESULT DGLE_API SetSpeed(uint uiSpeed) = 0;//in percents
		virtual DGLE_RESULT DGLE_API GetSpeed(uint &uiSpeed) = 0;
		virtual DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos) = 0;
		virtual DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos) = 0;
		virtual DGLE_RESULT DGLE_API GetLength(uint &uiLength) = 0;
		virtual DGLE_RESULT DGLE_API IsStreamable(bool &bStreamable) = 0;
		virtual DGLE_RESULT DGLE_API Unaquire() = 0;
	};
	
	enum E_SOUND_SAMPLE_PARAMS
	{
		SSP_NONE	= 0x00000000,
		SSP_LOOPED	= 0x00000001
	};

	// {30DD8C94-D3FA-40cf-9C49-649211424919}
	static const GUID IID_ISoundSample = 
	{ 0x30dd8c94, 0xd3fa, 0x40cf, { 0x9c, 0x49, 0x64, 0x92, 0x11, 0x42, 0x49, 0x19 } };

	class ISoundSample : public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API Play(int iPan = 0) = 0;
		virtual DGLE_RESULT DGLE_API PlayEx(ISoundChannel *&pSndChnl, E_SOUND_SAMPLE_PARAMS eFlags = SSP_NONE) = 0; //pSndChnl must be checked on null
		virtual DGLE_RESULT DGLE_API SetVolume(uint uiVolume) = 0;
		virtual DGLE_RESULT DGLE_API GetVolume(uint &uiVolume) = 0;
	};

	//Music interface//

	// {81F1E67B-3FEB-4ab1-9AD2-D27C4E662164}
	static const GUID IID_IMusic = 
	{ 0x81f1e67b, 0x3feb, 0x4ab1, { 0x9a, 0xd2, 0xd2, 0x7c, 0x4e, 0x66, 0x21, 0x64 } };

	class IMusic : public IEngineBaseObject
	{
	public:
		virtual DGLE_RESULT DGLE_API Play(bool bLooped = true) = 0;
		virtual DGLE_RESULT DGLE_API Pause(bool bPaused) = 0;
		virtual DGLE_RESULT DGLE_API Stop() = 0;
		virtual DGLE_RESULT DGLE_API IsPlaying(bool &bIsPlaying) = 0;
		virtual DGLE_RESULT DGLE_API SetVolume(uint uiVolume) = 0;
		virtual DGLE_RESULT DGLE_API GetVolume(uint &uiVolume) = 0;
		virtual DGLE_RESULT DGLE_API SetCurrentPosition(uint uiPos) = 0;
		virtual DGLE_RESULT DGLE_API GetCurrentPosition(uint &uiPos) = 0;
		virtual DGLE_RESULT DGLE_API GetLength(uint &uiLength) = 0;
	};

//FileSystem SubSystem//

	class IFileSystem;

	// {4850286F-4770-4bcf-A90A-33D7BE41E686}
	static const GUID IID_IMainFileSystem = 
	{ 0x4850286f, 0x4770, 0x4bcf, { 0xa9, 0xa, 0x33, 0xd7, 0xbe, 0x41, 0xe6, 0x86 } };

	class IMainFileSystem : public IEngineSubSystem
	{
	public:
		virtual DGLE_RESULT DGLE_API LoadFile(const char* pcFileName, IFile *&prFile) = 0;// c:\data.zip|img.jpg
		virtual DGLE_RESULT DGLE_API FreeFile(IFile *&prFile) = 0;
		virtual DGLE_RESULT DGLE_API GetVirtualFileSystem(const char *pcVFSExtension/*NULL to get HDD file system*/, IFileSystem *&prVFS) = 0;
		virtual DGLE_RESULT DGLE_API RegisterVirtualFileSystem(const char* pcVFSExtension, const char *pcDescription, IFileSystem *pVFS, void (DGLE_API *pDeleteDGLE_API)(void *pParameter, IFileSystem *pVFS), void *pParameter = NULL) = 0;
		virtual DGLE_RESULT DGLE_API UnregisterVirtualFileSystem(const char* pcVFSExtension) = 0;
		virtual DGLE_RESULT DGLE_API GetRegisteredVirtualFileSystems(char* pcTxt, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API GetVirtualFileSystemDescription(const char* pcVFSExtension, char* pcTxt, uint &uiCharsCount) = 0;
	};

	enum E_FIND_FLAGS
	{
		FF_RECURSIVE = 1
	};

	enum E_FILE_SYSTEM_OPEN_FLAGS
	{
		FSOF_READ	= 0x00000001,	
		FSOF_WRITE	= 0x00000002,	
		FSOF_TRUNC	= 0x00000004,	
		FSOF_BINARY	= 0x00000008	
	};

	enum E_FILE_SYSTEM_SEEK_FLAG
	{
		FSSF_BEGIN = 0,
		FSSF_CURRENT,
		FSSF_END
	};

	//File interface//

	// {AE6E8AE7-3E5B-4bc4-A512-42E1CF1DF005}
	static const GUID IID_IFile = 
	{ 0xae6e8ae7, 0x3e5b, 0x4bc4, { 0xa5, 0x12, 0x42, 0xe1, 0xcf, 0x1d, 0xf0, 0x5 } };

	class IFile : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API Read(void *pBuffer, uint uiCount, uint &uiRead) = 0;
		virtual DGLE_RESULT DGLE_API Write(const void *pBuffer, uint uiCount, uint &uiWritten) = 0;
		virtual DGLE_RESULT DGLE_API Seek(uint32 ui32Offset, E_FILE_SYSTEM_SEEK_FLAG eWay, uint32 &ui32Position) = 0;
		virtual DGLE_RESULT DGLE_API GetSize(uint32 &ui32Size) = 0;
		virtual DGLE_RESULT DGLE_API IsOpen(bool &bOpened) = 0;
		virtual DGLE_RESULT DGLE_API GetName(char *pcName, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API GetPath(char *pcPath, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API Free() = 0;
	};

	//FileIterator interface//

	// {5D73F249-0E74-4cc5-9646-270CB1E22750}
	static const GUID IID_IFileIterator = 
	{ 0x5d73f249, 0xe74, 0x4cc5, { 0x96, 0x46, 0x27, 0xc, 0xb1, 0xe2, 0x27, 0x50 } };

	class IFileIterator : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API FileName(char *pcName, uint &uiCharsCount) = 0;
		virtual DGLE_RESULT DGLE_API Next() = 0;
		virtual DGLE_RESULT DGLE_API Free() = 0;
	};

	//FileSystem interface//

	// {2DAE578E-9636-4fae-BABB-7D835EEA7518}
	static const GUID IID_IFileSystem = 
	{ 0x2dae578e, 0x9636, 0x4fae, { 0xba, 0xbb, 0x7d, 0x83, 0x5e, 0xea, 0x75, 0x18 } };

	class IFileSystem : public IDGLE_Base
	{
	public:
		virtual DGLE_RESULT DGLE_API OpenFile(const char *pcName, E_FILE_SYSTEM_OPEN_FLAGS eFlags, IFile *&prFile) = 0;	// if only filepath is passed, i.e. "C:\MyFolder\" then it creates directory
		virtual DGLE_RESULT DGLE_API DeleteFile(const char *pcName) = 0; // if only filepath is passed, then it deletes directory
		virtual DGLE_RESULT DGLE_API FileExists(const char *pcName, bool &bExists) = 0;// if only filepath is passed, then it verifies existence of directory
		virtual DGLE_RESULT DGLE_API Find(const char *pcMask, E_FIND_FLAGS eFlags, IFileIterator *&prIterator) = 0;
	};

	enum E_GET_ENGINE_FLAGS
	{
		GEF_DEFAULT				= 0x00000000,	
		GEF_FORCE_SINGLE_THREAD	= 0x00000001,	
		GEF_FORCE_NO_LOG_FILE	= 0x00000002,
		GEF_FORCE_QUIT			= 0x00000004
	};

}//namespace DGLE

// Engine initialization macroses //

/** \def DGLE_EXTERN_FUNC Macros you can insert in any cpp file to use CreateEngine and FreeEngine functions.
	\note In your main cpp file you must include DGLE_STATIC_FUNC or DGLE_DYNAMIC_FUNC anyway. This macros provides two functions CreateEngine and FreeEngine.
	\warning If you have more than one instance of engine you must set bFreeLib Parameter to true only when releasing the last engine instance.
	\see E_GET_ENGINE_FLAGS
*/
#define DGLE_EXTERN_FUNC \
extern bool CreateEngine(DGLE::IEngineCore *&pEngineCore, DGLE::E_GET_ENGINE_FLAGS eFlags = DGLE::GEF_DEFAULT);\
extern bool FreeEngine(DGLE::IEngineCore *pEngineCore, bool bFreeLib = false);

/** \def DGLE_STATIC_FUNC Macros you must insert in your main cpp file to use CreateEngine and FreeEngine functions.
	\note This macros is used when engine is linked statically from object file. This macros provides two functions CreateEngine and FreeEngine. Parameter bFreeLib for FreeEngine is not used.
	\see E_GET_ENGINE_FLAGS
*/
#define DGLE_STATIC_FUNC \
DGLE_EXTERN_FUNC\
error Static linking is not implemented!

/** \def DGLE_DYNAMIC_FUNC Macros you must insert in your main cpp file to use GetEngine, CreateEngine and FreeEngine functions.
	\note This macros is used when engine is linked dynamically from library file (ex. DLL for Windows). This macros provides three functions GetEngine, CreateEngine and FreeEngine. First one must be called to load engine from dynamic library.
	\see E_GET_ENGINE_FLAGS
*/
#define DGLE_DYNAMIC_FUNC \
bool (CALLBACK *pCreateEngine)(DGLE::IEngineCore *&pEngineCore, DGLE::E_GET_ENGINE_FLAGS eFlags, DGLE::uint8 ubtSDKVer) = NULL;\
bool (CALLBACK *pFreeEngine)(DGLE::IEngineCore *pEngineCore) = NULL;\
HMODULE hServer = NULL;\
bool CreateEngine(DGLE::IEngineCore *&pEngineCore, DGLE::E_GET_ENGINE_FLAGS eFlags = DGLE::GEF_DEFAULT)\
{\
	if (pCreateEngine == NULL)\
		return false;\
	return (*pCreateEngine)(pEngineCore, eFlags, _DGLE_SDK_VER_);\
}\
bool FreeEngine(DGLE::IEngineCore *pEngineCore = NULL, bool bFreeLib = true)\
{\
	if (pEngineCore)\
		(*pFreeEngine)(pEngineCore);\
	if (bFreeLib && hServer)\
	{\
		::FreeLibrary(hServer);\
		hServer = NULL;\
		pCreateEngine = NULL;\
		pFreeEngine = NULL;\
	}\
	return true;\
}\
bool GetEngine(const char *pcDllFileName, DGLE::IEngineCore *&pEngineCore, DGLE::E_GET_ENGINE_FLAGS eFlags = DGLE::GEF_DEFAULT)\
{\
	if (hServer == NULL)\
	{\
		pEngineCore = NULL;\
		if (hServer == NULL)\
		{\
			hServer = ::LoadLibraryA(pcDllFileName);\
			if (hServer == NULL) return false;\
		}\
		if (pCreateEngine == NULL && pFreeEngine == NULL)\
		{\
			pCreateEngine = reinterpret_cast<bool (CALLBACK *)(DGLE::IEngineCore *&, DGLE::E_GET_ENGINE_FLAGS, DGLE::uint8)>\
				(::GetProcAddress(hServer,("CreateEngine")));\
			pFreeEngine = reinterpret_cast<bool (CALLBACK *)(DGLE::IEngineCore *)>\
				(::GetProcAddress(hServer,("FreeEngine")));\
			if (pCreateEngine == NULL || pFreeEngine == NULL)\
			{\
				::FreeLibrary(hServer);\
				hServer = NULL;\
				return false;\
			}\
		}\
	}\
	if (hServer) return CreateEngine(pEngineCore, eFlags);\
	return false;\
}

// Helper macroses //

/** Macros checks DGLE_RESULT value and throws exception of DGLE_RESULT type if value is failed.
	\param[in] res DGLE_RESULT value to be checked.
	\see FAILED
*/
#define CHECK_RES(res) { DGLE_RESULT res_ = res; if (FAILED(res_)) throw res_; }

/** \def PARANOIC_CHECK_RES(res) Macros checks DGLE_RESULT value and drop assertion on any value except S_OK.
	Useful for debugging. After skipping assertion always returns S_OK. In release macros do nothing. 
	\param[in] res DGLE_RESULT value to be checked.
	\code
	PARANOIC_CHECK_RES(pEngineCore->StartEngine());
	\endcode
*/
#ifdef _DEBUG
#	define PARANOIC_CHECK_RES(res) assert(res == S_OK), S_OK
#else
#	define PARANOIC_CHECK_RES(res) res
#endif

//Implementation macroses//

#ifdef DGLE_USE_COM

#define INTERFACE_IMPL(interface_name, next) \
	if(memcmp(&riid, &IID_##interface_name, sizeof(GUID)) == 0)\
		*ppvObject = static_cast<interface_name *>(this);\
	else\
		next

#define INTERFACE_IMPL_END return E_NOINTERFACE;

#define IUNKNOWN_IMPL(interface_impl_list) \
	HRESULT CALLBACK QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)\
	{\
		*ppvObject = NULL;\
		if (memcmp(&riid, &__uuidof(IUnknown), sizeof(GUID)) == 0)\
			*ppvObject = static_cast<IUnknown *>(this);\
		else\
			interface_impl_list\
		return S_OK;\
	}\
	ULONG CALLBACK AddRef() { return 1; }\
	ULONG CALLBACK Release() { return 1; }

#else//DGLE_USE_COM

#define INTERFACE_IMPL(interface_name, next)
#define INTERFACE_IMPL_END
#define IUNKNOWN_IMPL(interface_impl_list)

#endif

#define IDGLE_BASE_DUMMY_COMMANDS_IMPL \
	DGLE_RESULT DGLE_API ExecuteCommand(uint uiCmd, TVariant &stVar)\
	{\
		stVar.Clear();\
		return E_NOTIMPL;\
	}\
	DGLE_RESULT DGLE_API ExecuteTextCommand(const char *pcCommand, TVariant &stVar)\
	{\
		stVar.Clear();\
		return E_NOTIMPL;\
	}\
	DGLE_RESULT DGLE_API ExecuteTextCommandEx(const char *pcCommand, char *pcResult, uint &uiCharsCount)\
	{\
		if (!pcCommand)\
			return E_INVALIDARG;\
		if (!pcResult)\
		{\
			uiCharsCount = 1;\
			return S_OK;\
		}\
		if (uiCharsCount < 1)\
			return E_INVALIDARG;\
		else\
		{\
			if (pcResult && uiCharsCount > 0)\
				strcpy(pcResult, "");\
			else\
				pcResult = NULL;\
			uiCharsCount = 0;\
			return E_NOTIMPL;\
		}\
	}

#define IDGLE_BASE_GUID_IMPL(interface_name) \
	DGLE_RESULT DGLE_API GetGUID(GUID &guid)\
	{\
		guid = IID_##interface_name;\
		return S_OK;\
	}\

/** \def IDGLE_BASE_IMPLEMENTATION(interface_name) Macros inserts realization of IDGLE_Base interface into class body.
	Can be used with interfaces inherited from IDGLE_Base.
	\param[in] interface_name Name of the last interface in inheritance chain from IDGLE_Base.
	\param[in] interface_impl_list List of all other interfaces names of inheritance chain. List is generated via define INTERFACE_IMPL.
	\note It also inserts IUnknown implementation for Windows builds with COM support turned on.
*/
#define IDGLE_BASE_IMPLEMENTATION(interface_name, interface_impl_list) \
	IDGLE_BASE_DUMMY_COMMANDS_IMPL\
	IDGLE_BASE_GUID_IMPL(interface_name)\
	IUNKNOWN_IMPL(INTERFACE_IMPL(IDGLE_Base, INTERFACE_IMPL(interface_name, interface_impl_list)))

#endif//DGLE_HEADER