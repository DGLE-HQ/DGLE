/**
\author		Korotkov Andrey aka DRON
\date		24.02.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"
#include "ResourceManager.h"

class CMesh: public CInstancedObj, public IMesh
{
	ICoreRenderer *_pCoreRenderer;
	IModel *_pOwnerModel;
	ICoreGeometryBuffer *_pBuffer;
	TPoint3 _stCenter;
	TVector3 _stExtents;

	inline bool _IsInterleaved(const TDrawDataDesc &stDesc) const;
	void _CopyMeshData(TDrawDataDesc &stDesc, uint &uiStride, uint &uiVerticesDataSize, uint &uiVerticesCount, uint &uiIndexesDataSize, uint &uiIndixesCount);

public:

	CMesh(uint uiInstIdx, ICoreGeometryBuffer *pBuffer, const TPoint3 &stCenter, const TVector3 &stExtents);
	~CMesh();

	DGLE_RESULT DGLE_API Draw();
	DGLE_RESULT DGLE_API GetCenter(TPoint3 &stCenter);
	DGLE_RESULT DGLE_API GetExtents(TVector3 &stExtents);
	DGLE_RESULT DGLE_API GetTrianglesCount(uint &uiCnt);
	DGLE_RESULT DGLE_API GetGeometryBuffer(ICoreGeometryBuffer *&prBuffer);
	DGLE_RESULT DGLE_API SetGeometryBuffer(ICoreGeometryBuffer *pBuffer, bool bFreeCurrentBuffer);
	DGLE_RESULT DGLE_API RecalculateNormals();
	DGLE_RESULT DGLE_API RecalculateTangentSpace();
	DGLE_RESULT DGLE_API RecalculateBounds();
	DGLE_RESULT DGLE_API TransformVertices(const TMatrix4x4 &stTransMatrix);
	DGLE_RESULT DGLE_API Optimize();
	DGLE_RESULT DGLE_API GetOwner(IModel *&prModel);
	DGLE_RESULT DGLE_API SetOwner(IModel *pModel);

	IENGBASEOBJ_IMPLEMENTATION(EOT_MESH)
	IDGLE_BASE_IMPLEMENTATION(IMesh, INTERFACE_IMPL(IEngBaseObj, INTERFACE_IMPL_END))
};