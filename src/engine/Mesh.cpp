/**
\author		Korotkov Andrey aka DRON
\date		23.03.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Common.h"
#include "Mesh.h"
#include "Render.h"
#include "Render3D.h"

CMesh::CMesh(uint uiInstIdx, ICoreGeometryBuffer *pBuffer, const TPoint3 &stCenter, const TVector3 &stExtents):
CInstancedObj(uiInstIdx), _pOwnerModel(NULL), _pBuffer(pBuffer), _stCenter(stCenter), _stExtents(stExtents)
{
	_pRender3D = Core()->pRender()->pRender3D();
}

CMesh::~CMesh()
{
	if (_pOwnerModel)
		_pOwnerModel->RemoveMesh(this);

	_pBuffer->Free();
}

inline bool CMesh::_IsInterleaved(const TDrawDataDesc &stDesc) const
{
	uint stride = stDesc.uiVertexStride;

	if (stDesc.uiNormalOffset != -1 && stDesc.uiNormalStride != stride)
		return false;

	if (stDesc.uiTextureVertexOffset != -1 && stDesc.uiTextureVertexStride != stride)
		return false;

	if (stDesc.uiColorOffset != -1 && stDesc.uiColorStride != stride)
		return false;

	if (stDesc.uiTangentOffset != -1 && stDesc.uiTangentStride != stride)
		return false;

	if (stDesc.uiBinormalOffset != -1 && stDesc.uiBinormalStride != stride)
		return false;

	if (stDesc.pAttribs)
	{
		/* not implemented */
	}

	return stride == 0 ? false : true;
}

void CMesh::_CopyMeshData(TDrawDataDesc &stDesc, uint &uiStride, uint &uiVerticesDataSize, uint &uiVerticesCount, uint &uiIndexesDataSize, uint &uiIndixesCount)
{
	if (!_pBuffer)
		return;

	uiStride = 3 * sizeof(float);
	_pBuffer->GetBufferDrawDataDesc(stDesc);

	if (_IsInterleaved(stDesc))
		uiStride = stDesc.uiVertexStride; // any stride is suitable when all data is interleaved

	_pBuffer->GetBufferDimensions(uiVerticesDataSize, uiVerticesCount, uiIndexesDataSize, uiIndixesCount);

	stDesc.pData = new uint8[uiVerticesDataSize + uiIndexesDataSize];
	stDesc.pIndexBuffer = &stDesc.pData[uiVerticesDataSize];

	PARANOIC_CHECK_RES(_pBuffer->GetGeometryData(stDesc, uiVerticesDataSize, uiIndexesDataSize));
}

bool CMesh::_SaveToFile(IFile *pFile)
{
	if (!_pBuffer)
		return false;

	TDrawDataDesc desc;
	uint stride, verts_data_size, verts_count, idxs_data_size, idxs_count;
	_CopyMeshData(desc, stride, verts_data_size, verts_count, idxs_data_size, idxs_count);

	if (_IsInterleaved(desc))
	{
		//ToDo: Add ability to save as interleaved when DMD format will support it.
		delete[] desc.pData;
		return false;
	}
	else
	{
		uint ui_written;
		
		bool textured = desc.uiTextureVertexOffset != -1;
		pFile->Write(&textured, sizeof(bool), ui_written);

		bool tangent_space = false; //ToDo: Write tangent space if calculated.
		pFile->Write(&tangent_space, sizeof(bool), ui_written);

		pFile->Write(&verts_count, sizeof(uint), ui_written);
		pFile->Write(&idxs_count, sizeof(uint), ui_written);

		pFile->Write(_stCenter.xyz, sizeof(TPoint3), ui_written);
		pFile->Write(_stExtents.xyz, sizeof(TVector3), ui_written);

		pFile->Write(&desc.pData[0], sizeof(float) * 3 * verts_count, ui_written);
		pFile->Write(&desc.pData[desc.uiNormalOffset], sizeof(float) * 3 * verts_count, ui_written);

		if (textured)
			pFile->Write(&desc.pData[desc.uiTextureVertexOffset], sizeof(float) * 2 * verts_count, ui_written);

		pFile->Write(desc.pIndexBuffer, idxs_data_size, ui_written);
	}

	delete[] desc.pData;

	return true;
}

DGLE_RESULT DGLE_API CMesh::Draw()
{
	if (!_pBuffer)
		return S_FALSE;

	_pRender3D->DrawBuffer(_pBuffer);

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::GetCenter(TPoint3 &stCenter)
{
	stCenter = _stCenter;
	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::GetExtents(TVector3 &stExtents)
{
	stExtents = _stExtents;
	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::GetTrianglesCount(uint &uiCnt)
{
	if (!_pBuffer)
	{
		uiCnt = 0;
		return S_FALSE;
	}

	uint vd, vc, id, ic;
	_pBuffer->GetBufferDimensions(vd, vc, id, ic);

	if (ic == 0)
		uiCnt = vc / 3;
	else
		uiCnt = ic / 3;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::GetGeometryBuffer(ICoreGeometryBuffer *&prBuffer)
{
	prBuffer = _pBuffer;
	return _pBuffer ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CMesh::SetGeometryBuffer(ICoreGeometryBuffer *pBuffer, bool bFreeCurrentBuffer)
{
	E_CORE_RENDERER_DRAW_MODE draw_mode;
	pBuffer->GetBufferDrawMode(draw_mode);

	if (draw_mode != CRDM_TRIANGLES)
		return E_INVALIDARG;

	if (_pBuffer && bFreeCurrentBuffer)
		_pBuffer->Free();

	_pBuffer = pBuffer;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::RecalculateNormals(bool bInvert)
{
	if (!_pBuffer)
		return S_FALSE;

	TDrawDataDesc desc;
	uint stride, verts_data_size, verts_count, idxs_data_size, idxs_count;
	_CopyMeshData(desc, stride, verts_data_size, verts_count, idxs_data_size, idxs_count);

	TDrawDataDesc desc_new(desc);
	bool do_delete_new = false;

	if (desc.uiNormalOffset == -1)
	{
		do_delete_new = true;

		const uint new_verts_data_size = verts_data_size + verts_count * 3 * sizeof(float);
		desc_new.pData = new uint8[new_verts_data_size + idxs_data_size];
		memcpy(desc_new.pData, desc.pData, verts_data_size);

		desc_new.uiNormalOffset = verts_data_size;
		desc_new.uiNormalStride = 0;

		if (idxs_count != 0)
		{
			desc_new.pIndexBuffer = &desc_new.pData[new_verts_data_size];
			memcpy(&desc_new.pData[new_verts_data_size], &desc.pData[verts_data_size], idxs_data_size);
		}

		memset(&desc_new.pData[verts_data_size], 0, new_verts_data_size - verts_data_size);
	}
	else
	{
		const uint stride = desc_new.uiNormalStride == 0 ? 3 * sizeof(float) : desc_new.uiNormalStride;

		for (uint i = 0; i < verts_count; ++i)
			*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiNormalOffset + i * stride])) = TVector3();
	}

	const uint n_stride = desc_new.uiNormalStride == 0 ? 3 * sizeof(float) : desc_new.uiNormalStride,
		v_stride = desc_new.uiVertexStride == 0 ? 3 * sizeof(float) : desc_new.uiVertexStride,
		count = idxs_count == 0 ? verts_count / 3 : idxs_count / 3;

	for(uint i = 0; i < count; ++i)
	{
		uint face[3];

		if (idxs_count == 0)
		{
			face[0] = i * 3;
			face[1] = i * 3 + 1;
			face[2] = i * 3 + 2;
		}
		else
			if (desc_new.bIndexBuffer32)
			{
				face[0] = reinterpret_cast<uint *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint)])[0];
				face[1] = reinterpret_cast<uint *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint)])[1];
				face[2] = reinterpret_cast<uint *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint)])[2];
			}
			else
			{
				face[0] = reinterpret_cast<uint16 *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint16)])[0];
				face[1] = reinterpret_cast<uint16 *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint16)])[1];
				face[2] = reinterpret_cast<uint16 *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint16)])[2];
			}

		const TPoint3 * const v[3] = {
			reinterpret_cast<TPoint3 *>(&desc_new.pData[face[0] * v_stride]),
			reinterpret_cast<TPoint3 *>(&desc_new.pData[face[1] * v_stride]),
			reinterpret_cast<TPoint3 *>(&desc_new.pData[face[2] * v_stride])};

		const TVector3 faset_normal = ((*v[0] - *v[1]).Cross(*v[1] - *v[2])).Normalize() * (bInvert ? -1.f : 1.f);

		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiNormalOffset + face[0] * n_stride])) += faset_normal;
		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiNormalOffset + face[1] * n_stride])) += faset_normal;
		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiNormalOffset + face[2] * n_stride])) += faset_normal;
	}

	for (uint i = 0; i < verts_count; ++i)
		(*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiNormalOffset + i * n_stride]))).Normalize();

	PARANOIC_CHECK_RES(_pBuffer->Reallocate(desc_new, verts_count, idxs_count, CRDM_TRIANGLES));

	if (do_delete_new)
		delete[] desc_new.pData;

	delete[] desc.pData;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::RecalculateTangentSpace()
{
	if (!_pBuffer)
		return S_FALSE;
	
	TDrawDataDesc desc;
		
	_pBuffer->GetBufferDrawDataDesc(desc);

	if (desc.uiTextureVertexOffset == -1 ||
		(desc.uiTangentOffset == -1 && desc.uiBinormalOffset != -1) || (desc.uiTangentOffset != -1 && desc.uiBinormalOffset == -1)) // confusing situation should never happen
		return E_ABORT;

	if (desc.uiNormalOffset == -1)
		RecalculateNormals(false);

	uint stride, verts_data_size, verts_count, idxs_data_size, idxs_count;
	_CopyMeshData(desc, stride, verts_data_size, verts_count, idxs_data_size, idxs_count);

	TDrawDataDesc desc_new(desc);
	bool do_delete_new = false;

	if (desc.uiTangentOffset == -1 && desc.uiBinormalOffset == -1)
	{
		do_delete_new = true;

		const uint new_verts_data_size = verts_data_size + verts_count * 6 * sizeof(float);
		desc_new.pData = new uint8[new_verts_data_size + idxs_data_size];
		memcpy(desc_new.pData, desc.pData, verts_data_size);

		desc_new.uiTangentOffset = verts_data_size;
		desc_new.uiTangentStride = 0;

		desc_new.uiBinormalOffset = verts_data_size + verts_count * 3 * sizeof(float);
		desc_new.uiBinormalStride = 0;

		if (idxs_count != 0)
		{
			desc_new.pIndexBuffer = &desc_new.pData[new_verts_data_size];
			memcpy(&desc_new.pData[new_verts_data_size], &desc.pData[verts_data_size], idxs_data_size);
		}

		memset(&desc_new.pData[verts_data_size], 0, new_verts_data_size - verts_data_size);
	}
	else
	{
		const uint t_stride = desc_new.uiTangentStride == 0 ? 3 * sizeof(float) : desc_new.uiTangentStride,
			b_stride = desc_new.uiBinormalStride == 0 ? 3 * sizeof(float) : desc_new.uiBinormalStride;

		for (uint i = 0; i < verts_count; ++i)
		{
			*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiTangentOffset + i * t_stride])) = TVector3();
			*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiBinormalOffset + i * b_stride])) = TVector3();
		}
	}

	const uint t_stride = desc_new.uiTangentStride == 0 ? 3 * sizeof(float) : desc_new.uiTangentStride,
		b_stride = desc_new.uiBinormalStride == 0 ? 3 * sizeof(float) : desc_new.uiBinormalStride,
		tex_stride = desc_new.uiTextureVertexStride == 0 ? 2 * sizeof(float) : desc_new.uiTextureVertexStride,
		v_stride = desc_new.uiVertexStride == 0 ? 3 * sizeof(float) : desc_new.uiVertexStride,
		n_stride = desc_new.uiNormalStride == 0 ? 3 * sizeof(float) : desc_new.uiNormalStride,
		count = idxs_count == 0 ? verts_count / 3 : idxs_count / 3;

	for(uint i = 0; i < count; ++i)
	{
		uint face[3];

		if (idxs_count == 0)
		{
			face[0] = i * 3;
			face[1] = i * 3 + 1;
			face[2] = i * 3 + 2;
		}
		else
			if (desc_new.bIndexBuffer32)
			{
				face[0] = reinterpret_cast<uint *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint)])[0];
				face[1] = reinterpret_cast<uint *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint)])[1];
				face[2] = reinterpret_cast<uint *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint)])[2];
			}
			else
			{
				face[0] = reinterpret_cast<uint16 *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint16)])[0];
				face[1] = reinterpret_cast<uint16 *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint16)])[1];
				face[2] = reinterpret_cast<uint16 *>(&desc_new.pIndexBuffer[i * 3 * sizeof(uint16)])[2];
			}
			
		const TPoint3 * const v[3] = {
			reinterpret_cast<TPoint3 *>(&desc_new.pData[face[0] * v_stride]),
			reinterpret_cast<TPoint3 *>(&desc_new.pData[face[1] * v_stride]),
			reinterpret_cast<TPoint3 *>(&desc_new.pData[face[2] * v_stride])};

		const TPoint2 * const t[3] = {
			reinterpret_cast<TPoint2 *>(&desc_new.pData[desc_new.uiTextureVertexOffset + face[0] * tex_stride]),
			reinterpret_cast<TPoint2 *>(&desc_new.pData[desc_new.uiTextureVertexOffset + face[1] * tex_stride]),
			reinterpret_cast<TPoint2 *>(&desc_new.pData[desc_new.uiTextureVertexOffset + face[2] * tex_stride])};

		const TVector3 v1 = *v[1] - *v[0], v2 = *v[2] - *v[0];
		const TVector2 v3 = *t[1] - *t[0], v4 = *t[2] - *t[0];

		const float d = v3.Cross(v4);

		if (d == 0.f) continue;

		const float r = 1.f / d;

		const TVector3 sdir = TVector3(v4.y * v1.x - v4.x * v2.x, v4.y * v1.y - v4.x * v2.y, v4.y * v1.z - v4.x * v2.z) * r,
			tdir = TVector3(v3.x * v2.x - v3.y * v1.x, v3.x * v2.y - v3.y * v1.y, v3.x * v2.z - v3.y * v1.z) * r;

		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiTangentOffset + face[0] * t_stride])) += sdir;
		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiTangentOffset + face[1] * t_stride])) += sdir;
		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiTangentOffset + face[2] * t_stride])) += sdir;

		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiBinormalOffset + face[0] * b_stride])) += tdir;
		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiBinormalOffset + face[1] * b_stride])) += tdir;
		*(reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiBinormalOffset + face[2] * b_stride])) += tdir;
	}

	for (uint i = 0; i < verts_count; ++i)
	{
		const TVector3 * const normal = reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiNormalOffset + i * n_stride]);
		TVector3 *tangent = reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiTangentOffset + i * t_stride]),
			*binormal = reinterpret_cast<TVector3 *>(&desc_new.pData[desc_new.uiBinormalOffset + i * b_stride]);
		
		float dot_1 = normal->Dot(*tangent), dot_2;
		*tangent = (*tangent - *normal * dot_1).Normalize();
		dot_1 = normal->Dot(*tangent);
		dot_2 = tangent->Dot(*binormal);
		*binormal = (*binormal - *normal * dot_1 + *tangent * dot_2).Normalize();
	}

	PARANOIC_CHECK_RES(_pBuffer->Reallocate(desc_new, verts_count, idxs_count, CRDM_TRIANGLES));

	if (do_delete_new)
		delete[] desc_new.pData;

	delete[] desc.pData;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::RecalculateBounds()
{
	if (!_pBuffer)
		return S_FALSE;

	TPoint3 min_dem((std::numeric_limits<float>::infinity)(), (std::numeric_limits<float>::infinity)(), (std::numeric_limits<float>::infinity)()),
		max_dem(-(std::numeric_limits<float>::infinity)(), -(std::numeric_limits<float>::infinity)(), -(std::numeric_limits<float>::infinity)());
	
	TDrawDataDesc desc;
	uint stride, verts_data_size, verts_count, idxs_data_size, idxs_count;
	_CopyMeshData(desc, stride, verts_data_size, verts_count, idxs_data_size, idxs_count);

	for (uint i = 0; i < verts_count; ++i)
	{
		const TPoint3 * const p = reinterpret_cast<TPoint3 *>(&desc.pData[stride * i]);

		max_dem.x = fmax(p->x, max_dem.x);
		min_dem.x = fmin(p->x, min_dem.x);

		max_dem.y = fmax(p->y, max_dem.y);
		min_dem.y = fmin(p->y, min_dem.y);

		max_dem.z = fmax(p->z, max_dem.z);
		min_dem.z = fmin(p->z, min_dem.z);
	}

	delete[] desc.pData;

	_stCenter = min_dem + (max_dem - min_dem) / 2.f;
	_stExtents = (max_dem - min_dem) / 2.f;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::TransformVertices(const TMatrix4x4 &stTransMatrix)
{
	if (!_pBuffer)
		return S_FALSE;

	TDrawDataDesc desc;
	uint stride, verts_data_size, verts_count, idxs_data_size, idxs_count;
	_CopyMeshData(desc, stride, verts_data_size, verts_count, idxs_data_size, idxs_count);

	for (uint i = 0; i < verts_count; ++i)
	{
		TPoint3 *p = reinterpret_cast<TPoint3 *>(&desc.pData[stride * i]);
		*p = stTransMatrix.ApplyToPoint(*p);
	}

	PARANOIC_CHECK_RES(_pBuffer->Reallocate(desc, verts_count, idxs_count, CRDM_TRIANGLES));

	delete[] desc.pData;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::Optimize()
{
	if (!_pBuffer)
		return S_FALSE;

	TDrawDataDesc desc;
	_pBuffer->GetBufferDrawDataDesc(desc);

	if (_IsInterleaved(desc))
		return S_FALSE;

	uint verts_data_size, verts_count, idxs_data_size, idxs_count;
	_pBuffer->GetBufferDimensions(verts_data_size, verts_count, idxs_data_size, idxs_count);

	desc.pData = new uint8[verts_data_size + idxs_data_size];
	desc.pIndexBuffer = &desc.pData[verts_data_size];

	PARANOIC_CHECK_RES(_pBuffer->GetGeometryData(desc, verts_data_size, idxs_data_size));

	TDrawDataDesc desc_optimized(desc);
	bool do_ushort_indexes = verts_count < 65536 && desc.bIndexBuffer32;
	desc_optimized.pData = new uint8[verts_data_size + (do_ushort_indexes ? idxs_data_size / 2 : idxs_data_size)];
	desc_optimized.pIndexBuffer = &desc_optimized.pData[verts_data_size];

	uint offset = 0;
		
	for (uint i = 0; i < verts_count; ++i)
	{
		uint stride = desc.uiVertexStride == 0 ? 3 * sizeof(float) : desc.uiVertexStride;
		*(reinterpret_cast<TPoint3 *>(&desc_optimized.pData[offset])) = *(reinterpret_cast<TPoint3 *>(&desc.pData[i * stride]));
		offset += 3 * sizeof(float);

		if (desc.uiTextureVertexOffset != -1)
		{
			stride = desc.uiTextureVertexStride == 0 ? 2 * sizeof(float) : desc.uiTextureVertexStride;
			*(reinterpret_cast<TPoint2 *>(&desc_optimized.pData[offset])) = *(reinterpret_cast<TPoint2 *>(&desc.pData[desc.uiTextureVertexOffset + i * stride]));
			offset += 2 * sizeof(float);
		}

		if (desc.uiColorOffset != -1)
		{
			stride = desc.uiColorStride == 0 ? 4 * sizeof(float) : desc.uiTextureVertexStride;
			*(reinterpret_cast<TColor4 *>(&desc_optimized.pData[offset])) = *(reinterpret_cast<TColor4 *>(&desc.pData[desc.uiColorOffset + i * stride]));
			offset += 4 * sizeof(float);
		}

		if (desc.uiNormalOffset != -1)
		{
			stride = desc.uiNormalStride == 0 ? 3 * sizeof(float) : desc.uiNormalStride;
			*(reinterpret_cast<TVector3 *>(&desc_optimized.pData[offset])) = *(reinterpret_cast<TVector3 *>(&desc.pData[desc.uiNormalOffset + i * stride]));
			offset += 3 * sizeof(float);
		}

		if (desc.uiTangentOffset != -1)
		{
			stride = desc.uiTangentStride == 0 ? 3 * sizeof(float) : desc.uiTangentStride;
			*(reinterpret_cast<TVector3 *>(&desc_optimized.pData[offset])) = *(reinterpret_cast<TVector3 *>(&desc.pData[desc.uiTangentOffset + i * stride]));
			offset += 3 * sizeof(float);
		}

		if (desc.uiBinormalOffset != -1)
		{
			stride = desc.uiBinormalStride == 0 ? 3 * sizeof(float) : desc.uiBinormalStride;
			*(reinterpret_cast<TVector3 *>(&desc_optimized.pData[offset])) = *(reinterpret_cast<TVector3 *>(&desc.pData[desc.uiBinormalOffset + i * stride]));
			offset += 3 * sizeof(float);
		}

		if (desc.pAttribs)
		{
			/* not implemented */
		}
	}

	offset = 3 * sizeof(float);

	if (desc_optimized.uiTextureVertexOffset != -1)
	{
		desc_optimized.uiTextureVertexOffset = offset;
		offset += 2 * sizeof(float);
	}

	if (desc_optimized.uiColorOffset != -1)
	{
		desc_optimized.uiColorOffset = offset;
		offset += 4 * sizeof(float);
	}

	if (desc_optimized.uiNormalOffset != -1)
	{
		desc_optimized.uiNormalOffset = offset;
		offset += 3 * sizeof(float);
	}

	if (desc_optimized.uiTangentOffset != -1)
	{
		desc_optimized.uiTangentOffset = offset;
		offset += 3 * sizeof(float);
	}

	if (desc_optimized.uiBinormalOffset != -1)
	{
		desc_optimized.uiBinormalOffset = offset;
		offset += 3 * sizeof(float);
	}

	if (desc_optimized.pAttribs)
	{
		/* not implemented */
	}

	desc_optimized.uiVertexStride = offset;

	if (desc_optimized.uiTextureVertexOffset != -1)
		desc_optimized.uiTextureVertexStride = offset;

	if (desc_optimized.uiColorOffset != -1)
		desc_optimized.uiColorStride = offset;

	if (desc_optimized.uiNormalOffset != -1)
		desc_optimized.uiNormalStride = offset;

	if (desc_optimized.uiTangentOffset != -1)
		desc_optimized.uiTangentStride = offset;

	if (desc_optimized.uiBinormalOffset != -1)
		desc_optimized.uiBinormalStride = offset;

	if (desc_optimized.pAttribs)
	{
		/* not implemented */
	}

	if (do_ushort_indexes)
	{
		desc_optimized.bIndexBuffer32 = false;

		for (uint i = 0; i < idxs_count; ++i)
			((uint16 *)desc_optimized.pIndexBuffer)[i] = (uint16)((uint *)desc.pIndexBuffer)[i];
	}
	else
		memcpy(desc_optimized.pIndexBuffer, desc.pIndexBuffer, idxs_data_size);

	PARANOIC_CHECK_RES(_pBuffer->Reallocate(desc_optimized, verts_count, idxs_count, CRDM_TRIANGLES));

	delete[] desc.pData;
	delete[] desc_optimized.pData;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::GetOwner(IModel *&prModel)
{
	prModel = _pOwnerModel;
	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::SetOwner(IModel *pModel)
{
	if (_pOwnerModel == pModel)
		return S_OK;

	if (_pOwnerModel)
		_pOwnerModel->RemoveMesh(this);

	_pOwnerModel = pModel;

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::ExecuteCommand(uint uiCmd, TVariant &stVar)
{
	if (uiCmd != 1 || stVar.GetType() != DVT_POINTER)
		return E_INVALIDARG;

	stVar.SetBool(_SaveToFile(reinterpret_cast<IFile *>(stVar.AsPointer())));

	return S_OK;
}

DGLE_RESULT DGLE_API CMesh::ExecuteTextCommand(const char *pcCommand, TVariant &stVar)
{
	stVar.Clear();
	return E_NOTIMPL;
}

DGLE_RESULT DGLE_API CMesh::ExecuteTextCommandEx(const char *pcCommand, char *pcResult, uint &uiCharsCount)
{
	if (!pcCommand)
		return E_INVALIDARG;

	if (!pcResult)
	{
		uiCharsCount = 1;
		return S_OK;
	}

	if (uiCharsCount < 1)
		return E_INVALIDARG;
	else
	{
		if (pcResult && uiCharsCount > 0)
			strcpy(pcResult, "");
		else
			pcResult = NULL;

		uiCharsCount = 0;

		return E_NOTIMPL;
	}
}