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

	DGLE2_RESULT DGLE2_API GetDimensions(uint &uiWidth, uint &uiHeight);
	DGLE2_RESULT DGLE2_API SetFrameSize(uint uiFrameWidth, uint uiFrameHeight);
	DGLE2_RESULT DGLE2_API GetFrameSize(uint &uiFrameWidth, uint &uiFrameHeight);
	
	DGLE2_RESULT DGLE2_API GetCoreTexture(ICoreTexture *&prCoreTex);
	
	DGLE2_RESULT DGLE2_API Draw2DSimple(int iX, int iY, uint uiFrameIndex);
	DGLE2_RESULT DGLE2_API Draw2D(int iX, int iY, uint uiWidth, uint uiHeight, float fAngle, uint uiFrameIndex);
	DGLE2_RESULT DGLE2_API Draw3D(uint uiFrameIndex);
	
	DGLE2_RESULT DGLE2_API Bind(uint uiMTextureLayer);

	DGLE2_RESULT DGLE2_API Free();
	DGLE2_RESULT DGLE2_API GetType(E_ENG_OBJ_TYPE &eObjType);
	DGLE2_RESULT DGLE2_API GetUnknownType(uint &uiObjUnknownType);

	IDGLE2_BASE_IMPLEMENTATION1(ITexture, IEngBaseObj)
};