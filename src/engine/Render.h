/**
\author		Korotkov Andrey aka DRON
\date		18.04.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

class CRender2D;
class CRender3D;

class CRender: public CInstancedObj, public IRender
{
	ICoreRenderer *_pCoreRenderer;
	
	std::string _strFeturesList, _strMetricsList;

	CRender2D *_pRender2D;
	CRender3D *_pRender3D;

	static bool DGLE_API _s_ConListFeatures(void *pParameter, const char *pcParam);

public:

	CRender(uint uiInstIdx);
	~CRender();

	inline CRender2D* pRender2D() const { return _pRender2D; }
	inline CRender3D* pRender3D() const { return _pRender3D; }

	void SetDefaultStates();
	void BeginRender();
	void EndRender();
	void BeginFrame();
	void EndFrame();
	void OnResize(uint uiWidth, uint uiHeight);
	void DrawProfiler();
	void RefreshBatchData();

	DGLE_RESULT DGLE_API SetClearColor(const TColor4 &stColor);
	DGLE_RESULT DGLE_API GetClearColor(TColor4 &stColor);
	DGLE_RESULT DGLE_API ClearColorBuffer();
	DGLE_RESULT DGLE_API Unbind(E_ENGINE_OBJECT_TYPE eType);
	DGLE_RESULT DGLE_API EnableScissor(const TRectF &stArea);	
	DGLE_RESULT DGLE_API DisableScissor();
	DGLE_RESULT DGLE_API GetScissor(bool &bEnabled, TRectF &stArea);
	DGLE_RESULT DGLE_API SetRenderTarget(ITexture *pTargetTex);
	DGLE_RESULT DGLE_API GetRenderTarget(ITexture *&prTargetTex);

	DGLE_RESULT DGLE_API GetRender2D(IRender2D *&prRender2D);
	DGLE_RESULT DGLE_API GetRender3D(IRender3D *&prRender3D);

	DGLE_RESULT DGLE_API GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType);

	IDGLE_BASE_IMPLEMENTATION(IRender, INTERFACE_IMPL(IEngineSubSystem, INTERFACE_IMPL_END))
};