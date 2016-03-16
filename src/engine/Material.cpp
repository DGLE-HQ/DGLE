/**
\author		Korotkov Andrey aka DRON
\date		16.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "Material.h"
#include "Render.h"
#include "Render3D.h"

CMaterial::CMaterial(uint uiInstIdx):
CInstancedObj(uiInstIdx), _stDiffCol(ColorWhite()),
_stSpecCol(ColorBlack()), _fShine(50.f), _pDiffTex(NULL),
_doBlending(false), _eBlendingMode(BE_NORMAL),
_doAlphaTest(false), _fAlphaTreshold(0.25f)
{}

DGLE_RESULT DGLE_API CMaterial::SetDiffuseColor(const TColor4 &stColor)
{
	_stDiffCol = stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetSpecularColor(const TColor4 &stColor)
{
	_stSpecCol = stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetShininess(float fShininess)
{
	if (fShininess < 0.f || fShininess > 100.f)
		return E_INVALIDARG;

	_fShine = fShininess;

	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetDiffuseTexture(ITexture *pTexture)
{
	_pDiffTex = pTexture;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetBlending(bool bEnabled, E_BLENDING_EFFECT eMode)
{
	_doBlending = bEnabled;
	_eBlendingMode = eMode;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetAlphaTest(bool bEnabled, float fTreshold)
{
	_doAlphaTest = bEnabled;
	_fAlphaTreshold = fTreshold;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetDiffuseColor(TColor4 &stColor)
{
	stColor = _stDiffCol;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetSpecularColor(TColor4 &stColor)
{
	stColor = _stSpecCol;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetShininess(float &fShininess)
{
	fShininess = _fShine;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetDiffuseTexture(ITexture *&prTexture)
{
	prTexture = _pDiffTex;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetBlending(bool &bEnabled, E_BLENDING_EFFECT &eMode)
{
	bEnabled = _doBlending;
	eMode = _eBlendingMode;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetAlphaTest(bool &bEnabled, float &fTreshold)
{
	bEnabled = _doAlphaTest;
	fTreshold = _fAlphaTreshold;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::Bind()
{
	Core()->pRender()->pRender3D()->BindMaterial(this);
	return S_OK;
}