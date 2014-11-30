/**
\author		Andrey Korotkov aka DRON
\date		29.11.2014 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "CoreRendererGL.h"
#include "ResourceManager.h"

#ifndef NO_BUILTIN_RENDERER

using namespace std;

//COpenGLBufferContainer

class COpenGLBufferContainer : public IOpenGLBufferContainer
{
	GLuint _verticesVBO, _indexesVBO;
public:

	COpenGLBufferContainer(GLuint verVBO, GLuint idxVBO):_verticesVBO(verVBO), _indexesVBO(idxVBO) {}
	
	~COpenGLBufferContainer()
	{
		if (_verticesVBO != 0)
			glDeleteBuffersARB(1, &_verticesVBO);

		if (_indexesVBO != 0)
			glDeleteBuffersARB(1, &_indexesVBO);
	}

	inline GLuint GetVerticesVBO() const { return _verticesVBO; }
	inline GLuint GetIndexesVBO() const { return _indexesVBO; }

	DGLE_RESULT DGLE_API GetVertexBufferObject(GLuint &vbo)
	{
		vbo = _verticesVBO;
		return S_OK;
	}
	
	DGLE_RESULT DGLE_API GetIndexBufferObject(GLuint &vbo)
	{
		vbo = _indexesVBO;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetObjectType(E_ENGINE_OBJECT_TYPE &eType)
	{
		eType = EOT_MESH;
		return S_OK;
	}

	IDGLE_BASE_IMPLEMENTATION(IOpenGLBufferContainer, INTERFACE_IMPL(IBaseRenderObjectContainer, INTERFACE_IMPL_END))
};

//CCoreGeometryBuffer

class CCoreGeometryBuffer : public ICoreGeometryBuffer
{
	CCoreRendererGL *_pCoreRenderer;
	COpenGLBufferContainer _clGLContainer;
	TDrawDataDesc _stDrawDataDesc;
	uint _uiVerticesDataSize, _uiIndexesDataSize,
		 _uiVerticesCount, _uiIndexesCount;
	E_CORE_RENDERER_DRAW_MODE _eDrawMode;
	E_CORE_RENDERER_BUFFER_TYPE _eBufferType;

public:
	CCoreGeometryBuffer(CCoreRendererGL *pCoreRenderer, GLuint verVBO, GLuint idxVBO, const TDrawDataDesc &drawDesc, uint verDatSize, uint idxDatSize, uint verCnt, uint idxCnt,
		E_CORE_RENDERER_DRAW_MODE mode, E_CORE_RENDERER_BUFFER_TYPE type):
		_pCoreRenderer(pCoreRenderer), _clGLContainer(verVBO, idxVBO),	_stDrawDataDesc(drawDesc), _uiVerticesDataSize(verDatSize),
		_uiIndexesDataSize(idxDatSize), _uiVerticesCount(verCnt), _uiIndexesCount(idxCnt), _eDrawMode(mode), _eBufferType(type)
	{}

	~CCoreGeometryBuffer()
	{
		delete[] _stDrawDataDesc.pData;
		delete[] _stDrawDataDesc.pIndexBuffer;
	}

	inline GLuint GetVerticesVBO() const { return _clGLContainer.GetVerticesVBO(); }
	inline GLuint GetIndexesVBO() const { return _clGLContainer.GetIndexesVBO(); }
	inline E_CORE_RENDERER_DRAW_MODE GetDrawMode() const { return _eDrawMode; }
	inline uint GetVerticesCount() const { return _uiVerticesCount; }
	inline uint GetIndexesCount() const { return _uiIndexesCount; }
	inline const TDrawDataDesc & GetDrawDesc() const { return _stDrawDataDesc; }

	DGLE_RESULT DGLE_API GetGeometryData(TDrawDataDesc &stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize)
	{
		if (uiVerticesDataSize != _uiVerticesDataSize || uiIndexesDataSize != _uiIndexesDataSize)
			return E_INVALIDARG;

		uint8 *p_data = stDesc.pData, *p_idx = stDesc.pIndexBuffer;
		stDesc = _stDrawDataDesc;
		stDesc.pData = p_data;
		stDesc.pIndexBuffer = p_idx;
		
		if (_eBufferType == CRBT_SOFTWARE)
		{
			memcpy(stDesc.pData, _stDrawDataDesc.pData, _uiVerticesDataSize);

			if (_uiIndexesDataSize != 0)
				memcpy(stDesc.pIndexBuffer, _stDrawDataDesc.pIndexBuffer, _uiIndexesDataSize);
		}
		else
		{
			_pCoreRenderer->pStateMan()->glBindBufferARB(GL_ARRAY_BUFFER_ARB, _clGLContainer.GetVerticesVBO());
			glGetBufferSubData(GL_ARRAY_BUFFER_ARB, 0, _uiVerticesDataSize, (GLvoid*)stDesc.pData);

			if (_clGLContainer.GetIndexesVBO() != 0)
			{
				_pCoreRenderer->pStateMan()->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _clGLContainer.GetIndexesVBO());
				glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, _uiIndexesDataSize, (GLvoid*)stDesc.pIndexBuffer);
			}
		}

		return S_OK;
	}

	DGLE_RESULT DGLE_API SetGeometryData(const TDrawDataDesc &stDesc, uint uiVerticesDataSize, uint uiIndexesDataSize)
	{
		if (uiVerticesDataSize != _uiVerticesDataSize || uiIndexesDataSize != _uiIndexesDataSize)
			return E_INVALIDARG;

		return Reallocate(stDesc, _uiVerticesCount, _uiIndexesCount, _eDrawMode);
	}

	DGLE_RESULT DGLE_API Reallocate(const TDrawDataDesc &stDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode)
	{
		const uint vertices_data_size = uiVerticesCount * CCoreRendererGL::GetVertexSize(stDesc),
		 indexes_data_size = uiIndexesCount * (stDesc.bIndexBuffer32 ? sizeof(uint32) : sizeof(uint16)) * (stDesc.pIndexBuffer ? 3 : 0);

		if (_uiIndexesDataSize == 0 && indexes_data_size != 0)
			return E_INVALIDARG;

		_uiVerticesCount = uiVerticesCount;
		_eDrawMode = eMode;

		uint8 *p_data = _stDrawDataDesc.pData, *p_idx = _stDrawDataDesc.pIndexBuffer;
		_stDrawDataDesc = stDesc;
		_stDrawDataDesc.pData = p_data;
		_stDrawDataDesc.pIndexBuffer = p_idx;
		
		if (_eBufferType == CRBT_SOFTWARE)
		{
			if (_uiVerticesDataSize != vertices_data_size)
			{
				_uiVerticesDataSize = vertices_data_size;
				delete[] _stDrawDataDesc.pData;
				_stDrawDataDesc.pData = new uint8[_uiVerticesDataSize];
			}

			memcpy(_stDrawDataDesc.pData, stDesc.pData, _uiVerticesDataSize);

			if (_uiIndexesDataSize != 0)
			{
				_uiIndexesCount = uiIndexesCount;

				if (_uiIndexesDataSize != indexes_data_size)
				{
					_uiIndexesDataSize = indexes_data_size;
					delete[] _stDrawDataDesc.pIndexBuffer;
					_stDrawDataDesc.pIndexBuffer = new uint8[_uiIndexesDataSize];
				}

				memcpy(_stDrawDataDesc.pIndexBuffer, stDesc.pIndexBuffer, _uiIndexesDataSize);
			}
		}
		else
		{
			_uiVerticesDataSize = vertices_data_size;
			_pCoreRenderer->pStateMan()->glBindBufferARB(GL_ARRAY_BUFFER_ARB, _clGLContainer.GetVerticesVBO());
			
			if (_eBufferType == CRBT_HARDWARE_STATIC)
			{
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, _uiVerticesDataSize, NULL, GL_STATIC_DRAW_ARB);
				glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, _uiVerticesDataSize, (GLvoid*)stDesc.pData);
			}
			else
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, _uiVerticesDataSize, (GLvoid*)stDesc.pData, GL_DYNAMIC_DRAW_ARB);

			if (_clGLContainer.GetIndexesVBO() != 0)
			{
				_uiIndexesDataSize = indexes_data_size;
				_uiIndexesCount = uiIndexesCount;

				_pCoreRenderer->pStateMan()->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _clGLContainer.GetIndexesVBO());
				
				if (_eBufferType == CRBT_HARDWARE_STATIC)
				{
					glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _uiIndexesDataSize, NULL, GL_STATIC_DRAW_ARB);
					glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, _uiIndexesDataSize, (GLvoid*)stDesc.pIndexBuffer);
				}
				else
					glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _uiIndexesDataSize, (GLvoid*)stDesc.pIndexBuffer, GL_DYNAMIC_DRAW_ARB);
			}
		}

		return S_OK;
	}

	DGLE_RESULT DGLE_API GetBufferDimensions(uint &uiVerticesDataSize, uint &uiVerticesCount, uint &uiIndexesDataSize, uint &uiIndexesCount)
	{
		uiVerticesDataSize = _uiVerticesDataSize;
		uiVerticesCount = _uiVerticesCount;
		uiIndexesDataSize = _uiIndexesDataSize;
		uiIndexesCount = _uiIndexesCount;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetBufferDrawDataDesc(TDrawDataDesc &stDesc)
	{
		stDesc = _stDrawDataDesc;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetBufferDrawMode(E_CORE_RENDERER_DRAW_MODE &eMode)
	{
		eMode = _eDrawMode;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetBufferType(E_CORE_RENDERER_BUFFER_TYPE &eType)
	{
		eType = _eBufferType;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetBaseObject(IBaseRenderObjectContainer *&prObj)
	{
		prObj = &_clGLContainer;
		return S_OK;
	}

	DGLE_RESULT DGLE_API Free()
	{
		delete this;
		return S_OK;
	}

	IDGLE_BASE_IMPLEMENTATION(ICoreGeometryBuffer, INTERFACE_IMPL_END)
};

//COpenGLTexture
	
class COpenGLTextureContainer : public IOpenGLTextureContainer
{
	GLuint _tex;

public:

	COpenGLTextureContainer(GLuint tex):_tex(tex){}
		
	~COpenGLTextureContainer()
	{
		glDeleteTextures(1, &_tex);
	}

	inline GLuint GetTex() const { return _tex; }

	DGLE_RESULT DGLE_API GetObjectType(E_ENGINE_OBJECT_TYPE &eType)
	{
		eType = EOT_TEXTURE;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetTexture(GLuint &texture)
	{
		texture = _tex;
		return S_OK;
	}

	IDGLE_BASE_IMPLEMENTATION(IOpenGLTextureContainer, INTERFACE_IMPL(IBaseRenderObjectContainer, INTERFACE_IMPL_END))
};

//CCoreTexture

class CCoreTexture : public ICoreTexture
{
	CCoreRendererGL *_pCR;
	COpenGLTextureContainer _clGLContainer;
	E_TEXTURE_TYPE _type;
	uint _w, _h, _d;
	E_TEXTURE_DATA_FORMAT _format;
	E_TEXTURE_LOAD_FLAGS _loadFlags;
	bool _bMipMaps;

	GLenum _GetGLFormat()
	{
		switch (_format)
		{
		case TDF_BGR8:
			return GL_BGR_EXT;

		case TDF_RGB8:
			return GL_RGB;

		case TDF_DEPTH_COMPONENT24:
		case TDF_DEPTH_COMPONENT32:
			return GL_DEPTH_COMPONENT;

		case TDF_BGRA8:
			return GL_BGRA_EXT;

		case TDF_RGBA8:
			return GL_RGBA;

		case TDF_ALPHA8:
			return GL_ALPHA;

		case TDF_DXT1:
			return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

		case TDF_DXT5:
			return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

		default:
			return 0;
		}
	}

	uint _DataSize(uint lod, uint &lw, uint &lh)
	{
		int bytes;

		switch (_format)
		{
		case TDF_BGR8:
		case TDF_RGB8:
		case TDF_DEPTH_COMPONENT24:
			bytes = 3;
			break;

		case TDF_BGRA8: 
		case TDF_RGBA8:
		case TDF_DEPTH_COMPONENT32:
			bytes = 4;
			break;

		case TDF_ALPHA8:
			bytes = 1;
			break;

		case TDF_DXT1:
			bytes = 8;
			break;

		case TDF_DXT5:
			bytes = 16;
			break;

		default:
			return 0;
		}

		uint w = _w, h = _h, l = 0;

		while (l != lod)
		{
			w /= 2; h /= 2;
			++l;
		}

		if (w == 0 || h == 0)
			return 0;

		lw = w; lh = h;

		if (_format == TDF_DXT1 || _format == TDF_DXT5)
			return ((w + 3) / 4) * ((h + 3) / 4) * bytes;
		else
			return h * w * bytes;
	}

public:

	CCoreTexture(CCoreRendererGL *pCR, GLuint tex, E_TEXTURE_TYPE type, uint w, uint h, uint d, E_TEXTURE_DATA_FORMAT format, E_TEXTURE_LOAD_FLAGS loadFlags, bool bMipMaps):
	_pCR(pCR), _clGLContainer(tex), _type(type), _w(w), _h(h), _d(d), _format(format), _loadFlags(loadFlags), _bMipMaps(bMipMaps)
	{}

	inline GLuint GetTex() const { return _clGLContainer.GetTex(); }

	DGLE_RESULT DGLE_API GetSize(uint &width, uint &height)
	{
		width = _w; height = _h;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetDepth(uint &depth)
	{
		depth = _d;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetType(E_TEXTURE_TYPE &eType)
	{
		eType = _type;
		return S_OK;
	}
	
	DGLE_RESULT DGLE_API GetFormat(E_TEXTURE_DATA_FORMAT &eFormat)
	{
		eFormat = _format;
		return S_OK;
	}

	DGLE_RESULT DGLE_API GetLoadFlags(E_TEXTURE_LOAD_FLAGS &eLoadFlags)
	{
		eLoadFlags = _loadFlags;
		return S_OK;
	}
	
	DGLE_RESULT DGLE_API GetPixelData(uint8 *pData, uint &uiDataSize, uint uiLodLevel)
	{
		if (!_bMipMaps && uiLodLevel != 0)
			return E_INVALIDARG;

		uint w, h;
		const uint size = _DataSize(uiLodLevel, w, h);

		if (!pData || size != uiDataSize)
		{
			uiDataSize = size;
			return S_FALSE;
		}

		_pCR->BindTexture(this, 0);

		if (_format == TDF_DXT1 || _format == TDF_DXT5)
			glGetCompressedTexImageARB(GL_TEXTURE_2D, uiLodLevel, pData);
		else
		{
			const bool packed = TDF_RGB8 || TDF_BGR8 || TDF_ALPHA8;
			
			if (packed)
				glPixelStorei(GL_PACK_ALIGNMENT, 1);

			glGetTexImage(GL_TEXTURE_2D, uiLodLevel, _GetGLFormat(), GL_UNSIGNED_BYTE, (GLvoid *)pData);

			if (packed)
				glPixelStorei(GL_PACK_ALIGNMENT, 4);
		}

		return S_OK;
	}

	DGLE_RESULT DGLE_API SetPixelData(const uint8 *pData, uint uiDataSize, uint uiLodLevel)
	{
		if (!_bMipMaps && uiLodLevel != 0)
			return E_INVALIDARG;

		uint w, h;
		if (_DataSize(uiLodLevel, w, h) != uiDataSize)
			return E_INVALIDARG;

		_pCR->BindTexture(this, 0);

		if (_format == TDF_DXT1 || _format == TDF_DXT5)
			glCompressedTexSubImage2DARB(GL_TEXTURE_2D, uiLodLevel, 0, 0, w, h, _GetGLFormat(), uiDataSize, (GLvoid*)pData);
		else
		{
			const bool packed = TDF_RGB8 || TDF_BGR8 || TDF_ALPHA8;
			
			if (packed)
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexSubImage2D(GL_TEXTURE_2D, uiLodLevel, 0, 0, w, h, _GetGLFormat(), GL_UNSIGNED_BYTE, (GLvoid*)pData);
			
			if (packed)
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}

		return S_OK;
	}

	DGLE_RESULT DGLE_API Reallocate(const uint8 *pData, uint uiWidth, uint uiHeight, E_TEXTURE_DATA_FORMAT eDataFormat)
	{
		if (eDataFormat != _format)
			return E_INVALIDARG;

		_w = uiWidth; _h = uiHeight;

		_pCR->BindTexture(this, 0);

		if (_bMipMaps && (!GLEW_ARB_framebuffer_object && GLEW_SGIS_generate_mipmap))
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		DGLE_RESULT ret = SetPixelData(pData, _DataSize(0, _w, _h), 0);

		if (_bMipMaps)
		{
			if (GLEW_ARB_framebuffer_object)
				glGenerateMipmap(GL_TEXTURE_2D);
			else
				if (GLEW_SGIS_generate_mipmap)
					glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
		}

		if (FAILED(ret))
			return E_ABORT;
		else
			if (_bMipMaps && (!GLEW_ARB_framebuffer_object && !GLEW_SGIS_generate_mipmap))
				return S_FALSE;
			else
				return S_OK;
	}

	DGLE_RESULT DGLE_API GetBaseObject(IBaseRenderObjectContainer *&prObj)
	{
		prObj = &_clGLContainer;
		return S_OK;
	}

	DGLE_RESULT DGLE_API Free()
	{
		delete this;
		return S_OK;
	}

	IDGLE_BASE_IMPLEMENTATION(ICoreTexture, INTERFACE_IMPL_END)
};

//CCoreRendererGL

CCoreRendererGL::CCoreRendererGL(uint uiInstIdx):
_stInitResults(false),
CBaseRendererGL(uiInstIdx), _clPassThroughStateMan(uiInstIdx), _pCachedStateMan(NULL), _iProfilerState(0),
_bStateFilterEnabled(true), _uiUnfilteredDrawSetups(0), _uiOverallDrawSetups(0), _uiOverallDrawCalls(0),
_uiOverallTrianglesCount(0), _pCurBindedBuffer(NULL), _pLastDrawnBuffer(NULL),
_bVerticesBufferBindedFlag(false), _bIndexesBufferBindedFlag(false),
_pCurRenderTarget(NULL), _uiCurFrameBufferIdx(-1), _uiDrawCount(-1)
{}

inline uint CCoreRendererGL::GetVertexSize(const TDrawDataDesc &stDesc)
{
	uint res = sizeof(float) * ((stDesc.bVertices2D ? 2 : 3) + (stDesc.uiNormalOffset != -1 ? 3 : 0) + (stDesc.uiTextureVertexOffset != -1 ? 2 : 0) +
			(stDesc.uiColorOffset != -1 ? 4 : 0) + (stDesc.uiTangentOffset != -1 ? 3 : 0) + (stDesc.uiBinormalOffset != -1 ? 3 : 0));

	if (stDesc.pAttribs)
	{
		/* not implemented */
	}

	return res;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Prepare(TCrRndrInitResults &stResults)
{
	return stResults = CBaseRendererGL::Prepare(), stResults ? S_OK : E_FAIL;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Initialize(TCrRndrInitResults &stResults)
{
	if (_stInitResults)
		return E_ABORT;

	LOG("Initializing Core Renderer...", LT_INFO);

	stResults = CBaseRendererGL::Initialize();

	if (!stResults)
		return E_ABORT;

	GLenum glew_res = glewInit();

	if (glew_res != GLEW_OK)
	{
		string cause = "";
		
		switch(glew_res)
		{
			case GLEW_ERROR_NO_GL_VERSION:
				cause = "Reason: Can't get OpenGL version.";
				break;
			case GLEW_ERROR_GL_VERSION_10_ONLY:
				cause = "Reason: OpenGL 1.1 or higher required.";
				break;
			case GLEW_ERROR_GLX_VERSION_11_ONLY:
				cause = "Reason: OpenGL 1.2 or higher required.";
				break;
		}

		LOG("Can't initialize OpenGL Extension Wrangler. " + cause, LT_FATAL);
		
		CBaseRendererGL::Finalize();

		return E_ABORT;
	}
	
	_strOpenGLExtensions = string((char*)glGetString(GL_EXTENSIONS));

	string gl_ver((char*)glGetString(GL_VERSION));

	if (GLEW_VERSION_2_1) gl_ver = "2.1";

	LOG(string("OpenGL ") + gl_ver + " on " + (char*)glGetString(GL_RENDERER) + " (" + (char*)glGetString(GL_VENDOR) + ")", LT_INFO);

#ifdef PLATFORM_WINDOWS
	if (string((char*)glGetString(GL_RENDERER)).find("GDI") != string::npos && string((char*)glGetString(GL_VENDOR)).find("Microsoft") != string::npos && !GLEW_VERSION_1_2)
	{
		LOG("Non hardware accelerated OpenGL implementation found! Videocard drivers are missing, corrupted or outdated.", LT_FATAL);
		return E_FAIL;
	}
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_iMaxTexResolution);

	if (GLEW_EXT_texture_filter_anisotropic) 
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_iMaxAnisotropy);

	if (GLEW_EXT_framebuffer_multisample)
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &_iMaxFBOSamples);
	else
		_iMaxFBOSamples = 1;

	_bIsGLSLSupported = false; /*TEMP*/ //GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_ARB_shading_language_100;

	if (GLEW_ARB_multitexture) 
	{
		if (_bIsGLSLSupported)
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &_iMaxTexUnits);
		else
			glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &_iMaxTexUnits);
	}
	else
		_iMaxTexUnits = 1;
	
	_pStateMan = _pCachedStateMan = new CStateManager<true>(InstIdx(), _iMaxTexUnits);

	_pStateMan->glEnable(GL_TEXTURE_2D); // Let it be always enabled for the zero texturing layer.

	if (!_bIsGLSLSupported)
	{
		glEnable(GL_NORMALIZE);
		_pFFP = new CFixedFunctionPipeline();
	}
	else
		_pFFP = NULL;

	SetBlendState(TBlendStateDesc());
	SetDepthStencilState(TDepthStencilDesc());
	SetRasterizerState(TRasterizerStateDesc());

	SetMatrix(MatrixIdentity(), MT_TEXTURE);
	SetMatrix(MatrixIdentity(), MT_PROJECTION);
	SetMatrix(MatrixIdentity(), MT_MODELVIEW);

	Core()->AddEventListener(ET_ON_PROFILER_DRAW, _s_EventsHandler, this);
	Core()->AddEventListener(ET_ON_PER_SECOND_TIMER, _s_EventsHandler, this);

	Console()->RegComProc("crgl_print_exts_list", "Reports extensions supported by current OpenGL implementation.\nw - write to logfile.", &_s_ConPrintGLExts, (void*)this);
	Console()->RegComVar("crgl_profiler", "Displays Core Renderer OpenGL subsystems profiler.", &_iProfilerState, 0, 2);

	_stInitResults = stResults;

	LOG("Core Renderer initialized.", LT_INFO);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Finalize()
{
	if (!_stInitResults)
		return E_ABORT;

	Console()->UnRegCom("crgl_print_exts_list");
	Console()->UnRegCom("crgl_profiler");

	Core()->RemoveEventListener(ET_ON_PROFILER_DRAW, _s_EventsHandler, this);
	Core()->RemoveEventListener(ET_ON_PER_SECOND_TIMER, _s_EventsHandler, this);

	if (GLEW_EXT_framebuffer_object)
	{
		for (size_t i = 0; i < _vecFrameBuffers.size(); ++i)
		{
			glDeleteFramebuffersEXT(1, &_vecFrameBuffers[i].uiFBObject);
			
			if (_vecFrameBuffers[i].uiFBBlitObject != 0)
				glDeleteFramebuffersEXT(1, &_vecFrameBuffers[i].uiFBBlitObject);
			
			if (_vecFrameBuffers[i].uiRBObjectColor != 0)
				glDeleteRenderbuffersEXT(1, &_vecFrameBuffers[i].uiRBObjectColor);
			
			glDeleteRenderbuffersEXT(1, &_vecFrameBuffers[i].uiRBObjectDepthStencil);
		}

		_vecFrameBuffers.clear();
	}

	delete _pFFP;
	delete _pCachedStateMan;

	DGLE_RESULT ret = CBaseRendererGL::Finalize() ? S_OK : E_FAIL;

	LOG("Core Renderer finalized.", LT_INFO);

	return ret;
}

DGLE_RESULT DGLE_API CCoreRendererGL::AdjustMode(TEngineWindow &stNewWin)
{
	return CBaseRendererGL::AdjustMode(stNewWin) ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CCoreRendererGL::MakeCurrent()
{
	return CBaseRendererGL::MakeCurrent() ? S_OK : E_ABORT;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetClearColor(const TColor4 &stColor)
{
	_pStateMan->glClearColor(stColor.r, stColor.g, stColor.b, stColor.a);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetClearColor(TColor4 &stColor)
{
	GLfloat color[4];
	_pStateMan->glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	stColor = color;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Clear(bool bColor, bool bDepth, bool bStencil)
{
	GLbitfield mask = 0;

	if (bColor)	mask |= GL_COLOR_BUFFER_BIT;
	if (bDepth)	mask |= GL_DEPTH_BUFFER_BIT;
	if (bStencil) mask |= GL_STENCIL_BUFFER_BIT;

	glClear(mask);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetViewport(uint x, uint y, uint width, uint height)
{
	_pStateMan->glViewport(x, y, width, height);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetViewport(uint &x, uint &y, uint &width, uint &height)
{
	GLint vp[4];
	_pStateMan->glGetIntegerv(GL_VIEWPORT, vp);
	x = vp[0]; y = vp[1];
	width = vp[2]; height = vp[3];

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetScissorRectangle(uint x, uint y, uint width, uint height)
{
	GLint vp[4];
	_pStateMan->glGetIntegerv(GL_VIEWPORT, vp);
	
	_pStateMan->glScissor(vp[0] + x, (int)(vp[3] - y - height), width, height);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetScissorRectangle(uint &x, uint &y, uint &width, uint &height)
{
	GLint vp[4], scissor_box[4];
	_pStateMan->glGetIntegerv(GL_VIEWPORT, vp);
	_pStateMan->glGetIntegerv(GL_SCISSOR_BOX, scissor_box);

	x = scissor_box[0] - vp[0];
	y = vp[3] - scissor_box[1] - scissor_box[3];
	width = scissor_box[2];
	height = scissor_box[3];

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetLineWidth(float fWidth)
{
	_pStateMan->glLineWidth(fWidth);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetLineWidth(float &fWidth)
{
	GLfloat value;
	_pStateMan->glGetFloatv(GL_LINE_WIDTH, &value);
	fWidth = value;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetPointSize(float fSize)
{
	_pStateMan->glPointSize(fSize);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetPointSize(float &fSize)
{
	GLfloat value;
	_pStateMan->glGetFloatv(GL_POINT_SIZE, &value);
	fSize = value;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ReadFrameBuffer(uint uiX, uint uiY, uint uiWidth, uint uiHeight, uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat)
{
	GLenum format;
	uint bytes;

	switch (eDataFormat)
	{
	case TDF_RGB8: format = GL_RGB; bytes = 3; break;
	case TDF_RGBA8: format = GL_RGBA; bytes = 4; break;
	case TDF_ALPHA8: format = GL_ALPHA; bytes = 1; break;
	case TDF_BGR8: 
		if (!GLEW_EXT_bgra)
			return E_INVALIDARG;
		format = GL_BGR;  bytes = 3;
		break;
	case TDF_BGRA8: 
		if (!GLEW_EXT_bgra)
			return E_INVALIDARG;		
		format = GL_BGRA; bytes = 4;
		break;
	case TDF_DEPTH_COMPONENT32: format = GL_DEPTH_COMPONENT; bytes = 4; break;
	
	default:
		return E_INVALIDARG;
	}

	if (uiDataSize < uiWidth * uiHeight * bytes)
		return E_INVALIDARG;
	
	GLint vp[4];
	_pStateMan->glGetIntegerv(GL_VIEWPORT, vp);
	
	glReadPixels(uiX, vp[3] - uiY, uiWidth, uiHeight, format, eDataFormat == TDF_DEPTH_COMPONENT32 ? GL_FLOAT : GL_UNSIGNED_BYTE, pData);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetRenderTarget(ICoreTexture *pTexture)
{
	if (pTexture != NULL && _pCurRenderTarget != NULL)
		SetRenderTarget(NULL);

	if (pTexture == NULL && _pCurRenderTarget != NULL) // swap image to texture
	{
		E_TEXTURE_DATA_FORMAT fmt;
		_pCurRenderTarget->GetFormat(fmt);
		const bool depth_texture = fmt == TDF_DEPTH_COMPONENT24 || fmt == TDF_DEPTH_COMPONENT32;

		uint width, height;
		_pCurRenderTarget->GetSize(width, height);

		if ((GLEW_EXT_framebuffer_object && GL_EXT_packed_depth_stencil) && _uiCurFrameBufferIdx != -1 && _vecFrameBuffers[_uiCurFrameBufferIdx].bValid)
		{
			if (GLEW_EXT_framebuffer_multisample && _vecFrameBuffers[_uiCurFrameBufferIdx].uiFBBlitObject != 0)
			{
				glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, _vecFrameBuffers[_uiCurFrameBufferIdx].uiFBObject);
				glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, _vecFrameBuffers[_uiCurFrameBufferIdx].uiFBBlitObject);
				
				if (depth_texture)
					glBlitFramebufferEXT(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				else
					glBlitFramebufferEXT(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}

			if (depth_texture)
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
			else
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		else
		{
			GLenum color_format;

			switch (fmt)
			{
			case TDF_RGB8: color_format = GL_RGB; break;
			case TDF_RGBA8: color_format = GL_RGBA; break;
			case TDF_ALPHA8: color_format = GL_ALPHA; break;
			case TDF_DEPTH_COMPONENT24:
			case TDF_DEPTH_COMPONENT32:
				color_format = GL_DEPTH_COMPONENT; break;
			default:
				return E_INVALIDARG;
			}

			BindTexture(_pCurRenderTarget, 0);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, color_format, 0, 0, width, height, 0);
		}

		SetViewport(uiPrevVpX, uiPrevVpY, uiPrevVpWidth, uiPrevVpHeight);

		if (depth_texture)
		{
			_pStateMan->glActiveTextureARB(GL_TEXTURE0_ARB);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			if (!GLEW_EXT_framebuffer_object)
				glClear(GL_DEPTH_BUFFER_BIT);
		}
		else
			if (!GLEW_EXT_framebuffer_object)
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_pCurRenderTarget = NULL;
	}
	else
		if (pTexture != NULL && _pCurRenderTarget == NULL) // prepare for rendering to texture
		{
			E_TEXTURE_DATA_FORMAT fmt;
			pTexture->GetFormat(fmt);
			const bool depth_texture = fmt == TDF_DEPTH_COMPONENT24 || fmt == TDF_DEPTH_COMPONENT32;

			uint depth;
			pTexture->GetDepth(depth);

			if ((fmt != TDF_RGB8 && fmt != TDF_RGBA8 && fmt != TDF_ALPHA8 && fmt != TDF_DEPTH_COMPONENT24 && fmt != TDF_DEPTH_COMPONENT32) || depth != 0)
				return E_INVALIDARG;

			uint width, height;
			pTexture->GetSize(width, height);

			if (GLEW_EXT_framebuffer_object && (GL_EXT_packed_depth_stencil || fmt == TDF_DEPTH_COMPONENT32))
			{		
				const GLuint tex_id = ((CCoreTexture*)pTexture)->GetTex();

				uint fbo_id = -1;

				for (size_t i = 0; i < _vecFrameBuffers.size(); ++i)
					if (_vecFrameBuffers[i].uiWidth == width && _vecFrameBuffers[i].uiHeight == height &&
						((!depth_texture && _vecFrameBuffers[i].uiRBObjectColor != 0) ||
						(depth_texture && _vecFrameBuffers[i].uiRBObjectColor == 0 &&
						((fmt == TDF_DEPTH_COMPONENT24 && _vecFrameBuffers[i].isDepthStencil) || (fmt == TDF_DEPTH_COMPONENT32 && !_vecFrameBuffers[i].isDepthStencil)))))
					{
						fbo_id = i;
						break;
					}

				if (fbo_id == -1) // create FBO if necessary
				{
					TFrameBuffer fbo;

					fbo.uiWidth = width;
					fbo.uiHeight = height;

					const TEngineWindow *wnd = Core()->EngWindow();

					const bool do_multisample = GLEW_EXT_framebuffer_multisample && wnd->eMultisampling != MM_NONE && (int)wnd->eMultisampling * 2 <= _iMaxFBOSamples;

					glGenFramebuffersEXT(1, &fbo.uiFBObject);
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo.uiFBObject);

					if (!depth_texture)
					{
						glGenRenderbuffersEXT(1, &fbo.uiRBObjectColor);
						glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo.uiRBObjectColor);

						if (do_multisample)
							glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, (int)wnd->eMultisampling * 2, GL_RGBA8, fbo.uiWidth, fbo.uiHeight);				
						else
							glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, fbo.uiWidth, fbo.uiHeight);
					}
					else
						fbo.uiRBObjectColor = 0;

					glGenRenderbuffersEXT(1, &fbo.uiRBObjectDepthStencil);
					glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo.uiRBObjectDepthStencil);
					
					GLenum rb_format = GL_DEPTH_COMPONENT24_ARB;
					fbo.isDepthStencil = false;

					if (fmt == TDF_DEPTH_COMPONENT32)
						rb_format = GL_DEPTH_COMPONENT32_ARB;
					else
						if (GL_EXT_packed_depth_stencil)
						{
							rb_format = GL_DEPTH24_STENCIL8_EXT;
							fbo.isDepthStencil = true;
						}

					if (do_multisample)
						glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, (int)wnd->eMultisampling * 2, rb_format, fbo.uiWidth, fbo.uiHeight);					
					else
						glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, rb_format, fbo.uiWidth, fbo.uiHeight);

					if (!depth_texture)
						glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, fbo.uiRBObjectColor);					
					
					glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo.uiRBObjectDepthStencil);
					
					if (rb_format == GL_DEPTH24_STENCIL8_EXT)
						glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo.uiRBObjectDepthStencil);

					glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

					fbo.bValid = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT;

					if (fbo.bValid)
					{
						if( do_multisample)
						{
							glGenFramebuffersEXT(1, &fbo.uiFBBlitObject);
							glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo.uiFBBlitObject);
						}
						else
							fbo.uiFBBlitObject = 0;

						if (depth_texture)
							glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, tex_id, 0);
						else
							glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_id, 0);

						fbo.bValid = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT;
					}

					if (!fbo.bValid)
					{
						_uiCurFrameBufferIdx = -1;
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					}
					else
					{
						_uiCurFrameBufferIdx = _vecFrameBuffers.size();
						
						if (do_multisample)
							glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo.uiFBObject);
					}

					fbo.uiIdleTime = 0;
					_vecFrameBuffers.push_back(fbo);
				}
				else // use already created FBO
					if (_vecFrameBuffers[fbo_id].bValid)
					{
						_uiCurFrameBufferIdx = fbo_id;

						_vecFrameBuffers[fbo_id].uiIdleTime = 0;

						const bool do_multisample = GLEW_EXT_framebuffer_multisample && _vecFrameBuffers[fbo_id].uiFBBlitObject != 0;

						if (do_multisample)
							glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _vecFrameBuffers[fbo_id].uiFBBlitObject);
						else
							glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _vecFrameBuffers[fbo_id].uiFBObject);

						if (depth_texture)
							glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, tex_id, 0);
						else
							glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_id, 0);
					
						if (do_multisample)
							glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _vecFrameBuffers[fbo_id].uiFBObject);
					}
					else
						_uiCurFrameBufferIdx = -1;
			}

			GetViewport(uiPrevVpX, uiPrevVpY, uiPrevVpWidth, uiPrevVpHeight);
			SetViewport(0, 0, width, height);

			if (depth_texture)
			{
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			else
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			_pCurRenderTarget = pTexture;
		}
		else
			return S_FALSE;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetRenderTarget(ICoreTexture *&prTexture)
{
	prTexture = _pCurRenderTarget;
	return S_OK;
}

void CCoreRendererGL::_KillDeadFBOs()
{
	size_t i = 0;
	while (i < _vecFrameBuffers.size())
	{
		++_vecFrameBuffers[i].uiIdleTime;

		if (_vecFrameBuffers.size() > _sc_uiFBOMaxCacheSize && _vecFrameBuffers[i].uiIdleTime > _sc_uiMaxFBOIdleTime)
		{
			glDeleteFramebuffersEXT(1, &_vecFrameBuffers[i].uiFBObject);
			
			if(_vecFrameBuffers[i].uiFBBlitObject != 0)
				glDeleteFramebuffersEXT(1, &_vecFrameBuffers[i].uiFBBlitObject);
			
			if (_vecFrameBuffers[i].uiRBObjectColor != 0)
				glDeleteRenderbuffersEXT(1, &_vecFrameBuffers[i].uiRBObjectColor);
			
			glDeleteRenderbuffersEXT(1, &_vecFrameBuffers[i].uiRBObjectDepthStencil);
			
			_vecFrameBuffers.erase(_vecFrameBuffers.begin() + i);
		}
		else
			++i;
	}
}

DGLE_RESULT DGLE_API CCoreRendererGL::CreateTexture(ICoreTexture *&prTex, const uint8 * const pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags)
{
	const bool b_non_power_of_two = uiWidth != 1 << (int)floor( ( logf( (float)uiWidth ) / logf(2.f) ) + 0.5f ) || uiHeight != 1 << (int)floor( ( logf( (float)uiHeight ) / logf(2.f) ) + 0.5f );

	if (
		((eDataFormat == TDF_BGR8 || eDataFormat == TDF_BGRA8) && !GLEW_EXT_bgra) || 
		((eDataFormat == TDF_DXT1 || eDataFormat == TDF_DXT5) && !GLEW_ARB_texture_compression) ||
		((eDataFormat == TDF_DEPTH_COMPONENT24 || eDataFormat == TDF_DEPTH_COMPONENT32) && !GLEW_ARB_depth_texture) ||
		((int)uiWidth > _iMaxTexResolution || (int)uiHeight > _iMaxTexResolution) ||
		(!GLEW_ARB_texture_non_power_of_two && b_non_power_of_two)
		)
		return E_INVALIDARG;

	DGLE_RESULT ret = S_OK;

	GLenum tex_format = 0, tex_internal_format = 0;
	uint bytes_per_pixel = 0;

	switch (eDataFormat)
	{
	case TDF_RGB8:
		tex_format = GL_RGB;
		tex_internal_format= GL_RGB8;
		bytes_per_pixel = 3;
		break;
	case TDF_RGBA8:
		tex_format = GL_RGBA;
		tex_internal_format= GL_RGBA8;
		bytes_per_pixel = 4;
		break;
	case TDF_ALPHA8:
		tex_format = GL_ALPHA;
		tex_internal_format= GL_ALPHA8;
		bytes_per_pixel = 1;
		break;
	case TDF_BGR8: 
		tex_format = GL_BGR;
		tex_internal_format= GL_RGB8;
		bytes_per_pixel = 3;
		break;
	case TDF_BGRA8: 
		tex_format = GL_BGRA;
		tex_internal_format = GL_RGBA8;
		bytes_per_pixel = 4;
		break;
	case TDF_DXT1:
		tex_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		bytes_per_pixel = 8; // per block
		break;
	case TDF_DXT5:
		tex_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		bytes_per_pixel = 16; // per block
		break;
	case TDF_DEPTH_COMPONENT24:
		tex_format = GL_DEPTH_COMPONENT;
		tex_internal_format= GL_DEPTH_COMPONENT24_ARB;
		bytes_per_pixel = 3;
		break;
	case TDF_DEPTH_COMPONENT32:
		tex_format = GL_DEPTH_COMPONENT;
		tex_internal_format= GL_DEPTH_COMPONENT32_ARB;
		bytes_per_pixel = 4;
		break;
	default : return E_INVALIDARG;
	}

	GLuint new_tex;
	glGenTextures(1, &new_tex);
	
	_pStateMan->glBindTexture(GL_TEXTURE_2D, new_tex);

	GLint required_anisotropy = 4;

	if (eLoadFlags & TLF_FILTERING_ANISOTROPIC)
	{
		if (GLEW_EXT_texture_filter_anisotropic)
		{
			if (eLoadFlags & TLF_ANISOTROPY_2X)
				required_anisotropy = 2;
			else
				if (eLoadFlags & TLF_ANISOTROPY_4X)
					required_anisotropy = 4;
				else
					if (eLoadFlags & TLF_ANISOTROPY_8X)
						required_anisotropy = 8;
					else
						if (eLoadFlags & TLF_ANISOTROPY_16X)
							required_anisotropy = 16;

			if (required_anisotropy > _iMaxAnisotropy)
				required_anisotropy = _iMaxAnisotropy;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, required_anisotropy);
		}
		else
		{
			(int &)eLoadFlags &= ~TLF_FILTERING_ANISOTROPIC;
			(int &)eLoadFlags |= TLF_FILTERING_TRILINEAR;
			
			(int &)eLoadFlags &= ~TLF_ANISOTROPY_2X;
			(int &)eLoadFlags &= ~TLF_ANISOTROPY_4X;
			(int &)eLoadFlags &= ~TLF_ANISOTROPY_8X;
			(int &)eLoadFlags &= ~TLF_ANISOTROPY_16X;

			ret = S_FALSE;
		}
	}

	if (eLoadFlags & TLF_FILTERING_TRILINEAR && !(eLoadFlags & TLF_GENERATE_MIPMAPS) && !bMipmapsPresented)
		(int &)eLoadFlags |= TLF_GENERATE_MIPMAPS;

	const bool b_is_compressed = eDataFormat == TDF_DXT1 || eDataFormat == TDF_DXT5;

	if (eLoadFlags & TLF_COMPRESS)
	{
		if (!GLEW_ARB_texture_compression)
			ret = S_FALSE;
		else
		{
			switch(tex_format)
			{
			case GL_BGR:
			case GL_RGB:
				tex_internal_format = GL_COMPRESSED_RGB_ARB;
				eDataFormat = TDF_DXT1;
				break;
			case GL_BGRA:
			case GL_RGBA:
				tex_internal_format = GL_COMPRESSED_RGBA_ARB;
				eDataFormat = TDF_DXT5;
				break;
			case GL_ALPHA:
				tex_internal_format = GL_COMPRESSED_ALPHA_ARB;
				eDataFormat = TDF_ALPHA8;
				break;
			default:
				ret = S_FALSE;
			}
		}
	}

	if (eDataAlignment == CRDA_ALIGNED_BY_1)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (bMipmapsPresented)
	{
		int i_mipmaps = 0, max_side = max(uiWidth, uiHeight) / 2;

		// This loop is more correct because of NPOT textures than calculation like this: log(max(uiWidth, uiHeight)) / log(2)
		while (max_side > 0)
		{
			max_side /= 2;
			++i_mipmaps;
		}

		int i_cur_w = uiWidth * 2, i_cur_h = uiHeight * 2;
		uint dat_offset = 0, cur_align = 0, data_size;
		
		uint8 *p_cur_data;

		int i_start_level = 0;
		
		if (i_mipmaps > 3 && (eLoadFlags & TLF_DECREASE_QUALITY_MEDIUM || eLoadFlags & TLF_DECREASE_QUALITY_HIGH))
		{
			if (eLoadFlags & TLF_DECREASE_QUALITY_MEDIUM)
				i_start_level = 1;
			else
				i_start_level = 2;

			for (int l = 0; l < i_start_level; ++l)
			{
				i_cur_w /= 2;
				i_cur_h /= 2;

				if (eDataAlignment == CRDA_ALIGNED_BY_4)
					cur_align = GetDataAlignmentIncrement((uint)i_cur_w, bytes_per_pixel, 4);

				if (b_is_compressed)
					dat_offset += ((i_cur_w + 3) / 4) * ((i_cur_h + 3) / 4) * bytes_per_pixel;
				else
					dat_offset += i_cur_h * (i_cur_w * bytes_per_pixel + cur_align);
			}
		}

		for (int i = i_start_level; i <= i_mipmaps; ++i)
		{
			i_cur_w /= 2;
			if (i_cur_w == 0) i_cur_w = 1;
			
			i_cur_h /= 2;
			if (i_cur_h == 0) i_cur_h = 1;

			if (eDataAlignment == CRDA_ALIGNED_BY_4)
				cur_align = GetDataAlignmentIncrement((uint)i_cur_w, bytes_per_pixel, 4);
			else
				cur_align = 0;

			if(b_is_compressed)
				data_size = ((i_cur_w + 3) / 4) * ((i_cur_h + 3) / 4) * bytes_per_pixel;
			else
				data_size = i_cur_h * (i_cur_w * bytes_per_pixel + cur_align);
			
			p_cur_data = const_cast<uint8 *>(&pData[dat_offset]);

			if (b_is_compressed)
				glCompressedTexImage2D(GL_TEXTURE_2D, i - i_start_level, tex_format, i_cur_w, i_cur_h, 0, data_size, (void*)p_cur_data);
			else
				glTexImage2D(GL_TEXTURE_2D, i - i_start_level, tex_internal_format, i_cur_w, i_cur_h, 0, tex_format, GL_UNSIGNED_BYTE, (void*)p_cur_data);

			dat_offset += data_size;
		}

		if (GLEW_VERSION_1_2)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i_mipmaps - i_start_level);
		}
	}
	else
	{
		if ((eLoadFlags & TLF_GENERATE_MIPMAPS) && (b_is_compressed || (!GLEW_SGIS_generate_mipmap && !GLEW_ARB_framebuffer_object)))
		{
			(int &)eLoadFlags &= ~TLF_GENERATE_MIPMAPS;
			ret = S_FALSE;
		}

		if (eLoadFlags & TLF_GENERATE_MIPMAPS && (!GLEW_ARB_framebuffer_object && GLEW_SGIS_generate_mipmap))
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		if (b_is_compressed)
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, tex_format, uiWidth, uiHeight, 0, ((uiWidth + 3) / 4) * ((uiHeight + 3) / 4) * bytes_per_pixel, (void *)pData);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, tex_internal_format, uiWidth, uiHeight, 0, tex_format, GL_UNSIGNED_BYTE, (void*)pData);

		if (eLoadFlags & TLF_GENERATE_MIPMAPS)
		{
			if (GLEW_ARB_framebuffer_object)
				glGenerateMipmap(GL_TEXTURE_2D);
			else
				if (GLEW_SGIS_generate_mipmap)
					glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
		}
	}

	if (bMipmapsPresented || eLoadFlags & TLF_GENERATE_MIPMAPS)
	{
		if (eLoadFlags & TLF_FILTERING_NONE)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else
			if (eLoadFlags & TLF_FILTERING_BILINEAR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		if (eLoadFlags & TLF_FILTERING_NONE)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if (eLoadFlags & TLF_FILTERING_NONE)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	if ((eLoadFlags & TLF_COORDS_MIRROR_REPEAT && GLEW_VERSION_1_4) || (eLoadFlags & TLF_COORDS_MIRROR_CLAMP && !GLEW_EXT_texture_mirror_clamp))
		ret = S_FALSE;

	if (eLoadFlags & TLF_COORDS_CLAMP)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
		if (eLoadFlags & TLF_COORDS_MIRROR_REPEAT && GLEW_VERSION_1_4)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		else
			if (eLoadFlags & TLF_COORDS_MIRROR_CLAMP && GLEW_EXT_texture_mirror_clamp)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_EXT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_EXT);
			}
			else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}

	_pStateMan->glBindTexture(GL_TEXTURE_2D, 0);

	if (eDataAlignment == CRDA_ALIGNED_BY_1)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	prTex = new CCoreTexture(this, new_tex, TT_2D, uiWidth, uiHeight, 0, eDataFormat, eLoadFlags, eLoadFlags & TLF_GENERATE_MIPMAPS || bMipmapsPresented);

	return ret;
}

DGLE_RESULT DGLE_API CCoreRendererGL::CreateGeometryBuffer(ICoreGeometryBuffer *&prBuffer, const TDrawDataDesc &stDrawDesc, uint uiVerticesCount, uint uiIndexesCount, E_CORE_RENDERER_DRAW_MODE eMode, E_CORE_RENDERER_BUFFER_TYPE eType)
{
	if (!stDrawDesc.pData || uiVerticesCount == 0)
		return E_INVALIDARG;

	uint vertices_data_size = uiVerticesCount * CCoreRendererGL::GetVertexSize(stDrawDesc),
		 indexes_data_size = uiIndexesCount * (stDrawDesc.bIndexBuffer32 ? sizeof(uint32) : sizeof(uint16));

	TDrawDataDesc desc(stDrawDesc);
	desc.pData = NULL;
	desc.pIndexBuffer = NULL;

	GLuint vbos[2];
	vbos[0] = vbos[1] = 0;

	DGLE_RESULT ret = S_OK;

	if (GLEW_ARB_vertex_buffer_object && eType != CRBT_SOFTWARE)
	{
		glGenBuffersARB(indexes_data_size != 0 ? 2 : 1, vbos);

		_pStateMan->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbos[0]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices_data_size, (GLvoid*)stDrawDesc.pData, (eType == CRBT_HARDWARE_DYNAMIC) ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB);

		if (vbos[1] != 0)
		{
			_pStateMan->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbos[1]);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexes_data_size, (GLvoid*)stDrawDesc.pIndexBuffer, (eType == CRBT_HARDWARE_DYNAMIC) ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB);
		}
	}
	else
	{
		if (eType != CRBT_SOFTWARE)
		{
			eType = CRBT_SOFTWARE;
			ret = S_FALSE;
		}

		desc.pData = new uint8[vertices_data_size];
		memcpy(desc.pData, stDrawDesc.pData, vertices_data_size);

		if (indexes_data_size != 0)
		{
			desc.pIndexBuffer = new uint8[indexes_data_size];
			memcpy(desc.pIndexBuffer, stDrawDesc.pIndexBuffer, indexes_data_size);
		}
		else
			desc.pIndexBuffer = NULL;
	}

	prBuffer = new CCoreGeometryBuffer(this, vbos[0], vbos[1], desc, vertices_data_size, indexes_data_size, uiVerticesCount, uiIndexesCount, eMode, eType);

	return ret;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ToggleStateFilter(bool bEnabled)
{
	_bStateFilterEnabled = bEnabled;
	_pStateMan = bEnabled ? (_pCachedStateMan->~CStateManager(), new(_pCachedStateMan) CStateManager<true>(InstIdx(), _iMaxTexUnits), static_cast<IStateManager *>(_pCachedStateMan)) : &_clPassThroughStateMan;
	
	if (_pFFP)
		_pFFP->ToggleStateFilter(bEnabled);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::InvalidateStateFilter()
{
	if (!_bStateFilterEnabled)
		return S_FALSE;
	
	ToggleStateFilter(false);
	ToggleStateFilter(true);

	_uiDrawCount = -1; // invalidate draw filtering

	if (_pFFP)
		_pFFP->InvalidateStateFilter();

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::PushStates()
{
	_pStateMan->Push();
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::PopStates()
{
	_pStateMan->Pop();
	_uiDrawCount = -1; // invalidate draw filtering
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetMatrix(const TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType)
{
	switch (eMatType)
	{
	case MT_MODELVIEW: _pStateMan->glMatrixMode(GL_MODELVIEW); break;
	case MT_PROJECTION: _pStateMan->glMatrixMode(GL_PROJECTION); break;
	case MT_TEXTURE: _pStateMan->glMatrixMode(GL_TEXTURE); break;
	}
	
	_pStateMan->glLoadMatrixf(stMatrix._1D);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetMatrix(TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType)
{
	switch (eMatType)
	{
	case MT_MODELVIEW: _pStateMan->glGetFloatv(GL_MODELVIEW_MATRIX, stMatrix._1D); break;
	case MT_PROJECTION: _pStateMan->glGetFloatv(GL_PROJECTION_MATRIX, stMatrix._1D); break;
	case MT_TEXTURE: _pStateMan->glGetFloatv(GL_TEXTURE_MATRIX, stMatrix._1D); break;
	}

	return S_OK;
}

inline void CCoreRendererGL::_TriangleStatistics(E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount)
{
	switch (eMode)
	{
	// Approximately treat point and line as one triangle.
	case CRDM_POINTS : _uiOverallTrianglesCount += uiCount; break;
	case CRDM_LINES : _uiOverallTrianglesCount += uiCount/2; break;
	case CRDM_LINE_STRIP : _uiOverallTrianglesCount += uiCount - 1; break;

	case CRDM_TRIANGLES : _uiOverallTrianglesCount += uiCount/3; break;
	case CRDM_TRIANGLE_STRIP : _uiOverallTrianglesCount += uiCount - 2; break;
	case CRDM_TRIANGLE_FAN : _uiOverallTrianglesCount += uiCount - 2; break;
	}
}

inline GLenum CCoreRendererGL::_GetGLDrawMode(E_CORE_RENDERER_DRAW_MODE eMode) const
{
	GLenum mode;

	switch (eMode)
	{
	case CRDM_POINTS : mode = GL_POINTS; break;
	case CRDM_LINES : mode = GL_LINES; break;
	case CRDM_LINE_STRIP : mode = GL_LINE_STRIP; break;
	case CRDM_TRIANGLES : mode = GL_TRIANGLES; break;
	case CRDM_TRIANGLE_STRIP : mode = GL_TRIANGLE_STRIP; break;
	case CRDM_TRIANGLE_FAN : mode = GL_TRIANGLE_FAN; break;
	}

	return mode;
}

FORCE_INLINE bool CCoreRendererGL::_LegacyDraw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount)
{
	if (_bVerticesBufferBindedFlag || uiCount > _sc_uiMaxVerticesCountForLegacy || stDrawDesc.pIndexBuffer != NULL ||
		(stDrawDesc.uiTangentOffset != -1 || stDrawDesc.uiBinormalOffset != -1 || stDrawDesc.pAttribs))
		return false;

	GLfloat prev_color[4];
	if (stDrawDesc.uiColorOffset != -1)
		_pStateMan->glGetFloatv(GL_CURRENT_COLOR, prev_color);

	const float * const data = (float *)stDrawDesc.pData;

	glBegin(_GetGLDrawMode(eMode));
	
	const	uint tex_offset = stDrawDesc.uiTextureVertexOffset / sizeof(float),
			color_offset = stDrawDesc.uiColorOffset / sizeof(float),
			normals_offset = stDrawDesc.uiNormalOffset / sizeof(float),
			vert_stride = stDrawDesc.uiVertexStride == 0 ? (stDrawDesc.bVertices2D ? 2 : 3):
			stDrawDesc.uiVertexStride / sizeof(float),
			tex_stride = stDrawDesc.uiTextureVertexStride == 0 ? 2 : stDrawDesc.uiTextureVertexStride / sizeof(float),
			color_stride = stDrawDesc.uiColorStride == 0 ? 4 : stDrawDesc.uiColorStride / sizeof(float),
			normals_stride = stDrawDesc.uiNormalStride == 0 ? 3 : stDrawDesc.uiNormalStride / sizeof(float);

	if (uiCount == 4 && stDrawDesc.bVertices2D && stDrawDesc.uiNormalOffset == -1) // most common case for single texture rendering
	{
		if (stDrawDesc.uiTextureVertexOffset != -1 && stDrawDesc.uiColorOffset == -1) // most common for 2D
		{
			glTexCoord2fv(&data[tex_offset]);	
			glVertex2fv(&data[0]);

			glTexCoord2fv(&data[tex_offset + tex_stride]);	
			glVertex2fv(&data[vert_stride]);

			glTexCoord2fv(&data[tex_offset + 2 * tex_stride]);	
			glVertex2fv(&data[2 * vert_stride]);

			glTexCoord2fv(&data[tex_offset + 3 * tex_stride]);	
			glVertex2fv(&data[3 * vert_stride]);
		}
		else
		{
			if (stDrawDesc.uiTextureVertexOffset != -1)
				glTexCoord2fv(&data[tex_offset]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset]);
			glVertex2fv(&data[0]);

			if (stDrawDesc.uiTextureVertexOffset != -1)
				glTexCoord2fv(&data[tex_offset + tex_stride]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + color_stride]);
			glVertex2fv(&data[vert_stride]);

			if (stDrawDesc.uiTextureVertexOffset != -1)
				glTexCoord2fv(&data[tex_offset + 2 * tex_stride]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + 2 * color_stride]);
			glVertex2fv(&data[2 * vert_stride]);

			if (stDrawDesc.uiTextureVertexOffset != -1)
				glTexCoord2fv(&data[tex_offset + 3 * tex_stride]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + 3 * color_stride]);
			glVertex2fv(&data[3 * vert_stride]);
		}
	}
	else
		for (uint i = 0; i < uiCount; ++i)
		{
			if (stDrawDesc.uiNormalOffset != -1)
				glNormal3fv(&data[normals_offset + i * normals_stride]);

			if (stDrawDesc.uiTextureVertexOffset != -1)
				glTexCoord2fv(&data[tex_offset + i * tex_stride]);
			
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + i * color_stride]);

			if (stDrawDesc.bVertices2D)
				glVertex2fv(&data[i * vert_stride]);
			else
				glVertex3fv(&data[i * vert_stride]);
		}

	glEnd();

	if (stDrawDesc.uiColorOffset != -1)
		glColor4fv(prev_color);

	return true;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount)
{
	++_uiOverallDrawCalls;

	_TriangleStatistics(eMode, uiCount);

	if (_LegacyDraw(stDrawDesc, eMode, uiCount))
		return S_OK;

	++_uiOverallDrawSetups;

	if (!_bVerticesBufferBindedFlag && GLEW_ARB_vertex_buffer_object)
	{
		_pStateMan->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		_pStateMan->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		_pCurBindedBuffer = NULL;
	}

	if (!_bStateFilterEnabled || (
			((!_bVerticesBufferBindedFlag || _pCurBindedBuffer == _pLastDrawnBuffer) && ( eMode != _eDrawMode || uiCount != _uiDrawCount || !(stDrawDesc == _stDrawDataDesc) )) ||
			(_bVerticesBufferBindedFlag && _pCurBindedBuffer != _pLastDrawnBuffer)
			))
	{
		++_uiUnfilteredDrawSetups;

		if (_bVerticesBufferBindedFlag)
			_pLastDrawnBuffer = _pCurBindedBuffer;

		if (stDrawDesc.uiColorOffset != -1)
		{
			_pStateMan->glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, stDrawDesc.uiColorStride, stDrawDesc.pData + stDrawDesc.uiColorOffset);
		}
		else
			_pStateMan->glDisableClientState(GL_COLOR_ARRAY);

		if (stDrawDesc.uiTextureVertexOffset != -1)
		{
			_pStateMan->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, stDrawDesc.uiTextureVertexStride, stDrawDesc.pData + stDrawDesc.uiTextureVertexOffset);
		}
		else
			_pStateMan->glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		if (stDrawDesc.uiNormalOffset != -1)
		{
			_pStateMan->glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, stDrawDesc.uiNormalStride, stDrawDesc.pData + stDrawDesc.uiNormalOffset);
		}
		else
			_pStateMan->glDisableClientState(GL_NORMAL_ARRAY);

		if (stDrawDesc.pAttribs)
		{
			/* not implemented */
		}

		if (stDrawDesc.pData || _bVerticesBufferBindedFlag)
		{
			_pStateMan->glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(stDrawDesc.bVertices2D ? 2 : 3, GL_FLOAT, stDrawDesc.uiVertexStride, stDrawDesc.pData);
		}
		else
			_pStateMan->glDisableClientState(GL_VERTEX_ARRAY);

		_stDrawDataDesc = stDrawDesc;
		_eDrawMode = eMode;
		_uiDrawCount = uiCount;
	}

	if (uiCount == 0 || (!stDrawDesc.pData && !_bVerticesBufferBindedFlag))
		return S_FALSE;

	if (stDrawDesc.pIndexBuffer || _bIndexesBufferBindedFlag)
		glDrawElements(_GetGLDrawMode(eMode), uiCount, stDrawDesc.bIndexBuffer32 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, stDrawDesc.pIndexBuffer);
	else
		glDrawArrays(_GetGLDrawMode(eMode), 0, uiCount);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::DrawBuffer(ICoreGeometryBuffer *pBuffer)
{
	if (!pBuffer)
	{
		if (GLEW_ARB_vertex_buffer_object)
		{
			_pStateMan->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
			_pStateMan->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		}

		_pCurBindedBuffer = NULL;
		
		return S_FALSE;
	}

	CCoreGeometryBuffer * const buff = (CCoreGeometryBuffer*)pBuffer;

	if (buff->GetVerticesVBO() != 0)
	{
		_bVerticesBufferBindedFlag = true;

		_pStateMan->glBindBufferARB(GL_ARRAY_BUFFER_ARB, buff->GetVerticesVBO());

		_pCurBindedBuffer = buff;

		if (buff->GetIndexesVBO())
		{
			_bIndexesBufferBindedFlag = true;
			_pStateMan->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, buff->GetIndexesVBO());
		}

		Draw(buff->GetDrawDesc(), buff->GetDrawMode(), buff->GetIndexesVBO() == 0 ? buff->GetVerticesCount() : buff->GetIndexesCount());
	}
	else
		Draw(buff->GetDrawDesc(), buff->GetDrawMode(), buff->GetDrawDesc().pIndexBuffer == NULL ? buff->GetVerticesCount() : buff->GetIndexesCount());

	_bVerticesBufferBindedFlag = false;
	_bIndexesBufferBindedFlag = false;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetColor(const TColor4 &stColor)
{
	_pStateMan->glColor4f(stColor.r, stColor.g, stColor.b, stColor.a);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetColor(TColor4 &stColor)
{
	GLfloat color[4];
	_pStateMan->glGetFloatv(GL_CURRENT_COLOR, color);
	stColor = color;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ToggleBlendState(bool bEnabled)
{
	if (bEnabled)
		_pStateMan->glEnable(GL_BLEND);
	else
		_pStateMan->glDisable(GL_BLEND);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ToggleAlphaTestState(bool bEnabled)
{
	if (bEnabled)
		_pStateMan->glEnable(GL_ALPHA_TEST);
	else
		_pStateMan->glDisable(GL_ALPHA_TEST);

	return S_OK;
}

inline GLenum CCoreRendererGL::_GetGLBlendFactor(E_BLEND_FACTOR factor) const
{
	switch (factor)
	{
	case BF_ZERO: return GL_ZERO;
	case BF_ONE: return GL_ONE;
	case BF_SRC_COLOR: return GL_SRC_COLOR;
	case BF_SRC_ALPHA: return GL_SRC_ALPHA;
	case BF_DST_COLOR: return GL_DST_COLOR;
	case BF_DST_ALPHA: return GL_DST_ALPHA;
	case BF_ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
	case BF_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
	}

	return GL_ZERO;
}

inline E_BLEND_FACTOR CCoreRendererGL::_GetEngBlendFactor(GLenum factor) const
{
	switch (factor)
	{
	case GL_ZERO: return BF_ZERO;
	case GL_ONE: return BF_ONE;
	case GL_SRC_COLOR: return BF_SRC_COLOR;
	case GL_SRC_ALPHA: return BF_SRC_ALPHA;
	case GL_DST_COLOR: return BF_DST_COLOR;
	case GL_DST_ALPHA: return BF_DST_ALPHA;
	case GL_ONE_MINUS_SRC_COLOR: return BF_ONE_MINUS_SRC_COLOR;
	case GL_ONE_MINUS_SRC_ALPHA: return BF_ONE_MINUS_SRC_ALPHA;
	}

	return BF_ZERO;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetBlendState(const TBlendStateDesc &stState)
{
	if (stState.bEnabled)
		_pStateMan->glEnable(GL_BLEND);
	else
		_pStateMan->glDisable(GL_BLEND);

	_pStateMan->glBlendFunc(_GetGLBlendFactor(stState.eSrcFactor), _GetGLBlendFactor(stState.eDstFactor));

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetBlendState(TBlendStateDesc &stState)
{
	stState.bEnabled = _pStateMan->glIsEnabled(GL_BLEND) == GL_TRUE;
	
	GLint src, dst;
	_pStateMan->glGetIntegerv(GL_BLEND_SRC, &src);
	_pStateMan->glGetIntegerv(GL_BLEND_DST, &dst);

	stState.eSrcFactor = _GetEngBlendFactor(src);
	stState.eDstFactor = _GetEngBlendFactor(dst);

	return S_OK;
}

inline GLenum CCoreRendererGL::_GetGLComparsionMode(E_COMPARISON_FUNC mode) const
{
	switch (mode)
	{
	case CF_NEVER: return GL_NEVER;
	case CF_LESS: return GL_LESS;
	case CF_EQUAL: return GL_EQUAL;
	case CF_LESS_EQUAL: return GL_LEQUAL;
	case CF_GREATER: return GL_GREATER;
	case CF_NOT_EQUAL: return GL_NOTEQUAL;
	case CF_GREATER_EQUAL: return GL_GEQUAL;
	case CF_ALWAYS: return GL_ALWAYS;
	}

	return GL_NEVER;
}

inline E_COMPARISON_FUNC CCoreRendererGL::_GetEngComparsionMode(GLenum mode) const
{
	switch (mode)
	{
	case GL_NEVER: return CF_NEVER;
	case GL_LESS: return CF_LESS;
	case GL_EQUAL: return CF_EQUAL;
	case GL_LEQUAL: return CF_LESS_EQUAL;
	case GL_GREATER: return CF_GREATER;
	case GL_NOTEQUAL: return CF_NOT_EQUAL;
	case GL_GEQUAL: return CF_GREATER_EQUAL;
	case GL_ALWAYS: return CF_ALWAYS;
	}

	return CF_NEVER;
}

DGLE_RESULT DGLE_API CCoreRendererGL::BindTexture(ICoreTexture *pTex, uint uiTextureLayer)
{
	if (uiTextureLayer > (uint)_iMaxTexUnits)
		return E_INVALIDARG;

	if (_iMaxTexUnits != 1)
		_pStateMan->glActiveTextureARB(GL_TEXTURE0_ARB + uiTextureLayer);
	
	if (pTex == NULL)
		_pStateMan->glBindTexture(GL_TEXTURE_2D, 0);
	else
		_pStateMan->glBindTexture(GL_TEXTURE_2D, ((CCoreTexture*)pTex)->GetTex());

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetBindedTexture(ICoreTexture *&prTex, uint uiTextureLayer)
{
	if (uiTextureLayer > (uint)_iMaxTexUnits)
		return E_INVALIDARG;
	
	if (_iMaxTexUnits != 1)
		_pStateMan->glActiveTextureARB(GL_TEXTURE0_ARB + uiTextureLayer);

	GLint tex_id;
	_pStateMan->glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &tex_id);

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
			
			if (((CCoreTexture*)p_ctex)->GetTex() == tex_id)
			{
				prTex = p_ctex;
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetDepthStencilState(const TDepthStencilDesc &stState)
{
	if (stState.bDepthTestEnabled)
		_pStateMan->glEnable(GL_DEPTH_TEST);
	else
		_pStateMan->glDisable(GL_DEPTH_TEST);

	if (stState.bWriteToDepthBuffer)
		_pStateMan->glDepthMask(GL_TRUE);
	else
		_pStateMan->glDepthMask(GL_FALSE);

	_pStateMan->glDepthFunc(_GetGLComparsionMode(stState.eDepthFunc));

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetDepthStencilState(TDepthStencilDesc &stState)
{
	stState.bDepthTestEnabled = _pStateMan->glIsEnabled(GL_DEPTH_TEST) == GL_TRUE;
	
	GLboolean wmask;
	_pStateMan->glGetBooleanv(GL_DEPTH_WRITEMASK, &wmask);
	stState.bWriteToDepthBuffer = wmask == GL_TRUE;

	GLint dfunc;
	_pStateMan->glGetIntegerv(GL_DEPTH_FUNC, &dfunc);
	stState.eDepthFunc = _GetEngComparsionMode(dfunc);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetRasterizerState(const TRasterizerStateDesc &stState)
{
	if (stState.bAlphaTestEnabled)
		_pStateMan->glEnable(GL_ALPHA_TEST);
	else
		_pStateMan->glDisable(GL_ALPHA_TEST);

	_pStateMan->glAlphaFunc(_GetGLComparsionMode(stState.eAlphaTestFunc), stState.fAlphaTestRefValue);

	if (stState.bScissorEnabled)
		_pStateMan->glEnable(GL_SCISSOR_TEST);
	else
		_pStateMan->glDisable(GL_SCISSOR_TEST);

	if (stState.bWireframe)
		_pStateMan->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		_pStateMan->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (stState.eCullMode == PCM_NONE)
		_pStateMan->glDisable(GL_CULL_FACE);
	else
	{
		_pStateMan->glEnable(GL_CULL_FACE);
		
		if (stState.eCullMode == PCM_FRONT)
			_pStateMan->glCullFace(GL_FRONT);
		else
			_pStateMan->glCullFace(GL_BACK);
	}

	if (stState.bFrontCounterClockwise)
		_pStateMan->glFrontFace(GL_CCW);
	else
		_pStateMan->glFrontFace(GL_CW);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetRasterizerState(TRasterizerStateDesc &stState)
{
	stState.bAlphaTestEnabled = _pStateMan->glIsEnabled(GL_ALPHA_TEST) == GL_TRUE;

	GLint i_v; GLfloat f_v;
	_pStateMan->glGetIntegerv(GL_ALPHA_TEST_FUNC, &i_v);
	_pStateMan->glGetFloatv(GL_ALPHA_TEST_REF, &f_v);

	stState.eAlphaTestFunc = _GetEngComparsionMode(i_v);
	stState.fAlphaTestRefValue = f_v;

	stState.bScissorEnabled = _pStateMan->glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE;

	_pStateMan->glGetIntegerv(GL_POLYGON_MODE, &i_v);
	stState.bWireframe = i_v == GL_LINE;

	if (_pStateMan->glIsEnabled(GL_CULL_FACE) == GL_TRUE)
	{
		_pStateMan->glGetIntegerv(GL_CULL_FACE_MODE, &i_v);
		stState.eCullMode = i_v == GL_FRONT ? PCM_FRONT : PCM_BACK;
	}
	else
		stState.eCullMode = PCM_NONE;
	
	_pStateMan->glGetIntegerv(GL_FRONT_FACE, &i_v);
	stState.bFrontCounterClockwise = i_v == GL_CCW;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Present()
{
	CBaseRendererGL::Present();

	_uiUnfilteredDrawSetups = 0;
	_uiOverallDrawSetups = 0;
	_uiOverallDrawCalls = 0;
	_uiOverallTrianglesCount = 0;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetFixedFunctionPipelineAPI(IFixedFunctionPipeline *&prFFP)
{
	prFFP = _pFFP;
	return _bIsGLSLSupported ? E_NOTIMPL : S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue)
{
	switch (eMetric)
	{
	case CRMT_MAX_TEXTURE_RESOLUTION: iValue = _iMaxTexResolution; break;
	case CRMT_MAX_ANISOTROPY_LEVEL: iValue = _iMaxAnisotropy; break;
	case CRMT_MAX_TEXTURE_LAYERS: iValue = _iMaxTexUnits; break;
	default:
		iValue = 0;
		return S_FALSE;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::IsFeatureSupported(E_CORE_RENDERER_FEATURE_TYPE eFeature, bool &bIsSupported)
{
	switch (eFeature)
	{
	case CRFT_BUILTIN_FULLSCREEN_MODE : bIsSupported = false; break;
	case CRFT_BUILTIN_STATE_FILTER : bIsSupported = true; break;
	case CRFT_MULTISAMPLING : bIsSupported = GLEW_ARB_multisample == GL_TRUE; break;
	case CRFT_VSYNC : 
#if defined(PLATFORM_WINDOWS)
		bIsSupported = WGLEW_EXT_swap_control == GL_TRUE;
#else
#error Platform not implemented.
#endif
		break;
	case CRFT_PROGRAMMABLE_PIPELINE : bIsSupported = _bIsGLSLSupported; break;
	case CRFT_LEGACY_FIXED_FUNCTION_PIPELINE_API : bIsSupported = _bIsGLSLSupported ? false : true; break;
	case CRFT_BGRA_DATA_FORMAT : bIsSupported = GLEW_EXT_bgra == GL_TRUE; break;
	case CRFT_TEXTURE_COMPRESSION : bIsSupported = GLEW_ARB_texture_compression == GL_TRUE; break;
	case CRFT_NON_POWER_OF_TWO_TEXTURES : bIsSupported = GLEW_ARB_texture_non_power_of_two == GL_TRUE; break;
	case CRFT_DEPTH_TEXTURES : bIsSupported = GLEW_ARB_depth_texture == GL_TRUE; break;
	case CRFT_TEXTURE_ANISOTROPY : bIsSupported = GLEW_EXT_texture_filter_anisotropic == GL_TRUE; break;
	case CRFT_TEXTURE_MIPMAP_GENERATION : bIsSupported = GLEW_SGIS_generate_mipmap == GL_TRUE || GLEW_ARB_framebuffer_object == GL_TRUE; break;
	case CRFT_TEXTURE_MIRRORED_REPEAT : bIsSupported = GLEW_VERSION_1_4 == GL_TRUE; break;
	case CRFT_TEXTURE_MIRROR_CLAMP : bIsSupported = GLEW_EXT_texture_mirror_clamp == GL_TRUE; break;
	case CRFT_GEOMETRY_BUFFER : bIsSupported = GLEW_ARB_vertex_buffer_object == GL_TRUE; break;
	case CRFT_FRAME_BUFFER :  bIsSupported = GLEW_EXT_framebuffer_object == GL_TRUE && GL_EXT_packed_depth_stencil == GL_TRUE ; break;

	default: 
		bIsSupported = false;
		return S_FALSE;
	}

	return S_OK;
}

void CCoreRendererGL::_ProfilerEventHandler() const
{
	if (_iProfilerState == 0)
		return;

	TColor4 color;

	Core()->RenderProfilerText("===Core Renderer Profiler===", color);

	if (_iProfilerState > 0)
	{
		if (_bStateFilterEnabled)
		{
			Core()->RenderProfilerText(("Overall draw calls:" + UIntToStr(_uiOverallDrawCalls)).c_str(), _uiOverallDrawCalls > _sc_uiMaxDrawCallsPerFrame ? ColorRed() : ColorWhite());
			Core()->RenderProfilerText(("Overall triangles:" + UIntToStr(_uiOverallTrianglesCount)).c_str(), ColorWhite());

			string str("Draw states setups (unfiltered/overall):" + UIntToStr(_uiUnfilteredDrawSetups) + '/' + UIntToStr(_uiOverallDrawSetups));

			if (_uiOverallDrawSetups)
				str.append(" (").append(UIntToStr((_uiOverallDrawSetups - _uiUnfilteredDrawSetups) * 100 / _uiOverallDrawSetups)).append("% redundant)");

			Core()->RenderProfilerText(str.c_str(), ColorWhite());
		}

		_pStateMan->OutputProfileSummary();
	}

	if (_iProfilerState > 1)
	{
		Core()->RenderProfilerText("---- State Filter Stats ----", color);
		_pStateMan->OutputProfileData();
		Core()->RenderProfilerText("----------------------------", color);
	}

	_pStateMan->ResetProfileData();
}

bool DGLE_API CCoreRendererGL::_s_ConPrintGLExts(void *pParameter, const char *pcParam)
{
	bool write = strlen(pcParam) != 0 && pcParam[0] == 'w';

	if (!write && strlen(pcParam) != 0)
	{
		CON(CCoreRendererGL, "Wrong command parametrs.");
		return false;
	}
	
	string res = string("------OpenGL Extensions------\n") + PTHIS(CCoreRendererGL)->_strOpenGLExtensions + "\n-----------------------------";

	if (write)
		LOG2(CCoreRendererGL, res, LT_INFO);
	else
		CON(CCoreRendererGL, res.c_str());

	return true;
}

void DGLE_API CCoreRendererGL::_s_EventsHandler(void *pParameter, IBaseEvent *pEvent)
{
	E_EVENT_TYPE type;
	pEvent->GetEventType(type);

	switch (type)
	{
	case ET_ON_PROFILER_DRAW:
		PTHIS(CCoreRendererGL)->_ProfilerEventHandler();
		break;
	
	case ET_ON_PER_SECOND_TIMER:
		PTHIS(CCoreRendererGL)->_KillDeadFBOs();
		break;
	}
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetRendererType(E_CORE_RENDERER_TYPE &eType)
{
	eType = CRT_OPENGL_LEGACY;
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetType(E_ENGINE_SUB_SYSTEM &eSubSystemType)
{
	eSubSystemType = ESS_CORE_RENDERER;
	return S_OK;
}

#endif