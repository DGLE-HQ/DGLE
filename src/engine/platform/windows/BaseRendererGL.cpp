/**
\author		Andrey Korotkov aka DRON
\date		26.04.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "BaseRendererGL.h"

#ifndef NO_BUILTIN_RENDERER

#pragma message("Linking with \"opengl32.lib\".")
#pragma comment(linker, "/defaultlib:opengl32.lib")

CBaseRendererGL::CBaseRendererGL(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{}

bool CBaseRendererGL::Prepare()
{
	return true;
}

bool CBaseRendererGL::Initialize()
{
	PIXELFORMATDESCRIPTOR pfd=				
	{
		sizeof(PIXELFORMATDESCRIPTOR),				
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		(Core()->InitFlags() & EIF_FORCE_16_BIT_COLOR) ? 16 : 24,// Select Our Color Bits
		0, 0, 0, 0, 0, 0,							// Per Color Bits and Shifts Ignored
		8,											// Alpha Bits
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24 Bit Z-Buffer (Depth Buffer)  
		8,											// Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	int pixel_format = NULL;

	if (Core()->EngWindow()->eMultisampling != MM_NONE)
	{
		HWND	temp_win_handle	= NULL;
		HDC		temp_win_dc		= NULL;
		HGLRC	temp_win_rc		= NULL;
		int temp_pixel_format	= NULL;

		if (
		!(temp_win_handle = CreateWindowEx(0, "STATIC", NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL)) ||
		!(temp_win_dc = GetDC(temp_win_handle)) ||
		!(temp_pixel_format = ChoosePixelFormat(temp_win_dc, &pfd)) ||
		!SetPixelFormat(temp_win_dc, temp_pixel_format, &pfd) ||
		!(temp_win_rc = wglCreateContext(temp_win_dc)) ||
		!wglMakeCurrent(temp_win_dc, temp_win_rc)
		)
		{
			Core()->EngWindow()->eMultisampling = MM_NONE;
			LOG("Error(s) while performing OpenGL MSAA preinit routine.", LT_ERROR);
		}
		else
		{		
			if (std::string((char*)glGetString(GL_EXTENSIONS)).find("ARB_multisample") != std::string::npos)
			{	
				int ia_attributes[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_COLOR_BITS_ARB, (Core()->InitFlags() & EIF_FORCE_16_BIT_COLOR) ? 16 : 24,
					WGL_ALPHA_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, (int)Core()->EngWindow()->eMultisampling * 2,
					0, 0
				};

				float	fa_attributes[] = {0, 0};
				uint	formats_cnt;
				int		tmp_pixel_format;
				
				wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
				int valid = wglChoosePixelFormatARB(temp_win_dc, ia_attributes, fa_attributes, 1, &tmp_pixel_format, &formats_cnt);
				wglChoosePixelFormatARB = NULL;

				if(valid && formats_cnt >= 1)
					pixel_format = tmp_pixel_format;
				else
				{
					LOG("Can't find suitable PixelFormat with required MSAA " + IntToStr((int)Core()->EngWindow()->eMultisampling * 2) + "X support.", LT_WARNING);
					Core()->EngWindow()->eMultisampling = MM_NONE;
				}
			}
			else
				Core()->EngWindow()->eMultisampling = MM_NONE;
		}

		if (
		!wglMakeCurrent(NULL, NULL)||
		(temp_win_rc!=NULL&&!wglDeleteContext(temp_win_rc))||
		(temp_win_dc!=NULL&&!ReleaseDC(temp_win_handle, temp_win_dc))||
		(temp_win_handle!=NULL&&!DestroyWindow(temp_win_handle))
		)
			LOG("Can't free resources after performing OpenGL MSAA preinit routine.", LT_WARNING);
	}

	Core()->pMainWindow()->GetDrawContext(_hDC);

	if (pixel_format == NULL && !(pixel_format = ChoosePixelFormat(_hDC, &pfd)))
	{
		Finalize();
		LOG("Can't find a suitable PixelFormat.", LT_FATAL);
		return false;
	}

	if (!SetPixelFormat(_hDC, pixel_format, &pfd))		
	{
		Finalize();
		LOG("Can't set PixelFormat.", LT_FATAL);
		return false;
	}

	if (!(_hRC = wglCreateContext(_hDC)))				
	{
		Finalize();
		LOG("Can't create OpenGL Rendering Context.", LT_FATAL);
		return false;
	}

	if (!wglMakeCurrent(_hDC, _hRC))
	{
		Finalize();
		LOG("Can't active OpenGL Rendering Context.", LT_FATAL);
		return false;
	}

	return true;
}

bool CBaseRendererGL::MakeCurrent()
{
	if(wglGetCurrentContext() != _hRC)
		return wglMakeCurrent(_hDC, _hRC) != FALSE;
	else
		return true;
}

void CBaseRendererGL::Present()
{
	SwapBuffers(_hDC);
}

bool CBaseRendererGL::Finalize()
{
	if(_hRC)
	{
		if (!wglMakeCurrent(NULL, NULL))
			LOG("Failed to release Device Context and Rendering Context.", LT_ERROR);

		if (!wglDeleteContext(_hRC))
			LOG("Failed to release Rendering Context.", LT_ERROR);

		return true;
	}
	else
		return false;
}

bool CBaseRendererGL::AdjustMode(TEngineWindow &stNewWin)
{
	bool res = true;

	if (Core()->EngWindow()->eMultisampling != stNewWin.eMultisampling)
	{
		res = false;

		if (!WGLEW_ARB_multisample && stNewWin.eMultisampling != MM_NONE)
		{
			stNewWin.eMultisampling = MM_NONE;
			LOG("MSAA is not supported.", LT_WARNING);
		}
		else
		{
			stNewWin.eMultisampling = Core()->EngWindow()->eMultisampling;
			LOG("Changing MSAA samples count after initialization is not allowed.", LT_WARNING);
		}
	}

	if (WGLEW_EXT_swap_control)
	{
		int swapint = wglGetSwapIntervalEXT();
		
		if (stNewWin.bVSync)
		{
			if (swapint != 1)
				wglSwapIntervalEXT(1);
		}
		else
			if (swapint != 0)
				wglSwapIntervalEXT(0);

	}
	else 
	{
		res = false;
		stNewWin.bVSync = false;
		LOG("VSync is not supported.", LT_WARNING);
	}

	return res;
}

#endif