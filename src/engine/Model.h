/**
\author		Korotkov Andrey aka DRON
\date		19.02.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CModel: public CInstancedObj, public IModel
{

public:

	CModel(uint uiInstIdx);
	~CModel();

	DGLE_RESULT DGLE_API Draw();
	DGLE_RESULT DGLE_API DrawMesh(uint uiMeshIdx);
	DGLE_RESULT DGLE_API GetCenter(TPoint3 &stCenter);
	DGLE_RESULT DGLE_API GetExtents(TVector3 &stExtents);
	DGLE_RESULT DGLE_API MeshsCount(uint &uiCount);
	DGLE_RESULT DGLE_API GetMesh(uint uiIdx, IMesh *&prMesh);
	DGLE_RESULT DGLE_API SetModelMaterial(IMaterial *pMaterial);
	DGLE_RESULT DGLE_API GetModelMaterial(IMaterial *&prMaterial);
	DGLE_RESULT DGLE_API SetMeshMaterial(uint uiMeshIdx, IMaterial *pMaterial);
	DGLE_RESULT DGLE_API GetMeshMaterial(uint uiMeshIdx, IMaterial *&prMaterial);
	DGLE_RESULT DGLE_API AddMesh(IMesh *pMesh);
	DGLE_RESULT DGLE_API RemoveMesh(IMesh *pMesh);
	DGLE_RESULT DGLE_API ReplaceMesh(uint uiMeshIdx, IMesh *pMesh);

	IENGBASEOBJ_IMPLEMENTATION(EOT_MODEL)
	IDGLE_BASE_IMPLEMENTATION(IModel, INTERFACE_IMPL(IEngBaseObj, INTERFACE_IMPL_END))
};