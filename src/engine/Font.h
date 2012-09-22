/**
\author		Korotkov Andrey aka DRON
\date		05.05.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#pragma once

#include "Common.h"

class CBitmapFont: public CInstancedObj, public IBitmapFont
{
public:
#pragma pack(push, 1)
	struct TCharBox
	{
		uint32 iX, iY, iW, iH;
	};
	
	struct TFontHeader
	{
		uint8	ubBitdepth;
		uint32	iTexWidth,
				iTexHeight;
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

	HRESULT DGLE2_API GetTexture(ITexture *&prTexture);
	HRESULT DGLE2_API SetScale(const float &fScale);
	HRESULT DGLE2_API GetScale(float &fScale);
	HRESULT DGLE2_API GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight);
	HRESULT DGLE2_API Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor);
	HRESULT DGLE2_API Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor, float fAngle, bool bVerticesColors);
	HRESULT DGLE2_API Draw3D(const char *pcTxt);
	
	HRESULT DGLE2_API Free();
	HRESULT DGLE2_API GetType(E_ENG_OBJ_TYPE &eObjType);
	HRESULT DGLE2_API GetUnknownType(uint &uiObjUnknownType);

	IDGLE2_BASE_IMPLEMENTATION1(IBitmapFont, IEngBaseObj)
};