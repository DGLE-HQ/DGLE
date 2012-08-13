/**
\author		Korotkov Andrey aka DRON
\date		01.05.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender2D;

class CTexture: public CInstancedObj, public ITexture
{
	ICoreRenderer *_pCoreRenderer;
	CRender2D *_pRender2D;
	ICoreTexture *_pCoreTexture;
	uint _uiWidth, _uiHeight, _uiFrameWidth, _uiFrameHeight;
	float _quad[16];

public:

	CTexture(uint uiInstIdx, ICoreTexture *pCoreTex, uint uiWidth, uint uiHeight);
	~CTexture();

	HRESULT CALLBACK GetDimensions(uint &uiWidth, uint &uiHeight);
	HRESULT CALLBACK SetFrameSize(uint uiFrameWidth, uint uiFrameHeight);
	HRESULT CALLBACK GetFrameSize(uint &uiFrameWidth, uint &uiFrameHeight);
	
	HRESULT CALLBACK GetCoreTexture(ICoreTexture *&prCoreTex);
	
	HRESULT CALLBACK Draw2DSimple(int iX, int iY, uint uiFrameIndex);
	HRESULT CALLBACK Draw2D(int iX, int iY, uint uiWidth, uint uiHeight, float fAngle, uint uiFrameIndex);
	HRESULT CALLBACK Draw3D(uint uiFrameIndex);
	
	HRESULT CALLBACK Bind(uint uiMTextureLayer);

	HRESULT CALLBACK Free();
	HRESULT CALLBACK GetType(E_ENG_OBJ_TYPE &eObjType);
	HRESULT CALLBACK GetUnknownType(uint &uiObjUnknownType);

	IDGLE2_BASE_IMPLEMENTATION1(ITexture, IEngBaseObj)
};