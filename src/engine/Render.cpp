/**
\author		Korotkov Andrey aka DRON
\date		03.03.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Render.h"
#include "Render2D.h"
#include "Render3D.h"

CRender::CRender(uint uiInstIdx):
CInstancedObj(uiInstIdx),
_fFovAngle(60.f), _fZNear(0.25f), _fZFar(1000.f)
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

	//Don't append "\n\t" to the last line!

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
	_uiMaxTexUnits = (uint)i_value;
	_strMetricsList += "Maximum multitexturing layer: " + IntToStr(i_value);

	//Don't append "\n\t" to the last line!

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

void CRender::BeginRender()
{
	_pCoreRenderer->Clear();
}

void CRender::EndRender()
{
	_pCoreRenderer->Present();
}

void CRender::_SetPerspectiveMatrix(uint width, uint height)
{
	const float aspect = (float)width / (float)height,
		top = _fZNear * tanf(_fFovAngle * (float)M_PI / 360.f),
		bottom = -top,
		left = bottom * aspect,
		right = top * aspect;

	_pCoreRenderer->SetMatrix(TMatrix4(
		2.f * _fZNear / (right - left),		0.f,								(right + left) / (right - left),				0.f,
		0.f,								2.f * _fZNear / (top - bottom),		(top + bottom) / (top - bottom),				0.f,
		0.f,								0.f,								-(_fZFar + _fZNear) / (_fZFar - _fZNear),		-1.f,
		0.f,								0.f,								-2.f * _fZFar * _fZNear / (_fZFar - _fZNear),	0.f
		), MT_PROJECTION);
}

void CRender::OnResize(uint uiWidth, uint uiHeight)
{
	_pCoreRenderer->SetViewport(0, 0, uiWidth, uiHeight);						
	_SetPerspectiveMatrix(uiWidth, uiHeight);
}

void CRender::SetPerspective(float fFovAngle, float fZNear, float fZFar)
{
	_fFovAngle = fFovAngle;
	_fZNear = fZNear;
	_fZFar = fZFar;

	uint x, y, width, height;
	_pCoreRenderer->GetViewport(x, y, width, height);
	_SetPerspectiveMatrix(width, height);
}

void CRender::GetPerspective(float &fFovAngle, float &fZNear, float &fZFar) const
{
	fFovAngle = _fFovAngle;
	fZNear = _fZNear;
	fZFar = _fZFar;
}

DGLE_RESULT DGLE_API CRender::SetClearColor(const TColor4 &stColor)
{
	_stClearColor = stColor;
	_pCoreRenderer->SetClearColor(stColor);
	return S_OK;
}

DGLE_RESULT DGLE_API CRender::GetClearColor(TColor4 &stColor)
{
	stColor = _stClearColor;
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
		
		for (uint i = _uiMaxTexUnits; i != 0; --i)
			_pCoreRenderer->BindTexture(NULL, i - 1);		
		
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
	_pCoreRenderer->SetScissor((uint)stArea.x, (uint)stArea.y, (uint)stArea.width, (uint)stArea.height);
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

DGLE_RESULT DGLE_API CRender::SetRenderTarget(ITexture* pTargetTex)
{
	ICoreTexture *p_tex = NULL;
	
	if (pTargetTex)
		pTargetTex->GetCoreTexture(p_tex);
	
	return _pCoreRenderer->SetRenderTarget(p_tex);
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