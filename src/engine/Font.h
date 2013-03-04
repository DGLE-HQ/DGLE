/**
\author		Korotkov Andrey aka DRON
\date		05.05.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CBitmapFont: public CInstancedObj, public IBitmapFont
{
public:
#pragma pack(push, 1)
	struct TCharBox
	{
		uint32 x, y, w, h;
	};
	
	struct TFontHeader
	{
		uint8	bitDepth;
		uint32	texWidth,
				texHeight;
	};
#pragma pack(pop)

private:

	float		*_pBuffer;
	uint		_uiBufferSize;

	float		 _fScale;
 
	TFontHeader	 _stHeader;
	TCharBox	 _astChars[224];

	ITexture	*_pTex;

public:
	CBitmapFont(uint uiInstIdx, ITexture *pTex, const TFontHeader &stHeader, TCharBox *pChars);
	~CBitmapFont();

	DGLE_RESULT DGLE_API GetTexture(ITexture *&prTexture);
	DGLE_RESULT DGLE_API SetScale(float fScale);
	DGLE_RESULT DGLE_API GetScale(float &fScale);
	DGLE_RESULT DGLE_API GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight);
	DGLE_RESULT DGLE_API Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor);
	DGLE_RESULT DGLE_API Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor, float fAngle, bool bVerticesColors);
	DGLE_RESULT DGLE_API Draw3D(const char *pcTxt);
	
	IENGINE_BASE_OBJECT_IMPLEMENTATION(EOT_BITMAP_FONT)
	IDGLE_BASE_IMPLEMENTATION(IBitmapFont, INTERFACE_IMPL(IEngineBaseObject, INTERFACE_IMPL_END))
};