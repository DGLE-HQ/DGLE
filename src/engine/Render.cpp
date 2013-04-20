/**
\author		Korotkov Andrey aka DRON
\date		18.04.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Render.h"
#include "Render2D.h"
#include "Render3D.h"
#include "ResourceManager.h"

CRender::CRender(uint uiInstIdx):
CInstancedObj(uiInstIdx)
{
	_pCoreRenderer = Core()->pCoreRenderer();

	Console()->RegComProc("rnd_list_features", "Prints list of features supported by current Core Renderer implementation.", &_s_ConListFeatures, (void*)this);

	bool b_supported;

	_strFeturesList = "Core Renderer supported features\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_BUILTIN_FULLSCREEN_MODE, b_supported);
	_strFeturesList += std::string("Builtin fullscreen mode: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_BUILTIN_STATE_FILTER, b_supported);
	_strFeturesList += std::string("Builtin state filter: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_MULTISAMPLING, b_supported);
	_strFeturesList += std::string("Multisampling: ") + (b_supported ? "Yes" : "No") + "\n\t";
	
	_pCoreRenderer->IsFeatureSupported(CRFT_VSYNC, b_supported);
	_strFeturesList += std::string("Vertical synchronization: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_PROGRAMMABLE_PIPELINE, b_supported);
	_strFeturesList += std::string("Programmable pipeline: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_LEGACY_FIXED_FUNCTION_PIPELINE_API, b_supported);
	_strFeturesList += std::string("Legacy fixed function pipeline API: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_BGRA_DATA_FORMAT, b_supported);
	_strFeturesList += std::string("BGRA textures: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_COMPRESSION, b_supported);
	_strFeturesList += std::string("Texture compression: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_NON_POWER_OF_TWO_TEXTURES, b_supported);
	_strFeturesList += std::string("Non power of two textures: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_DEPTH_TEXTURES, b_supported);
	_strFeturesList += std::string("Depth textures: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_ANISOTROPY, b_supported);
	_strFeturesList += std::string("Texture anisotropy: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_MIPMAP_GENERATION, b_supported);
	_strFeturesList += std::string("Hardware mipmap generation: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_MIRRORED_REPEAT, b_supported);
	_strFeturesList += std::string("Texture mirrored repeat: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_MIRROR_CLAMP, b_supported);
	_strFeturesList += std::string("Texture mirror clamp: ") + (b_supported ? "Yes" : "No") + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_GEOMETRY_BUFFER, b_supported);
	_strFeturesList += std::string("Hardware geometry buffers: ") + (b_supported ? "Yes" : "No") + "\n\t";;
	
	_pCoreRenderer->IsFeatureSupported(CRFT_FRAME_BUFFER, b_supported);
	_strFeturesList += std::string("Hardware frame buffers: ") + (b_supported ? "Yes" : "No");

	// Don't append "\n\t" to the last line!
	assert(_strFeturesList[_strFeturesList.length() - 2] != '\n' && _strFeturesList[_strFeturesList.length() - 1] != '\t');

	LOG(_strFeturesList, LT_INFO);

	for (size_t i = 0; i < _strFeturesList.size(); ++i)
		if (_strFeturesList[i] == '\t')
			_strFeturesList[i] = ' ';

	_strMetricsList = "Core Renderer device metrics\n\t";

	int i_value;

	_pCoreRenderer->GetDeviceMetric(CRMT_MAX_TEXTURE_RESOLUTION, i_value);
	_strMetricsList += "Maximum texture resolution: " + IntToStr(i_value) + "X" + IntToStr(i_value) + "\n\t";

	_pCoreRenderer->IsFeatureSupported(CRFT_TEXTURE_ANISOTROPY, b_supported);
	if (b_supported)
	{
		_pCoreRenderer->GetDeviceMetric(CRMT_MAX_ANISOTROPY_LEVEL, i_value);
		_strMetricsList += "Maximum anisotropy level: " + IntToStr(i_value) + "X\n\t";
	}

	_pCoreRenderer->GetDeviceMetric(CRMT_MAX_TEXTURE_LAYERS, i_value);
	_strMetricsList += "Maximum multitexturing layer: " + IntToStr(i_value);

	// Don't append "\n\t" to the last line!
	assert(_strMetricsList[_strMetricsList.length() - 2] != '\n' && _strMetricsList[_strMetricsList.length() - 1] != '\t');

	LOG(_strMetricsList, LT_INFO);

	for (size_t i = 0; i < _strMetricsList.size(); ++i)
		if (_strMetricsList[i] == '\t')
			_strMetricsList[i] = ' ';

	SetClearColor(ColorClear());

	_pRender2D = new CRender2D(InstIdx());
	_pRender3D = new CRender3D(InstIdx());

	LOG("Render subsystem initialized.", LT_INFO);
}

CRender::~CRender()
{
	delete _pRender2D;
	delete _pRender3D;

	Console()->UnRegCom("rnd_list_features");

	LOG("Render subsystem finalized.", LT_INFO);
}

void CRender::SetDefaultStates()
{
	_pRender3D->SetDefaultStates();
}

void CRender::BeginRender()
{
	SetRenderTarget(NULL);
	_pCoreRenderer->Clear();
}

void CRender::EndRender()
{
	SetRenderTarget(NULL);
	_pCoreRenderer->Present();
}

void CRender::OnResize(uint uiWidth, uint uiHeight)
{
	_pCoreRenderer->SetViewport(0, 0, uiWidth, uiHeight);
	_pRender3D->OnResize(uiWidth, uiHeight);
}

void CRender::RefreshBatchData()
{
	_pRender2D->RefreshBatchData();
	_pRender3D->RefreshBatchData();
}

void CRender::BeginFrame()
{
	_pRender3D->BeginFrame();
	_pRender2D->BeginFrame();
}

void CRender::EndFrame()
{
	_pRender2D->EndFrame();
	_pRender3D->EndFrame();
}

void CRender::DrawProfiler()
{
	_pRender3D->DrawProfiler();
	_pRender2D->DrawProfiler();
}

DGLE_RESULT DGLE_API CRender::SetClearColor(const TColor4 &stColor)
{
	_pCoreRenderer->SetClearColor(stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::GetClearColor(TColor4 &stColor)
{
	_pCoreRenderer->GetClearColor(stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::ClearColorBuffer()
{
	_pCoreRenderer->Clear(true, false, false);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::Unbind(E_ENGINE_OBJECT_TYPE eType)
{
	switch (eType)
	{
	case EOT_UNKNOWN:		
		Unbind(EOT_MATERIAL);
		Unbind(EOT_TEXTURE);
		Unbind(EOT_MESH);
		Unbind(EOT_LIGHT);
		break;

	case EOT_TEXTURE:
		_pRender3D->UnbindTextures();
		break;

	case EOT_MESH:
	case EOT_MODEL:
		_pCoreRenderer->DrawBuffer(NULL);
		break;

	case EOT_LIGHT:
		_pRender3D->UnbindLights();
		break;

	case EOT_MATERIAL:
		_pRender3D->UnbindMaterial();
		break;

	default:
		return E_INVALIDARG;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CRender::EnableScissor(const TRectF &stArea)
{
	TRasterizerStateDesc desc;
	_pCoreRenderer->GetRasterizerState(desc);
	
	desc.bScissorEnabled = true;
	_pCoreRenderer->SetRasterizerState(desc);
	
	_pCoreRenderer->SetScissorRectangle((uint)stArea.x, (uint)stArea.y, (uint)stArea.width, (uint)stArea.height);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::DisableScissor()
{
	TRasterizerStateDesc desc;
	_pCoreRenderer->GetRasterizerState(desc);
	
	desc.bScissorEnabled = false;
	_pCoreRenderer->SetRasterizerState(desc);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::GetScissor(bool &bEnabled, TRectF &stArea)
{
	TRasterizerStateDesc desc;
	_pCoreRenderer->GetRasterizerState(desc);
	bEnabled = desc.bScissorEnabled;
	
	uint x, y, w, h;
	_pCoreRenderer->GetScissorRectangle(x, y, w, h);

	stArea = TRectF((float)x, (float)y, (float)w, (float)h);

	return S_OK;
}

DGLE_RESULT DGLE_API CRender::SetRenderTarget(ITexture *pTargetTex)
{
	ICoreTexture *p_tex = NULL;
	
	if (pTargetTex)
		pTargetTex->GetCoreTexture(p_tex);
	
	_pRender2D->End2D(); // we need to be shure that Begin2D will recalculate matrix on any first 2D call

	return _pCoreRenderer->SetRenderTarget(p_tex);
}

DGLE_RESULT DGLE_API CRender::GetRenderTarget(ITexture *&prTargetTex)
{
	ICoreTexture *p_cur_target_ctex;
	_pCoreRenderer->GetRenderTarget(p_cur_target_ctex);

	if (p_cur_target_ctex == NULL)
	{
		prTargetTex = NULL;
		return S_OK;
	}

	uint count;
	Core()->pResMan()->GetResourcesCount(count);

	for (uint i = 0; i < count; ++i)
	{
		IEngineBaseObject *p_obj;
		Core()->pResMan()->GetResourceByIndex(i, p_obj);
		
		E_ENGINE_OBJECT_TYPE type;
		p_obj->GetType(type);

		if (type == EOT_TEXTURE)
		{
			ICoreTexture *p_ctex;
			((ITexture *)p_obj)->GetCoreTexture(p_ctex);
			
			if (p_ctex == p_cur_target_ctex)
			{
				prTargetTex = (ITexture *)p_obj;
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

DGLE_RESULT DGLE_API CRender::GetRender2D(IRender2D *&prRender2D)
{
	prRender2D = _pRender2D;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::GetRender3D(IRender3D *&prRender3D)
{
	prRender3D = _pRender3D;
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType)
{
	eSubSystemType = ESS_RENDER;
	return S_OK;
}

bool DGLE_API CRender::_s_ConListFeatures(void *pParameter, const char *pcParam)
{
	if (strlen(pcParam) != 0)
	{
		CON(CRender, "No parameters expected.");
		return false;
	}
	else
	{
		CON(CRender, PTHIS(CRender)->_strFeturesList.c_str());
		CON(CRender, PTHIS(CRender)->_strMetricsList.c_str());
		return true;
	}
}