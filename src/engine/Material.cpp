/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Material.h"

CMaterial::CMaterial(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{}

DGLE_RESULT DGLE_API CMaterial::SetDiffuseColor(const TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetSpecularColor(const TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::SetDiffuseTexture(ITexture *pTexture)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetDiffuseColor(TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetSpecularColor(TColor4 &stColor)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::GetDiffuseTexture(ITexture *&prTexture)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CMaterial::Bind()
{
	return S_OK;
}