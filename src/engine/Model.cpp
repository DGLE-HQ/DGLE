/**
\author		Korotkov Andrey aka DRON
\date		10.02.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Model.h"

CModel::CModel(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{}

CModel::~CModel()
{
}

DGLE_RESULT DGLE_API CModel::Draw()
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::DrawMesh(uint uiMeshIdx)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetCenter(TPoint3 &stCenter)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetExtents(TVector3 &stExtents)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::MeshsCount(uint &uiCount)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetMesh(uint uiIdx, IMesh *&prMesh)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::SetModelMaterial(IMaterial *pMaterial)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetModelMaterial(IMaterial *&prMaterial)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::SetMeshMaterial(uint uiMeshIdx, IMaterial *pMaterial)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetMeshMaterial(uint uiMeshIdx, IMaterial *&prMaterial)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::AddMesh(IMesh *pMesh)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::RemoveMesh(IMesh *pMesh)
{
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::ReplaceMesh(uint uiMeshIdx, IMesh *pMesh)
{
	return S_OK;
}