/**
\author		Korotkov Andrey aka DRON
\date		24.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Material.h"
#include "Render.h"
#include "Render3D.h"

CMaterial::CMaterial(uint uiInstIdx):
CInstancedObj(uiInstIdx), _stDiffCol(ColorGray()),
_stSpecCol(ColorBlack()), _fShine(0.f), _pDiffTex(NULL)
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
	_fShine = fShininess;
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetDiffuseTexture(ITexture *pTexture)
{
	_pDiffTex = pTexture;
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

DGLE_RESULT DGLE_API CMaterial::Bind()
{
	Core()->pRender()->pRender3D()->BindMaterial(this);
	return S_OK;
}