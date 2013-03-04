/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Model.h"

CModel::CModel(uint uiInstIdx):
CInstancedObj(uiInstIdx), _mat(NULL)
{}

CModel::~CModel()
{
	while (!_meshes.empty())
		(*_meshes.begin())->Free();
}

void CModel::_RecalculateBounds()
{
	TPoint3 min_dem((std::numeric_limits<float>::infinity)(), (std::numeric_limits<float>::infinity)(), (std::numeric_limits<float>::infinity)()),
		max_dem(-(std::numeric_limits<float>::infinity)(), -(std::numeric_limits<float>::infinity)(), -(std::numeric_limits<float>::infinity)());
	
	for (size_t i = 0; i < _meshes.size(); ++i)
	{
		TPoint3 center;
		TVector3 extents;

		_meshes[i]->GetCenter(center);
		_meshes[i]->GetExtents(extents);

		const TPoint3 walls[6] = {
			TPoint3(center.x - extents.x, center.y, center.z), TPoint3(center.x + extents.x, center.y, center.z), TPoint3(center.x, center.y - extents.y, center.z),
			TPoint3(center.x, center.y + extents.y, center.z), TPoint3(center.x, center.y, center.z - extents.z), TPoint3(center.x, center.y, center.z + extents.z)
		};

		for (uint i = 0; i < 6; ++i)
		{
			const TPoint3 &p = walls[i];

			max_dem.x = max(p.x, max_dem.x);
			min_dem.x = min(p.x, min_dem.x);

			max_dem.y = max(p.y, max_dem.y);
			min_dem.y = min(p.y, min_dem.y);

			max_dem.z = max(p.z, max_dem.z);
			min_dem.z = min(p.z, min_dem.z);
		}
	}

	_center = min_dem + (max_dem - min_dem) / 2.f;
	_extents = (max_dem - min_dem) / 2.f;
}

DGLE_RESULT DGLE_API CModel::Draw()
{
	for (size_t i = 0; i < _meshes.size(); ++i)
	{
		if (_materials[i])
			_materials[i]->Bind();
		else
			if (_mat)
				_mat->Bind();

		_meshes[i]->Draw();
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CModel::DrawMesh(uint uiMeshIdx)
{
	if (uiMeshIdx >= _meshes.size())
		return E_INVALIDARG;

	if (_materials[uiMeshIdx])
		_materials[uiMeshIdx]->Bind();
	else
		if (_mat)
			_mat->Bind();

	_meshes[uiMeshIdx]->Draw();

	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetCenter(TPoint3 &stCenter)
{
	stCenter = _center;
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetExtents(TVector3 &stExtents)
{
	stExtents = _extents;
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::MeshesCount(uint &uiCount)
{
	uiCount = _meshes.size();
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetMesh(uint uiMeshIdx, IMesh *&prMesh)
{
	if (uiMeshIdx >= _meshes.size())
		return E_INVALIDARG;

	prMesh = _meshes[uiMeshIdx];

	return S_OK;
}

DGLE_RESULT DGLE_API CModel::SetModelMaterial(IMaterial *pMaterial)
{
	_mat = pMaterial;
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetModelMaterial(IMaterial *&prMaterial)
{
	prMaterial = _mat;
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::SetMeshMaterial(uint uiMeshIdx, IMaterial *pMaterial)
{
	if (uiMeshIdx >= _meshes.size())
		return E_INVALIDARG;

	_materials[uiMeshIdx] = pMaterial;

	return S_OK;
}

DGLE_RESULT DGLE_API CModel::GetMeshMaterial(uint uiMeshIdx, IMaterial *&prMaterial)
{
	if (uiMeshIdx >= _meshes.size())
		return E_INVALIDARG;

	prMaterial = _materials[uiMeshIdx];

	return S_OK;
}

DGLE_RESULT DGLE_API CModel::AddMesh(IMesh *pMesh)
{
	pMesh->SetOwner(this);
	_meshes.push_back(pMesh);
	_materials.push_back(NULL);

	_RecalculateBounds();
	
	return S_OK;
}

DGLE_RESULT DGLE_API CModel::RemoveMesh(IMesh *pMesh)
{
	for (size_t i = 0; i < _meshes.size(); ++i)
		if (pMesh == _meshes[i])
		{
			_meshes.erase(_meshes.begin() + i);
			_materials.erase(_materials.begin() + i);

			return S_OK;
		}

	return S_FALSE;
}

DGLE_RESULT DGLE_API CModel::ReplaceMesh(uint uiMeshIdx, IMesh *pMesh)
{
	if (uiMeshIdx >= _meshes.size())
		return E_INVALIDARG;

	if (_meshes[uiMeshIdx] != pMesh)
	{
		_meshes[uiMeshIdx]->SetOwner(NULL);
		_meshes[uiMeshIdx]->Free();
	}

	pMesh->SetOwner(this);
	_meshes[uiMeshIdx] = pMesh;

	_RecalculateBounds();

	return S_OK;
}