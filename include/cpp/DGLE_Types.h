/**
\file		DGLE_types.h
\author		Korotkov Andrey aka DRON
\version	2:0.3.0
\date		xx.xx.2012 (c)Korotkov Andrey

\brief		Engine types definition header.

This header is a part of DGLE SDK.

\warning	Don't include this header directly, include "DGLE.h" instead.
\see		DGLE.h
*/

#ifndef DGLE_TYPES
#define DGLE_TYPES

#ifndef DGLE_HEADER
#error Don't include this header directly, include "DGLE.h" instead.
#endif

#define _USE_MATH_DEFINES
#include <Math.h>
#include <stack>

#ifndef PLATFORM_WINDOWS

//DGLE_RESULT//

/** Engine interface unique identifier. 
	Every engine interface must have it's own GUID.
*/
struct GUID
{
	unsigned long int	Data1;
	unsigned short int	Data2;
	unsigned short int	Data3;
	unsigned char		Data4[8];
};

/** Default return type for all DGLE methods. 
	Every engine interface method returnes DGLE_RESULT as result.
	DGLE_RESULT must be one of these types S_OK, S_FALSE, E_FAIL, E_INVALIDARG, E_ABORT or E_NOTIMPL.
	\note Engine suppresses a lot of errors and keeps all things stable, so it is not necessary to check every methods result.
	\see SUCCEEDED, FAILED, CHECK_RES, PARANOIC_CHECK_RES, S_OK, S_FALSE, E_FAIL, E_INVALIDARG, E_ABORT, E_NOTIMPL
*/
typedef long int DGLE_RESULT;

//DGLE_RESULT return values//

/** Return value of DGLE_RESULT type. Method returns S_OK when all of its routines were done succesfully. */
#define S_OK			((DGLE_RESULT)0L)

/** Return value of DGLE_RESULT type. Method returns S_FALSE when most of its routines were done succesfully but some minor errors occured. */
#define S_FALSE			((DGLE_RESULT)1L)

/** Return value of DGLE_RESULT type. Method returns E_FAIL when unspecified error occured. */
#define E_FAIL			((DGLE_RESULT)0x80004005L)

/** Return value of DGLE_RESULT type. Method returns E_INVALIDARG when one or more arguments are invalid. */
#define E_INVALIDARG	((DGLE_RESULT)0x80070057L)

/** Return value of DGLE_RESULT type. Method returns E_ABORT when operation aborted due to errors of some other function. */
#define E_ABORT			((DGLE_RESULT)0x80004004L)

/** Return value of DGLE_RESULT type. Method returns E_NOTIMPL when it is empty dummy or this routine is unsupported by realisation. */
#define E_NOTIMPL		((DGLE_RESULT)0x80004001L)

/** Macros to check DGLE_RESULT value. Returns true if value is S_OK or S_FALSE and false otherwise.
	\param[in] hr DGLE_RESULT value to be checked.
	\code
	if(SUCCEEDED(pEngineCore->InitializeEngine(NULL, "Test"))) pEngineCore->StartEngine();
	\endcode
*/
#define SUCCEEDED(res) (((DGLE_RESULT)(res)) >= 0)

/** Macros to check DGLE_RESULT value. Returns true for any value except S_OK and S_FALSE.
	\param[in] hr DGLE_RESULT value to be checked.
	\code
	if(FAILED(pEngineCore->InitializeEngine(NULL, "Test"))) Exit(1);
	\endcode
*/
#define FAILED(res) (((DGLE_RESULT)(res)) < 0)

#else

#define DGLE_RESULT HRESULT

#endif

namespace DGLE
{

//Some standart types redifinition//

	//signed//
	typedef short int			int16;
	typedef long int			int32;
	typedef signed char			int8;
	typedef wchar_t				wchar;
	
	//unsigned//
	typedef unsigned int		uint;
	typedef unsigned short int	uint16;
	typedef unsigned long int	uint32;
	typedef unsigned char		uint8;
	typedef uint8				uchar;

	//int64//
	typedef signed long long	int64;
	typedef unsigned long long	uint64;

#if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
#pragma pack( push, 1 )
#endif

//Platform depended types//

#ifdef PLATFORM_WINDOWS

	typedef HWND TWinHandle;
	typedef HDC TWinDrawHandle;
	typedef bool TCRendererInitResult;

#endif

	/** Flags of TWinMessage structure that determines type of the message. 
		\see TWinMessage
	 */
	enum E_WINDOW_MESSAGE_TYPE
	{
		WMT_UNKNOWN = 0,/**< This type will be returned if there is no proper conversion from platform specific window event to engine window message. Message parameters have platform specific usage. \note For Windows pParam3 of TWinMessage structure will point to the MSG structure. */
		WMT_REDRAW,		/**< Message indicates that window must redraw itself. Message parameters are not used. */
		WMT_PRESENT,	/**< Message indicates that window is ready and now will be displayed to the user for the first time. Message parameters are not used. */
		WMT_CLOSE,		/**< Message indicates that window is ready to be destroyed and now will disappear from the screen. Message parameters are not used. */
		WMT_CREATE,		/**< Message indicates that window is being created. Message parameters are not used. */
		WMT_DESTROY,	/**< Message indicates that window is ready to be destroyed. Message parameters are not used. */
		WMT_RELEASED,	/**< Message indicates that window was destroyed and released successfully. Message parameters are not used. */
		WMT_ACTIVATED,	/**< Message indicates that window became a foreground window and get user input focus. If ui32Param1 value is TWinHandle of the whindow which loses focus. */
		WMT_DEACTIVATED,/**< Message indicates that window became a background window and lost user input focus. If ui32Param1 value is TWinHandle of the whindow which gets focus. */
		WMT_MINIMIZED,	/**< Message indicates that window was minimized. Message parameter ui32Param1 stores width of the window and ui32Param2 stores height. \note Under Windows pParam3 points to RECT structure with window size. */
		WMT_RESTORED,	/**< Message indicates that window was restored to its normal state. Message parameter ui32Param1 stores width of the window and ui32Param2 stores height. \note Under Windows pParam3 points to RECT structure with window size. */
		WMT_MOVE,		/**< Message indicates that window is being moved. Message parameter ui32Param1 stores x coordinate of upper left window corner and ui32Param2 stores y coordinate. \note Under Windows pParam3 points to RECT structure with window size. */
		WMT_SIZE,		/**< Message indicates that window is being sized. Message parameter ui32Param1 stores width of the window and ui32Param2 stores height. \note Under Windows pParam3 points to RECT structure with window size. */
		WMT_KEY_UP,		/**< Message indicates that the user has released some keyboard key. Message parameter ui32Param1 is the engine code of the key being released. \see E_KEYBOARD_KEY_CODES */
		WMT_KEY_DOWN,	/**< Message indicates that the user has pressed some keyboard key. Message parameter ui32Param1 is the engine code of the key being pressed. \see E_KEYBOARD_KEY_CODES */
		WMT_ENTER_CHAR, /**< Message indicates that the user has pressed some keyboard key. Message parameter ui32Param1 is the ASCII code of the key being pressed. */
		WMT_MOUSE_MOVE, /**< Message indicates that cursor is being moved within the window. Message parameter ui32Param1 stores x coordinate of cursor and ui32Param1 stores y coordinate. */
		WMT_MOUSE_DOWN, /**< Message indicates that the user has pressed mouse button. Message parameter ui32Param1 indicates what button was pressed: 0 - Left, 1 - Right, 2 - Middle */
		WMT_MOUSE_UP,	/**< Message indicates that the user has released mouse button. Message parameter ui32Param1 indicates what button was released: 0 - Left, 1 - Right, 2 - Middle */
		WMT_MOUSE_WHEEL /**< Message indicates that the user has rolled mouse wheel. Message parameter pParam3 points to integer (int) with mouse wheel delta value. */
	};

	/** Structure with window event message information. 
		\see IEvWinMessage
	 */
	struct TWinMessage
	{
		E_WINDOW_MESSAGE_TYPE
				uiMsgType;	/**< Window message type identifier. */
		uint32	ui32Param1;	/**< Message first parametr. */
		uint32	ui32Param2;	/**< Message second parametr. */
		void	*pParam3;	/**< Message third parametr. Points to specific message data. */
		
		TWinMessage():uiMsgType(WMT_UNKNOWN), ui32Param1(0), ui32Param2(0), pParam3(NULL){}
		TWinMessage(E_WINDOW_MESSAGE_TYPE msg, uint32 param1 = 0, uint32 param2 = 0, void *param3 = NULL):uiMsgType(msg), ui32Param1(param1), ui32Param2(param2), pParam3(param3){}
	};


//Engine types definition//

	/** Flags of TEngWindow structure that determines the behavior of main engine window. */
	enum E_ENGINE_WINDOW_FLAGS
	{
		EWF_DEFAULT					= 0x00000000,/**< This flag is suitable in most cases. */
		EWF_ALLOW_SIZEING			= 0x00000001,/**< User can resize engine window arbitrarily */
		EWF_TOPMOST					= 0x00000002,/**< Engine window will be always on top. */
		EWF_DONT_HOOK_MLOOP			= 0x00000004,/**< If flag set and engine doesn't owns window, host applications main loop will not be hooked. User must call window repaint manually. */
		EWF_DONT_HOOK_ROOT_WIN		= 0x00000008,/**< If flag set and engine doesn't owns window, main host application window will not be hooked. User must redirect windows messages manually.*/
		EWF_RESTRICT_FSCREEN_HOTKEY	= 0x00000010,/**< Switching between fullscreen and windowed modes by pressing platform specific hotkey combination will be restricted. */
		EWF_RESTRICT_CONSOLE_HOTKEY	= 0x00000020 /**< Restricts calling engine console window by pressing "~" or "§" key. */
	};

	/** Flags of TEngWindow structure that determines antialiasing samples count. */
	enum E_MULTISAMPLING_MODE
	{
		MM_NONE	= 0x00000000,/**< Multisampling is off. */
		MM_2X	= 0x00000001,/**< 2xMSAA */
		MM_4X	= 0x00000002,/**< 4xMSAA \note This value is recomended. */
		MM_8X	= 0x00000004,/**< 8xMSAA */
		MM_16X	= 0x00000008 /**< 16xMSAA */
	};

	/** Describes the parameters of the main engine window and its behavior. 
		\see IEngineCore::InitializeEngine
	 */
	struct TEngWindow
	{	
		uint	uiWidth;			/**< Resolution width of client area. */
		uint	uiHeight;			/**< Resolution height of client area. */
		bool	bFullScreen;		/**< Switch to fullscreen mode or not. */
		bool	bVSync;				/**< Enable or not vertical synchronization(triple buffering). */
		E_MULTISAMPLING_MODE
				eMSampling;			/**< Fullscreen antialiasing samples count. */
		E_ENGINE_WINDOW_FLAGS
				uiFlags;			/**< Additional settings. */

		TEngWindow()
		{
			uiWidth			= 800;
			uiHeight		= 600;
			bFullScreen		= false;
			bVSync			= false;
			eMSampling		= MM_NONE;
			uiFlags			= EWF_DEFAULT;
		}

		TEngWindow(uint width, uint height, bool fscreen, bool vsync = false, E_MULTISAMPLING_MODE msampling = MM_NONE, E_ENGINE_WINDOW_FLAGS flags = EWF_DEFAULT)
		{
			uiWidth			= width;
			uiHeight		= height;
			bFullScreen		= fscreen;
			bVSync			= vsync;
			eMSampling		= msampling;
			uiFlags			= flags;
		}
	};

	/** Describes user hardware and operating system. 
		\see IEngineCore::GetSystemInfo
	 */
	struct TSystemInfo
	{	
		char cOSName[128];			/**< String with operating system discription. */
		char cCPUName[128];			/**< String with CPU discription. */
		uint uiCPUCount;			/**< Number of CPUs on host system. */
		uint uiCPUFreq;				/**< Real CPU frequency in MHz. */
		uint uiRAMTotal;			/**< Ammount of RAM in system in megabytes. */
		uint uiRAMAvailable;		/**< Ammount of free RAM in system on engine start in megabytes. */
		char cVideocardName[128];	/**< String with primary videocard discription. */
		uint uiVideocardCount;		/**< Number of videocards in system. */
		uint uiVideocardRAM;		/**< Ammount of video RAM in system in megabytes. */
	};

	/** Describes engine plugin information. 
		\see IPlugin
	 */
	struct TPluginInfo
	{
		uint8 btPluginSDKVersion;	/**< Version of SDK with wich plugin was build. \warning This value MUST BE set to "_DGLE_PLUGIN_SDK_VER_"! */
		char  cName[128];		 	/**< String with plugin name. */
		char  cVersion[64];		 	/**< String with plugin version. */
		char  cVendor[128];		 	/**< String with name of vendor(developer). */
		char  cDiscription[256]; 	/**< String with plugin discription. */
		
		TPluginInfo()
		{
			btPluginSDKVersion = _DGLE_PLUGIN_SDK_VER_;
		}

	};

	#define RGBA(r, g, b, a) ((uint32)(((uint8)(r) | ((uint32)((uint8)(g)) << 8)) | (((uint32)(uint8)(b)) << 16) | (((uint32)(uint8)(a)) << 24)))

	/** Describes color in engine. Color is four component based. Each component is float, value can vary from 0.0 to 1.0.*/
	struct TColor4
	{
		union
		{
			struct
			{
				float	r, g, b, a;
			};
			float rgba[4];
		};

		typedef float (&ref)[4];
		typedef const float (&cref)[4];

		inline TColor4():r(1.f), g(1.f), b(1.f), a(1.f){}

		inline TColor4(uint32 ui32RGBA):
			r((float)(ui32RGBA & 0xff) / 0xff),
			g((float)((ui32RGBA >> 8) & 0xff) / 0xff),
			b((float)((ui32RGBA >> 16) & 0xff) / 0xff),
			a((float)((ui32RGBA >> 24) & 0xff) / 0xff)
		{}

		inline TColor4(uint8 ubR, uint8 ubG, uint8 ubB, uint8 ubA)
		{
			SetColorB(ubR, ubG, ubB, ubA);
		}
		
		inline TColor4(cref rgba)
		{
			memcpy(TColor4::rgba, rgba, sizeof rgba);
		}

		inline void SetColorF(float fR, float fG, float fB, float fA)
		{
			r = fR; g = fG; b = fB; a = fA;
		}
		
		inline void SetColorB(uint8 ubR, uint8 ubG, uint8 ubB, uint8 ubA)
		{
			r = ubR/255.f; g = ubG/255.f; b = ubB/255.f; a = ubA/255.f;
		}
		
		inline uint32 ColorRGB()
		{
			return RGB(255*r, 255*g, 255*b);
		}

		inline uint32 ColorRGBA()
		{
			return RGBA(255*r, 255*g, 255*b, 255*a);
		}

		inline operator uint32()
		{
			return RGBA(255*r, 255*g, 255*b, 255*a);
		}

		inline operator ref()
		{
			return rgba;
		}

		inline operator cref() const
		{
			return rgba;
		}
	};

	typedef TColor4 TColor;

	/** Describes point coordinates in 2D space. */
	struct TPoint2
	{
		union
		{
			struct
			{
				float x, y;
			};
			float xy[2];
		};

		typedef float (&ref)[2];
		typedef const float (&cref)[2];

		inline TPoint2():x(0.f), y(0.f){}
		inline TPoint2(const float *pfArray):x(pfArray[0]), y(pfArray[1]){}
		inline TPoint2(float fX, float fY):x(fX), y(fY){}
		inline TPoint2(cref xy)
		{
			memcpy(TPoint2::xy, xy, sizeof xy);
		}

		inline TPoint2 &operator += (const TPoint2 &point)
		{
			x += point.x;
			y += point.y;
			return *this;
		}

		inline TPoint2 operator + (const TPoint2 &point) const
		{
			return TPoint2(*this) += point;
		}

		inline TPoint2 &operator -= (const TPoint2 &point)
		{
			x -= point.x;
			y -= point.y;
			return *this;
		}

		inline TPoint2 operator - (const TPoint2 &point) const
		{
			return TPoint2(*this) -= point;
		}
		
		inline TPoint2 &operator *= (const TPoint2 &point)
		{
			x *= point.x;
			y *= point.y;
			return *this;
		}

		inline TPoint2 operator * (const TPoint2 &point) const
		{
			return TPoint2(*this) *= point;
		}

		inline TPoint2 &operator *= (float val)
		{
			x *= val;
			y *= val;
			return *this;
		}

		inline TPoint2 operator * (float val) const
		{
			return TPoint2(*this) *= val;
		}

		inline TPoint2 &operator /= (const TPoint2 &point)
		{
			x /= point.x;
			y /= point.y;
			return *this;
		}

		inline TPoint2 operator / (const TPoint2 &point) const
		{
			return TPoint2(*this) /= point;
		}

		inline TPoint2 &operator /= (float val)
		{
			x /= val;
			y /= val;
			return *this;
		}

		inline TPoint2 operator / (float val) const
		{
			return TPoint2(*this) /= val;
		}

		inline float Dot(const TPoint2 &point) const
		{
			return x * point.x + y * point.y;
		}

		inline float Cross(const TPoint2 &point) const
		{
			return x * point.y - point.x * y;
		}

		inline float DistTo(const TPoint2 &point) const
		{
			return sqrtf( (point.x - x)*(point.x - x) + (point.y - y)*(point.y - y) );
		}

		inline float DistToQ(const TPoint2 &point) const
		{
			return (point - *this).LengthQ();
		}

		inline float LengthQ() const
		{
			return x*x + y*y;
		}

		inline float Length() const
		{
			return sqrtf(LengthQ());
		}

		inline TPoint2 &Normalize()
		{
			const float len = Length();
			x /= len, y /= len;
			return *this;
		}
		
		inline TPoint2 Lerp(const TPoint2 &point, float coeff) const
		{
			return *this + (point - *this) * coeff;
		}

		inline float Angle(const TPoint2 &point) const
		{
			return atan2f(x * point.y - y * point.x, x * point.x + y * point.y);
		}

		inline TPoint2 Rotate(float fAngle) const
		{
			const float s = sinf(fAngle), c = cosf(fAngle);
			return TPoint2(x * c - y * s, x * s + y * c);
		}

		inline TPoint2 Reflect(const TPoint2 &normal) const
		{
			return *this - normal * (2.f * Dot(normal));
		}

		inline operator ref() { return xy; }
		inline operator cref() const { return xy; }
	};

	typedef TPoint2 TVec2, TVector2;

	/** Describes point coordinates in 3D space. */
	struct TPoint3
	{
		union
		{
			struct
			{
				float x, y, z;
			};
			float xyz[3];
		};

		typedef float (&ref)[3];
		typedef const float (&cref)[3];

		inline TPoint3():x(0.f), y(0.f), z(0.f){}
		inline TPoint3(const float *pfArray):x(pfArray[0]), y(pfArray[1]), z(pfArray[2]){}
		inline TPoint3(float fX, float fY, float fZ):x(fX), y(fY), z(fZ){}
		inline TPoint3(const TPoint2 &p):x(p.x), y(p.y), z(0.f){}
		inline TPoint3(cref xyz)
		{
			memcpy(TPoint3::xyz, xyz, sizeof xyz);
		}

		inline TPoint3 &operator += (const TPoint3 &point)
		{
			x += point.x;
			y += point.y;
			z += point.y;
			return *this;
		}

		inline TPoint3 operator + (const TPoint3 &point) const
		{
			return TPoint3(*this) += point;
		}

		inline TPoint3 &operator -= (const TPoint3 &point)
		{
			x -= point.x;
			y -= point.y;
			z -= point.z;
			return *this;
		}

		inline TPoint3 operator - (const TPoint3 &point) const
		{
			return TPoint3(*this) -= point;
		}
		
		inline TPoint3 &operator *= (const TPoint3 &point)
		{
			x *= point.x;
			y *= point.y;
			z *= point.z;
			return *this;
		}

		inline TPoint3 operator * (const TPoint3 &point) const
		{
			return TPoint3(*this) *= point;
		}

		inline TPoint3 &operator *= (float val)
		{
			x *= val;
			y *= val;
			z *= val;
			return *this;
		}

		inline TPoint3 operator * (float val) const
		{
			return TPoint3(*this) *= val;
		}

		inline TPoint3 &operator /= (const TPoint3 &point)
		{
			x /= point.x;
			y /= point.y;
			z /= point.z;
			return *this;
		}

		inline TPoint3 operator / (const TPoint3 &point) const 
		{ 
			return TPoint3(*this) /= point; 
		}

		inline TPoint3 &operator /= (float val)
		{
			x /= val;
			y /= val;
			z /= val;
			return *this;
		}

		inline TPoint3 operator / (float val) const 
		{ 
			return TPoint3(*this) /= val; 
		}

		inline float Dot(const TPoint3 &point) const
		{
			return x * point.x + y * point.y + z * point.z;
		}

		inline TPoint3 Cross(const TPoint3 &point) const
		{
			return TPoint3(y * point.z - z * point.y, z * point.x - x * point.z, x * point.y - y * point.x);
		}

		inline float FlatDistTo(const TPoint3 &point) const
		{
			return sqrtf( (point.x - x)*(point.x - x) + (point.y - y)*(point.y - y) );
		}

		inline float DistTo(const TPoint3 &point) const
		{
			return sqrtf( (point.x - x)*(point.x - x) + (point.y - y)*(point.y - y) + (point.z - z)*(point.z - z) );
		}

		inline float DistToQ(const TPoint3 &point) const
		{
			return (point - *this).LengthQ();
		}

		inline float LengthQ() const
		{
			return x*x + y*y + z*z;
		}

		inline float Length() const
		{
			return sqrtf(LengthQ());
		}

		inline TPoint3 &Normalize()
		{
			const float len = Length();
			x /= len, y /= len; z /= len;
			return *this;
		}

		inline TPoint3 Lerp(const TPoint3 &point, float coeff) const
		{
			return *this + (point - *this) * coeff;
		}

		inline float Angle(const TPoint3 &point) const
		{
			return acosf(Dot(point) / sqrtf( LengthQ() * point.LengthQ() ));
		}

		inline TPoint3 Rotate(float fAngle, const TPoint3 &axis) const
		{
			const float s = sinf(fAngle), c = cosf(fAngle);
			
			TPoint3 v[3];
			
			v[0] = axis * Dot(axis);
			v[1] = *this - v[0];
			v[2] = axis.Cross(v[1]);

			return TPoint3(v[0].x + v[1].x * c + v[2].x * s, v[0].y + v[1].y * c + v[2].y * s, v[0].z + v[1].z * c + v[2].z * s);
		}

		inline TPoint3 Reflect(const TPoint3 &normal) const
		{
			return *this - normal * (2.f * Dot(normal));
		}

		inline operator ref() { return xyz; }
		inline operator cref() const { return xyz; }

	};

	typedef TPoint3 TVec3, TVector3;

	/** Describes graphical point coordinates, color and its texture coordinates in 2D space. */
	struct TVertex2
	{
		union
		{
			struct
			{
				float x, y, u, w, r, g, b, a;
			};
			float data[8];
		};
		
		inline TVertex2():x(0.f), y(0.f), u(0.f), w(0.f), r(1.f), g(1.f), b(1.f), a(1.f){}
		inline TVertex2(float fX, float fY, float fU, float fW, float fR, float fG, float fB, float fA):
		x(fX), y(fY), u(fU), w(fW), r(fR), g(fG), b(fB), a(fA){}
	};

	/** Describes graphical point coordinates, color and its texture coordinates in 3D space. */
	struct TVertex3
	{
		union
		{
			struct
			{
				float x, y, z, u, w, r, g, b, a;
			};
			float data[9];
		};

		inline TVertex3():x(0.f), y(0.f), z(0.f), u(0.f), w(0.f), r(1.f), g(1.f), b(1.f), a(1.f){}
		inline TVertex3(float fX, float fY, float fZ, float fU, float fW, float fR, float fG, float fB, float fA):
		x(fX), y(fY), z(fZ), u(fU), w(fW), r(fR), g(fG), b(fB), a(fA){}
	};

	/** Describes 2D rectangle by float values. */
	struct TRectF
	{
		float x, y, width, height;

		inline TRectF(): x(0.f), y(0.f), width(0.f), height(0.f){}
		inline TRectF(float fX, float fY, float fWidth, float fHeight): x(fX), y(fY), width(fWidth), height(fHeight){}
		inline TRectF(const TPoint2 &stLeftTop, const TPoint2 &stRightBottom): x(stLeftTop.x), y(stLeftTop.y), width(stRightBottom.x - stLeftTop.x), height(stRightBottom.y - stLeftTop.y){}
		
		inline bool IntersectRect(const TRectF &stRect) const
		{
			return	(x < stRect.x + stRect.width && x + width > stRect.x && y < stRect.y + stRect.height && y + height > stRect.y)||
					(stRect.x + stRect.width < x && stRect.x > x + width && stRect.y + stRect.height < y && stRect.y > y + height);
		}
		
		inline bool PointInRect(const TPoint2 &stPoint) const
		{
			return stPoint.x > x && stPoint.x < x + width && stPoint.y > y && stPoint.y < y + height;
		}
		
		inline bool RectInRect(const TRectF &stRect) const
		{
			return	stRect.x < x && stRect.y < y && stRect.x + stRect.width > x + width && stRect.y + stRect.height > y + height;
		}

		inline TRectF GetIntersectionRect(const TRectF &stRect) const
		{
			if ( IntersectRect(stRect) )
			{
				TRectF result = stRect;
				
				if (x > stRect.x) result.x = x;
				if (y > stRect.y) result.y = y;
				
				float rectr = x + width, strectr = stRect.x + stRect.width;
				result.width = (rectr > strectr ? strectr : rectr) - result.x;
				
				float rectb = y + height, strectb = stRect.y + stRect.height;
				result.height = (rectb > strectb ? strectb : rectb) - result.y;

				return result;
			}
			else
				return TRectF();
		}
	};

	/** Describes 4x4 matrix and some math routines used in 3D graphics computes. 
	\warning In engine matrices are stored per lines. Like in Direct3D and unlike it in OpenGL. So keep it in mind when using them directly via GAPI.
	*/
	struct TMatrix
	{
		union
		{
			float _1D[16];
			float _2D[4][4];
		};

		inline TMatrix()
		{
			memset(_1D, 0, sizeof(_1D));
		}

		inline TMatrix(
			float _00, float _01, float _02, float _03,
			float _10, float _11, float _12, float _13,
			float _20, float _21, float _22, float _23,
			float _30, float _31, float _32, float _33)
		{
			_2D[0][0] = _00; _2D[0][1] = _01; _2D[0][2] = _02; _2D[0][3] = _03;
			_2D[1][0] = _10; _2D[1][1] = _11; _2D[1][2] = _12; _2D[1][3] = _13;
			_2D[2][0] = _20; _2D[2][1] = _21; _2D[2][2] = _22; _2D[2][3] = _23;
			_2D[3][0] = _30; _2D[3][1] = _31; _2D[3][2] = _32; _2D[3][3] = _33;
		}

		inline TMatrix &operator +=(float right)
		{
			for (int i = 0; i < 16; i++)
				_1D[i] += right;
			return *this;
		}

		inline TMatrix &operator -=(float right)
		{
			for (int i = 0; i < 16; i++)
				_1D[i] -= right;
			return *this;
		}

		inline TMatrix &operator *=(float right)
		{
			for (int i = 0; i < 16; i++)
				_1D[i] *= right;
			return *this;
		}

		inline TMatrix &operator /=(float right)
		{
			for (int i = 0; i < 16; i++)
				_1D[i] /= right;
			return *this;
		}

		TMatrix &operator +=(const TMatrix &right)
		{
			for (int i = 0; i < 16; i++)
				_1D[i] += right._1D[i];
			return *this;
		}

		TMatrix &operator -=(const TMatrix &right)
		{
			for (int i = 0; i < 16; i++)
				_1D[i] -= right._1D[i];
			return *this;
		}

		TMatrix &operator *=(const TMatrix &right)
		{
			return *this = *this * right;
		}

		TMatrix operator +(float right) const
		{
			return TMatrix(*this) += right;
		}

		TMatrix operator -(float right) const
		{
			return TMatrix(*this) -= right;
		}

		TMatrix operator *(float right) const
		{
			return TMatrix(*this) *= right;
		}

		TMatrix operator /(float right) const
		{
			return TMatrix(*this) /= right;
		}

		TMatrix operator +(const TMatrix &right) const
		{
			return TMatrix(*this) += right;
		}

		TMatrix operator -(const TMatrix &right) const
		{
			return TMatrix(*this) -= right;
		}

		TMatrix operator *(const TMatrix &right) const
		{
			TMatrix product;

			product._2D[0][0] = _2D[0][0] * right._2D[0][0] + _2D[0][1] * right._2D[1][0] + _2D[0][2] * right._2D[2][0] + _2D[0][3] * right._2D[3][0];
			product._2D[1][0] = _2D[1][0] * right._2D[0][0] + _2D[1][1] * right._2D[1][0] + _2D[1][2] * right._2D[2][0] + _2D[1][3] * right._2D[3][0];
			product._2D[2][0] = _2D[2][0] * right._2D[0][0] + _2D[2][1] * right._2D[1][0] + _2D[2][2] * right._2D[2][0] + _2D[2][3] * right._2D[3][0];
			product._2D[3][0] = _2D[3][0] * right._2D[0][0] + _2D[3][1] * right._2D[1][0] + _2D[3][2] * right._2D[2][0] + _2D[3][3] * right._2D[3][0];
			product._2D[0][1] = _2D[0][0] * right._2D[0][1] + _2D[0][1] * right._2D[1][1] + _2D[0][2] * right._2D[2][1] + _2D[0][3] * right._2D[3][1];
			product._2D[1][1] = _2D[1][0] * right._2D[0][1] + _2D[1][1] * right._2D[1][1] + _2D[1][2] * right._2D[2][1] + _2D[1][3] * right._2D[3][1];
			product._2D[2][1] = _2D[2][0] * right._2D[0][1] + _2D[2][1] * right._2D[1][1] + _2D[2][2] * right._2D[2][1] + _2D[2][3] * right._2D[3][1];
			product._2D[3][1] = _2D[3][0] * right._2D[0][1] + _2D[3][1] * right._2D[1][1] + _2D[3][2] * right._2D[2][1] + _2D[3][3] * right._2D[3][1];
			product._2D[0][2] = _2D[0][0] * right._2D[0][2] + _2D[0][1] * right._2D[1][2] + _2D[0][2] * right._2D[2][2] + _2D[0][3] * right._2D[3][2];
			product._2D[1][2] = _2D[1][0] * right._2D[0][2] + _2D[1][1] * right._2D[1][2] + _2D[1][2] * right._2D[2][2] + _2D[1][3] * right._2D[3][2];
			product._2D[2][2] = _2D[2][0] * right._2D[0][2] + _2D[2][1] * right._2D[1][2] + _2D[2][2] * right._2D[2][2] + _2D[2][3] * right._2D[3][2];
			product._2D[3][2] = _2D[3][0] * right._2D[0][2] + _2D[3][1] * right._2D[1][2] + _2D[3][2] * right._2D[2][2] + _2D[3][3] * right._2D[3][2];
			product._2D[0][3] = _2D[0][0] * right._2D[0][3] + _2D[0][1] * right._2D[1][3] + _2D[0][2] * right._2D[2][3] + _2D[0][3] * right._2D[3][3];
			product._2D[1][3] = _2D[1][0] * right._2D[0][3] + _2D[1][1] * right._2D[1][3] + _2D[1][2] * right._2D[2][3] + _2D[1][3] * right._2D[3][3];
			product._2D[2][3] = _2D[2][0] * right._2D[0][3] + _2D[2][1] * right._2D[1][3] + _2D[2][2] * right._2D[2][3] + _2D[2][3] * right._2D[3][3];
			product._2D[3][3] = _2D[3][0] * right._2D[0][3] + _2D[3][1] * right._2D[1][3] + _2D[3][2] * right._2D[2][3] + _2D[3][3] * right._2D[3][3];

			return product;
		}
	};
	
	typedef TMatrix TMat4, TMatrix4, TMatrix4x4;

	// Matrix helper functions //

	/** Returns identity matrx. */
	inline TMatrix MatrixIdentity()
	{
		return TMatrix(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);
	}

	/** Returns inverce matrx. */
	inline TMatrix MatrixInverse(const TMatrix &stMatrix)
	{
		float mat[4][8] =
		{
			stMatrix._2D[0][0], stMatrix._2D[0][1], stMatrix._2D[0][2], stMatrix._2D[0][3], 1, 0, 0, 0,
			stMatrix._2D[1][0], stMatrix._2D[1][1], stMatrix._2D[1][2], stMatrix._2D[1][3], 0, 1, 0, 0,
			stMatrix._2D[2][0], stMatrix._2D[2][1], stMatrix._2D[2][2], stMatrix._2D[2][3], 0, 0, 1, 0,
			stMatrix._2D[3][0], stMatrix._2D[3][1], stMatrix._2D[3][2], stMatrix._2D[3][3], 0, 0, 0, 1
		};
		float *rows[4] = { mat[0], mat[1], mat[2], mat[3] };
		int i, r;
		
		for (i = 0; i < 4; ++i)
		{
			uint8 row_num = i;
			float major = fabs(rows[i][i]);
			
			for (r = i + 1; r < 4; ++r)
			{
				float cur_ABS = fabs(rows[r][i]);
				if (cur_ABS > major)
				{
					major=cur_ABS;
					row_num=r;
				}
			}

			if (row_num != i)
			{
				(int &)rows[i]			^= (int)rows[row_num];
				(int &)rows[row_num]	^= (int)rows[i];
				(int &)rows[i]			^= (int)rows[row_num];
			}
			
			for (r = i + 1; r < 4; ++r)
			{
				float factor=rows[r][i] / rows[i][i];
				for (int c = i; c < 8; ++c)
					rows[r][c] -= factor * rows[i][c];
			}
		}
		for (i = 3; i > 0; --i)
			for (r = 0; r < i; ++r) 
			{
				float factor=rows[r][i] / rows[i][i];
				for (int c = 4; c < 8; ++c)
					rows[r][c] -= factor * rows[i][c];
			}

		return TMatrix(
			rows[0][4] / rows[0][0], rows[0][5] / rows[0][0], rows[0][6] / rows[0][0], rows[0][7] / rows[0][0],
			rows[1][4] / rows[1][1], rows[1][5] / rows[1][1], rows[1][6] / rows[1][1], rows[1][7] / rows[1][1],
			rows[2][4] / rows[2][2], rows[2][5] / rows[2][2], rows[2][6] / rows[2][2], rows[2][7] / rows[2][2],
			rows[3][4] / rows[3][3], rows[3][5] / rows[3][3], rows[3][6] / rows[3][3], rows[3][7] / rows[3][3]);
	}

	/** Returns transpose matrx. */
	inline TMatrix MatrixTranspose(const TMatrix &stMatrix)
	{
		return TMatrix(
			stMatrix._2D[0][0], stMatrix._2D[1][0], stMatrix._2D[2][0], stMatrix._2D[3][0],
			stMatrix._2D[0][1], stMatrix._2D[1][1], stMatrix._2D[2][1], stMatrix._2D[3][1],
			stMatrix._2D[0][2], stMatrix._2D[1][2], stMatrix._2D[2][2], stMatrix._2D[3][2],
			stMatrix._2D[0][3], stMatrix._2D[1][3], stMatrix._2D[2][3], stMatrix._2D[3][3]);
	}
	
	/** Returns scaled matrix by given vector. */
	inline TMatrix MatrixScale(const TVector3 &stVec)
	{
		return TMatrix(
			stVec.x, 0.f, 0.f, 0.f,
			0.f, stVec.y, 0.f, 0.f,
			0.f, 0.f, stVec.z, 0.f,
			0.f, 0.f, 0.f, 1.f);
	}

	/** Returns translated matrix by given vector. */
	inline TMatrix MatrixTranslate(const TVector3 &stVec)
	{
		return TMatrix(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			stVec.x, stVec.y, stVec.z, 1.f);
	}

	/** Returns rotated matrix by given axes vector and angle. */
	inline TMatrix MatrixRotate(float fAngle, TVector3 &stAxes)
	{
		const float
			axis_norm = sqrt(stAxes.x*stAxes.x + stAxes.y*stAxes.y + stAxes.z*stAxes.z),
			x = stAxes.x / axis_norm,
			y = stAxes.y / axis_norm,
			z = stAxes.z / axis_norm,
			sin_angle = sinf(fAngle*(float)M_PI/180.f),
			cos_angle = cosf(fAngle*(float)M_PI/180.f);
		return TMatrix(
			(1.f - x * x) * cos_angle + x * x,			z * sin_angle + x * y * (1.f - cos_angle),	x * z * (1.f - cos_angle) - y * sin_angle,	0.f,
			x * y * (1.f - cos_angle) - z * sin_angle,	(1.f - y * y) * cos_angle + y * y,			y * z * (1.f - cos_angle) + x * sin_angle,	0.f,
			x * z * (1.f - cos_angle) + y * sin_angle,	y * z * (1.f - cos_angle) - x * sin_angle,	(1.f - z * z) * cos_angle + z * z,			0.f,
			0.f,										0.f,										0.f,										1.f);
	}

	/** Returns billboarded matrix. Removes rotation information from the matrix. */
	inline TMatrix MatrixBillboard(const TMatrix &stMatrix)
	{
		return TMatrix(
			1.f,				0.f,				0.f,				stMatrix._2D[0][3],
			0.f,				1.f,				0.f,				stMatrix._2D[1][3],
			0.f,				0.f,				1.f,				stMatrix._2D[2][3],
			stMatrix._2D[3][0],	stMatrix._2D[3][1],	stMatrix._2D[3][2],	stMatrix._2D[3][3]);
	}

	/** Template for multiplication stacks. */
	template<class TTransform>
	class TTransformStack: private std::stack<TTransform>
	{
	public:
		TTransformStack(const TTransform &base_transform = TTransform());
		/** Saves current stack state. */
		void Push();
		/** Restores previously saved stack state. */
		void Pop();
		/** Returns current stack state. */
		const TTransform &Top() const;
		/** Returns current stack state. */
		TTransform &Top();
		/** Multiplicates current stack state on transform. */
		void MultGlobal(const TTransform &transform);
		/** Multiplicates transform on current stack state and saves result as current stack state. */
		void MultLocal(const TTransform &transform);
	};

	template<class TTransform>
	inline TTransformStack<TTransform>::TTransformStack(const TTransform &base_transform)
	{
		push(base_transform);
	}

	template<class TTransform>
	inline void TTransformStack<TTransform>::Push()
	{
		push(top());
	}

	template<class TTransform>
	inline void TTransformStack<TTransform>::Pop()
	{
		assert(size() > 1);
		pop();
	}

	template<class TTransform>
	inline const TTransform &TTransformStack<TTransform>::Top() const
	{
		return top();
	}

	template<class TTransform>
	inline TTransform &TTransformStack<TTransform>::Top()
	{
		return top();
	}

	template<class TTransform>
	inline void TTransformStack<TTransform>::MultGlobal(const TTransform &transform)
	{
		top() *= transform;
	}

	template<class TTransform>
	inline void TTransformStack<TTransform>::MultLocal(const TTransform &transform)
	{
		top() = transform * top();
	}

	/** Stack of matrix 4x4 multiplication operations. */
	typedef TTransformStack<TMatrix> TMatrixStack;

	/** Describes the state of the mouse. 
		\see IInput
	*/
	struct TMouseStates
	{
		int	 iX;			/**< X coordinate of mouse pointer. */ 
		int	 iY;			/**< Y coordinate of mouse pointer. */
		int	 iDeltaX;		/**< The difference between the current and previous X coordinate value. */
		int	 iDeltaY;		/**< The difference between the current and previous Y coordinate value. */
		int	 iDeltaWheel;	/**< Mouse wheel offset. */
		bool bLeftButton;	/**< Is mouse left button pressed. */
		bool bRightButton;	/**< Is mouse right button pressed. */
		bool bMiddleButton; /**< Is mouse middle button pressed. */
	};

	/** Describes the state of the keyboard. 
		\see IInput
	*/
	struct TKeyboardStates 
	{
		bool bCapsLock; 	/**< Is CapsLock pressed. */
		bool bShiftL;		/**< Is left Shift pressed. */
		bool bShiftR;		/**< Is right Shift pressed. */
		bool bCtrlL;		/**< Is left Control pressed. */
		bool bCtrlR;		/**< Is right Control pressed. */
		bool bAltL;			/**< Is left Alt pressed. */
		bool bAltR;			/**< Is right Alt pressed. */
	};

	/** Describes the state of the joystick or gamepad. 
		\see IInput
	*/
	struct TJoystickStates
	{
		uint uiBtnsCount;	/**< Count of available joystick buttons. */
		bool bButtons[32];	/**< Array of joystick buttons states (pressed or not). */
		int iXAxes;		    /**< X-axis position. Value varies -100 to 100. */
		int iYAxes;		    /**< Y-axis position. Value varies -100 to 100. */
		int iZAxes;		    /**< Z-axis position. Value varies -100 to 100. */
		int iRAxes;			/**< Current position of the rudder or fourth joystick axis. Value varies -100 to 100. */
		int iUAxes;			/**< Current fifth axis position. Value varies -100 to 100. */
		int iVAxes;			/**< Current sixth axis position. Value varies -100 to 100. */
		int iPOV;			/**< Point-Of-View direction. */
	};

#if defined(STRUCT_ALIGNMENT_1) && defined(PLATFORM_WINDOWS)
#pragma pack(pop)
#endif

	/** Describes all keyboard key codes.
		\warning This is not common ASCII key codes!
		\see IInput
	*/
	enum E_KEYBOARD_KEY_CODES
	{
		KEY_ESCAPE			= 0x01,
		KEY_TAB				= 0x0F,
		KEY_GRAVE			= 0x29,
		KEY_CAPSLOCK		= 0x3A,
		KEY_BACKSPACE		= 0x0E,
		KEY_RETURN			= 0x1C,
		KEY_SPACE			= 0x39,
		KEY_SLASH			= 0x35,
		KEY_BACKSLASH		= 0x2B,

		KEY_SYSRQ			= 0xB7,
		KEY_SCROLL			= 0x46,
		KEY_PAUSE			= 0xC5,

		KEY_INSERT			= 0xD2,
		KEY_DELETE			= 0xD3,
		KEY_HOME			= 0xC7,
		KEY_END				= 0xCF,
		KEY_PGUP			= 0xC9,
		KEY_PGDN			= 0xD1,

		KEY_LSHIFT			= 0x2A,
		KEY_RSHIFT			= 0x36,
		KEY_LALT			= 0x38,
		KEY_RALT			= 0xB8,
		KEY_LWIN_OR_CMD		= 0xDB,
		KEY_RWIN_OR_CMD		= 0xDC,
		KEY_LCONTROL		= 0x1D,
		KEY_RCONTROL		= 0x9D,

		KEY_UP				= 0xC8,
		KEY_RIGHT			= 0xCD,
		KEY_LEFT			= 0xCB,
		KEY_DOWN			= 0xD0,

		KEY_1				= 0x02,
		KEY_2				= 0x03,
		KEY_3				= 0x04,
		KEY_4				= 0x05,
		KEY_5				= 0x06,
		KEY_6				= 0x07,
		KEY_7				= 0x08,
		KEY_8				= 0x09,
		KEY_9				= 0x0A,
		KEY_0				= 0x0B,

		KEY_F1				= 0x3B,
		KEY_F2				= 0x3C,
		KEY_F3				= 0x3D,
		KEY_F4				= 0x3E,
		KEY_F5				= 0x3F,
		KEY_F6				= 0x40,
		KEY_F7				= 0x41,
		KEY_F8				= 0x42,
		KEY_F9				= 0x43,
		KEY_F10				= 0x44,
		KEY_F11				= 0x57,
		KEY_F12				= 0x58,

		KEY_Q				= 0x10,
		KEY_W				= 0x11,
		KEY_E				= 0x12,
		KEY_R				= 0x13,
		KEY_T				= 0x14,
		KEY_Y				= 0x15,
		KEY_U				= 0x16,
		KEY_I				= 0x17,
		KEY_O				= 0x18,
		KEY_P				= 0x19,
		KEY_A				= 0x1E,
		KEY_S				= 0x1F,
		KEY_D				= 0x20,
		KEY_F				= 0x21,
		KEY_G				= 0x22,
		KEY_H				= 0x23,
		KEY_J				= 0x24,
		KEY_K				= 0x25,
		KEY_L				= 0x26,
		KEY_Z				= 0x2C,
		KEY_X				= 0x2D,
		KEY_C				= 0x2E,
		KEY_V				= 0x2F,
		KEY_B				= 0x30,
		KEY_N				= 0x31,
		KEY_M				= 0x32,

		KEY_MINUS			= 0x0C,
		KEY_PLUS			= 0x0D,
		KEY_LBRACKET		= 0x1A,
		KEY_RBRACKET		= 0x1B,

		KEY_SEMICOLON		= 0x27,
		KEY_APOSTROPHE		= 0x28,

		KEY_COMMA			= 0x33,
		KEY_PERIOD			= 0x34,

		KEY_NUMPAD0			= 0x52,
		KEY_NUMPAD1			= 0x4F,
		KEY_NUMPAD2			= 0x50,
		KEY_NUMPAD3			= 0x51,
		KEY_NUMPAD4			= 0x4B,
		KEY_NUMPAD5			= 0x4C,
		KEY_NUMPAD6			= 0x4D,
		KEY_NUMPAD7			= 0x47,
		KEY_NUMPAD8			= 0x48,
		KEY_NUMPAD9			= 0x49,
		KEY_NUMPADPERIOD	= 0x53,
		KEY_NUMPADENTER		= 0x9C,
		KEY_NUMPADSTAR		= 0x37,
		KEY_NUMPADPLUS		= 0x4E,
		KEY_NUMPADMINUS		= 0x4A,
		KEY_NUMPADSLASH		= 0xB5,
		KEY_NUMLOCK			= 0x45,
	};

}

#endif//DGLE_TYPES
