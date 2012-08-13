/**
\author		Korotkov Andrey aka DRON
\date		29.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender2D;
class CRender3D;

class CRender: public CInstancedObj, public IRender
{
	float _fFovAngle, _fZNear, _fZFar;
	int _iMaxTexResolution, _iMaxAnisotropy, _iMaxLight, _iMaxTexUnits;

	ICoreRenderer *_pCoreRenderer;

	CRender2D *_pRender2D;
	CRender3D *_pRender3D;

	void _SetPerspectiveMatrix(uint width, uint height);

public:

	CRender(uint uiInstIdx);
	~CRender();

	inline CRender2D* pRender2D() { return _pRender2D; }
	inline CRender3D* pRender3D() { return _pRender3D; }

	void BeginRender();
	void EndRender();
	void OnResize(uint uiWidth, uint uiHeight);

	HRESULT CALLBACK SetClearColor(const TColor4 &stColor);
	HRESULT CALLBACK Unbind(E_ENG_OBJ_TYPE eType);
	HRESULT CALLBACK EnableScissor(const TRectF &stArea);	
	HRESULT CALLBACK DisableScissor();
	HRESULT CALLBACK SetRenderTarget(ITexture* pTargetTex);
	HRESULT CALLBACK ScreenshotBMP(const char* pFileName);
	HRESULT CALLBACK CreatePostProcess(IPostprocess *&pPP);

	HRESULT CALLBACK GetRender2D(IRender2D *&prRender2D);
	HRESULT CALLBACK GetRender3D(IRender3D *&prRender3D);

	HRESULT CALLBACK GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE2_BASE_IMPLEMENTATION1(IRender, IEngineSubSystem)
};