/**
\author		Andrey Korotkov aka DRON
\date		22.03.2016 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_RENDERER

#include <gl\glew.h>
#include <gl\wglew.h>

class CBaseRendererGL : public CInstancedObj, public CPlatformBaseRenderGL
{
	HGLRC _hRC;
	TWindowDrawHandle _hDC;
	E_MULTISAMPLING_MODE _eMultisampling;

public:
	
	CBaseRendererGL(uint uiInstIdx);

	bool Prepare();
	bool Initialize(TEngineWindow &stWin, E_ENGINE_INIT_FLAGS &eInitFlags);
	bool Finalize();
	bool AdjustMode(TEngineWindow &stNewWin);
	bool MakeCurrent();
	void Present();

};

#endif