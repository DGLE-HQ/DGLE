/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CRender2D;
class CRender3D;

class CTexture: public CInstancedObj, public ITexture
{
	CRender2D *_pRender2D;
	CRender3D *_pRender3D;
	ICoreTexture *_pCoreTexture;
	uint _uiWidth, _uiHeight, _uiFrameWidth, _uiFrameHeight;
	float _afQuad[28];

public:

	CTexture(uint uiInstIdx, ICoreTexture *pCoreTex, uint uiWidth, uint uiHeight);
	~CTexture();

	DGLE_RESULT DGLE_API GetDimensions(uint &uiWidth, uint &uiHeight) override;
	DGLE_RESULT DGLE_API SetFrameSize(uint uiFrameWidth, uint uiFrameHeight) override;
	DGLE_RESULT DGLE_API GetFrameSize(uint &uiFrameWidth, uint &uiFrameHeight) override;
	DGLE_RESULT DGLE_API FramesCount(uint &uiCount) override;

	DGLE_RESULT DGLE_API GetCoreTexture(ICoreTexture *&prCoreTex) override;
	
	DGLE_RESULT DGLE_API Draw2DSimple(int iX, int iY, uint uiFrameIndex) override;
	DGLE_RESULT DGLE_API Draw2D(int iX, int iY, uint uiWidth, uint uiHeight, float fAngle, uint uiFrameIndex) override;
	DGLE_RESULT DGLE_API Draw3D(uint uiFrameIndex) override;
	
	DGLE_RESULT DGLE_API Bind(uint uiTextureLayer) override;

	IENGINE_BASE_OBJECT_IMPLEMENTATION(EOT_TEXTURE)
	IDGLE_BASE_IMPLEMENTATION(ITexture, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};