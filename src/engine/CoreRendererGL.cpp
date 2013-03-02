/**
\author		Andrey Korotkov aka DRON
\date		11.09.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "CoreRendererGL.h"

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

	DGLE_RESULT DGLE_API GetObjectType(E_ENG_OBJ_TYPE &eType)
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
		 indexes_data_size = uiIndexesCount * (stDesc.bIndexBuffer32 ? sizeof(uint16) : sizeof(uint32)) * (stDesc.pIndexBuffer ? 3 : 0);

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
				if (_uiIndexesDataSize != indexes_data_size)
				{
					_uiIndexesCount = indexes_data_size;
					delete[] _stDrawDataDesc.pIndexBuffer;
					_stDrawDataDesc.pIndexBuffer = new uint8[_uiIndexesCount];
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

	DGLE_RESULT DGLE_API GetObjectType(E_ENG_OBJ_TYPE &eType)
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
		case TDF_BGR8: return GL_BGR_EXT;
		case TDF_RGB8: return GL_RGB;
		case TDF_DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT;
		case TDF_BGRA8: return GL_BGRA_EXT;
		case TDF_RGBA8: return GL_RGBA;
		case TDF_ALPHA8: return GL_ALPHA;
		case TDF_DXT1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		case TDF_DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		default : return 0;
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
		default : return 0;
		}

		int w = _w, h = _h;
		uint l = 0;

		while (l != lod)
		{
			w /= 2; h /= 2;
			++l;
		}

		if (w == 0 || h == 0)
			return 0;

		lw = w; lh = h;

		if (_format == TDF_DXT1 || _format == TDF_DXT5)
			return ((w + 3)/4)*((h + 3)/4)*bytes;
		else
			return h*w*bytes;
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

		uint w, h, size = _DataSize(uiLodLevel, w, h);

		if (size != uiDataSize)
		{
			uiDataSize = size;
			return S_FALSE;
		}

		_pCR->BindTexture(this, 0);

		if (_format == TDF_DXT1 || _format == TDF_DXT5)
			glGetCompressedTexImageARB(GL_TEXTURE_2D, uiLodLevel, pData);
		else
			glGetTexImage(GL_TEXTURE_2D, uiLodLevel, _GetGLFormat(), GL_UNSIGNED_BYTE, (GLvoid*)pData);

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
			glTexSubImage2D(GL_TEXTURE_2D, uiLodLevel, 0, 0, w, h, _GetGLFormat(), GL_UNSIGNED_BYTE, (GLvoid*)pData);

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
_uiDrawCount(-1)
{}

inline uint CCoreRendererGL::GetVertexSize(const TDrawDataDesc &stDesc)
{
	uint res = sizeof(float) * ((stDesc.bVertexCoord2 ? 2 : 3) + (stDesc.uiNormalOffset != -1 ? 3 : 0) + (stDesc.uiTexCoordOffset != -1 ? 2 : 0) +
			(stDesc.uiColorOffset != -1 ? 4 : 0) + (stDesc.uiTangentOffset != -1 ? 3 : 0) + (stDesc.uiBinormalOffset != -1 ? 3 : 0));

	if (stDesc.pAttribs)
	{
		/* not implemented */
	}

	return res;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Prepare(TCRendererInitResult &stResults)
{
	return stResults = CBaseRendererGL::Prepare(), stResults ? S_OK : E_FAIL;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Initialize(TCRendererInitResult &stResults)
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

	if (GLEW_ARB_multitexture) 
	{
		if (GLEW_ARB_shading_language_100)
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &_iMaxTexUnits);
		else
			glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &_iMaxTexUnits);
	}
	else
		_iMaxTexUnits = 1;

	_bIsGLSLSupported = GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_ARB_shading_language_100;

	_pStateMan = _pCachedStateMan = new CStateManager<true>(InstIdx(), _iMaxTexUnits);

	_pStateMan->glEnable(GL_TEXTURE_2D); // Let it be always enabled for the zero texturing layer.

	// ToDo: Switch off for programmable pipeline.
	glGetIntegerv(GL_MAX_LIGHTS, &_iMaxLight);
	_pStateMan->glEnable(GL_COLOR_MATERIAL);
	_pStateMan->glEnable(GL_NORMALIZE);
	//

	_stCurrentState.clActivatedTexUnits.resize(_iMaxTexUnits);

	SetBlendState(_stCurrentState.stBlendDesc);
	SetDepthStencilState(_stCurrentState.stDepthStencilDesc);
	SetRasterizerState(_stCurrentState.stRasterDesc);

	Core()->AddEventListener(ET_ON_PROFILER_DRAW, _s_ProfilerEventHandler, this);

	Console()->RegComProc("crgl_print_exts_list", "Reports extensions supported by current OpenGL implementation.\nw - write to logfile.", &_s_ConPrintGLExts, (void*)this);
	Console()->RegComValue("crgl_profiler", "Displays Core Renderer OpenGL subsystems profiler.", &_iProfilerState, 0, 2);

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

	Core()->RemoveEventListener(ET_ON_PROFILER_DRAW, _s_ProfilerEventHandler, this);

	delete _pCachedStateMan;

	DGLE_RESULT ret = CBaseRendererGL::Finalize() ? S_OK : E_FAIL;

	LOG("Core Renderer finalized.", LT_INFO);

	return ret;
}

DGLE_RESULT DGLE_API CCoreRendererGL::AdjustMode(TEngWindow &stNewWin)
{
	return CBaseRendererGL::AdjustMode(stNewWin) ? S_OK : S_FALSE;
}

DGLE_RESULT DGLE_API CCoreRendererGL::MakeCurrent()
{
	return CBaseRendererGL::MakeCurrent() ? S_OK : E_ABORT;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetClearColor(const TColor4 &stColor)
{
	glClearColor(stColor.r, stColor.g, stColor.b, stColor.a);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::Clear(bool bColor, bool bDepth, bool bStencil)
{
	GLbitfield mask = 0;

	if (bColor)		mask |= GL_COLOR_BUFFER_BIT;
	if (bDepth)		mask |= GL_DEPTH_BUFFER_BIT;
	if (bStencil)	mask |= GL_STENCIL_BUFFER_BIT;

	glClear(mask);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetViewport(uint x, uint y, uint width, uint height)
{
	_stCurrentState.iViewPortX = x; _stCurrentState.iViewPortY = y;
	_stCurrentState.iViewPortWidth = width; _stCurrentState.iViewPortHeight = height;
	glViewport(x, y, width, height);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetViewport(uint &x, uint &y, uint &width, uint &height)
{
	x = _stCurrentState.iViewPortX; y = _stCurrentState.iViewPortY;
	width = _stCurrentState.iViewPortWidth; height = _stCurrentState.iViewPortHeight;
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetScissor(uint x, uint y, uint width, uint height)
{
	glScissor(_stCurrentState.iViewPortX + x, (int)(_stCurrentState.iViewPortHeight - y - height), width, height);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetLineWidth(float fWidth)
{
	_pStateMan->glLineWidth(fWidth);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetPointSize(float fSize)
{
	_pStateMan->glPointSize(fSize);
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ReadFrameBuffer(uint8 *pData, uint uiDataSize, E_TEXTURE_DATA_FORMAT eDataFormat)
{
	GLenum format;
	int bytes;

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
	case TDF_DEPTH_COMPONENT24: format = GL_DEPTH_COMPONENT; bytes = 3; break;
	default : return E_INVALIDARG;
	}

	if (uiDataSize < Core()->EngWindow()->uiWidth*Core()->EngWindow()->uiHeight*bytes)
		return E_INVALIDARG;

	glReadPixels(0, 0, Core()->EngWindow()->uiWidth, Core()->EngWindow()->uiHeight, format, GL_UNSIGNED_BYTE, pData);
	
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetRenderTarget(ICoreTexture *pTexture)
{
	//ToDo
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::CreateTexture(ICoreTexture *&prTex, const uint8 * const pData, uint uiWidth, uint uiHeight, bool bMipmapsPresented, E_CORE_RENDERER_DATA_ALIGNMENT eDataAlignment, E_TEXTURE_DATA_FORMAT eDataFormat, E_TEXTURE_LOAD_FLAGS eLoadFlags)
{
	const bool b_non_power_of_two = uiWidth != 1 << (int)floor( ( log( (double)uiWidth ) / log(2.f) ) + 0.5f ) || uiHeight != 1 << (int)floor( ( log( (double)uiHeight ) / log(2.f) ) + 0.5f );

	if (
		((eDataFormat == TDF_BGR8 || eDataFormat == TDF_BGRA8) && !GLEW_EXT_bgra) || 
		((eDataFormat == TDF_DXT1 || eDataFormat == TDF_DXT5) && !GLEW_ARB_texture_compression) ||
		(eDataFormat == TDF_DEPTH_COMPONENT24 && !GLEW_ARB_depth_texture) ||
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
		bytes_per_pixel = 8; //per block
		break;
	case TDF_DXT5:
		tex_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		bytes_per_pixel = 16; //per block
		break;
	case TDF_DEPTH_COMPONENT24:
		tex_format = GL_DEPTH_COMPONENT;
		tex_internal_format= GL_DEPTH_COMPONENT24_ARB;
		bytes_per_pixel = 3;
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
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	if (bMipmapsPresented)
	{
		int i_mipmaps = 0, max_side = max(uiWidth, uiHeight)/2;

		//This loop is more correct because of NPOT textures than calculation like this: (int)(log((float)max(uiWidth, uiHeight))/log(2.f))
		while (max_side > 0)
		{
			max_side /= 2;
			++i_mipmaps;
		}

		int i_cur_w = uiWidth*2, i_cur_h = uiHeight*2;
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
					dat_offset += ((i_cur_w + 3)/4)*((i_cur_h + 3)/4)*bytes_per_pixel;
				else
					dat_offset += i_cur_h*(i_cur_w*bytes_per_pixel + cur_align);
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
				data_size = ((i_cur_w + 3)/4)*((i_cur_h + 3)/4)*bytes_per_pixel;
			else
				data_size = i_cur_h*(i_cur_w*bytes_per_pixel + cur_align);
			
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
			(int &)eLoadFlags |= TLF_GENERATE_MIPMAPS;
			ret = S_FALSE;
		}

		if (eLoadFlags & TLF_GENERATE_MIPMAPS && (!GLEW_ARB_framebuffer_object && GLEW_SGIS_generate_mipmap))
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		if (b_is_compressed)
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, tex_format, uiWidth, uiHeight, 0, ((uiWidth + 3)/4)*((uiHeight + 3)/4)*bytes_per_pixel, (void*)pData);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, tex_internal_format, uiWidth, uiHeight, 0, tex_format, GL_UNSIGNED_BYTE, (void*)pData);

		if (eLoadFlags & TLF_GENERATE_MIPMAPS)
		{
			if (GLEW_ARB_framebuffer_object)
				glGenerateMipmap(GL_TEXTURE_2D);
			else
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
		 indexes_data_size = uiIndexesCount * (stDrawDesc.bIndexBuffer32 ? sizeof(uint16) : sizeof(uint32)) * (stDrawDesc.pIndexBuffer ? 3 : 0);

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
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::PushStates()
{
	_pStateMan->Push();
	_clStatesStack.push(_stCurrentState);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::PopStates()
{
	if (_clStatesStack.empty())
		return E_ABORT;
	
	_pStateMan->Pop();

	_stCurrentState = _clStatesStack.top();
	
	SetMatrix(_stCurrentState.stTextureMat, MT_TEXTURE);
	SetMatrix(_stCurrentState.stProjectionMat, MT_PROJECTION);
	SetMatrix(_stCurrentState.stModelviewMat, MT_MODELVIEW);

	_clStatesStack.pop();

	_uiDrawCount = -1; // invalidate draw filtering

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetMatrix(const TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType)
{
	if (eMatType != MT_MODELVIEW)
	{
		if (eMatType == MT_PROJECTION)
			glMatrixMode(GL_PROJECTION);
		else
			if (eMatType == MT_TEXTURE)
				glMatrixMode(GL_TEXTURE);
	}

	switch (eMatType)
	{
	case MT_MODELVIEW: _stCurrentState.stModelviewMat = stMatrix; break;
	case MT_PROJECTION: _stCurrentState.stProjectionMat = stMatrix; break;
	case MT_TEXTURE: _stCurrentState.stTextureMat = stMatrix; break;
	}

	glLoadMatrixf(stMatrix._1D);

	if (eMatType != MT_MODELVIEW)
		glMatrixMode(GL_MODELVIEW);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetMatrix(TMatrix4x4 &stMatrix, E_MATRIX_TYPE eMatType)
{
	switch (eMatType)
	{
	case MT_MODELVIEW: stMatrix = _stCurrentState.stModelviewMat; break;
	case MT_PROJECTION: stMatrix = _stCurrentState.stProjectionMat; break;
	case MT_TEXTURE: stMatrix = _stCurrentState.stTextureMat; break;
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

#ifdef USE_LEGACY_DRAW
__forceinline bool CCoreRendererGL::_LegacyDraw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount)
{
	if (_bVerticesBufferBindedFlag || uiCount > _sc_uiMaxVerticesCountForLegacy || stDrawDesc.pIndexBuffer != NULL ||
		(stDrawDesc.uiTangentOffset != -1 || stDrawDesc.uiBinormalOffset != -1 || stDrawDesc.pAttribs))
		return false;

	const float * const data = (float *)stDrawDesc.pData;

	glBegin(_GetGLDrawMode(eMode));
	
	const	uint tex_offset = stDrawDesc.uiTexCoordOffset / sizeof(float),
			color_offset = stDrawDesc.uiColorOffset / sizeof(float),
			normals_offset = stDrawDesc.uiNormalOffset / sizeof(float),
			vert_stride = stDrawDesc.uiVertexStride == 0 ? (stDrawDesc.bVertexCoord2 ? 2 : 3):
			stDrawDesc.uiVertexStride / sizeof(float),
			tex_stride = stDrawDesc.uiTexCoordStride == 0 ? 2 : stDrawDesc.uiTexCoordStride / sizeof(float),
			color_stride = stDrawDesc.uiColorStride == 0 ? 4 : stDrawDesc.uiColorStride / sizeof(float),
			normals_stride = stDrawDesc.uiNormalStride == 0 ? 3 : stDrawDesc.uiNormalStride / sizeof(float);

	if (uiCount == 4 && stDrawDesc.bVertexCoord2 && stDrawDesc.uiNormalOffset == -1) // most common case for 2D
	{
			if (stDrawDesc.uiTexCoordOffset != -1)
				glTexCoord2fv(&data[tex_offset]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset]);
			glVertex2fv(&data[0]);

			if (stDrawDesc.uiTexCoordOffset != -1)
				glTexCoord2fv(&data[tex_offset + tex_stride]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + color_stride]);
			glVertex2fv(&data[vert_stride]);

			if (stDrawDesc.uiTexCoordOffset != -1)
				glTexCoord2fv(&data[tex_offset + 2 * tex_stride]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + 2 * color_stride]);
			glVertex2fv(&data[2*vert_stride]);

			if (stDrawDesc.uiTexCoordOffset != -1)
				glTexCoord2fv(&data[tex_offset + 3 * tex_stride]);	
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + 3 * color_stride]);
			glVertex2fv(&data[3*vert_stride]);
	}
	else
		for (uint i = 0; i < uiCount; ++i)
		{
			if (stDrawDesc.uiNormalOffset != -1)
				glNormal3fv(&data[normals_offset + i * normals_stride]);

			if (stDrawDesc.uiTexCoordOffset != -1)
				glTexCoord2fv(&data[tex_offset + i * tex_stride]);
			
			if (stDrawDesc.uiColorOffset != -1)
				glColor4fv(&data[color_offset + i * color_stride]);

			if (stDrawDesc.bVertexCoord2)
				glVertex2fv(&data[i * vert_stride]);
			else
				glVertex3fv(&data[i * vert_stride]);
		}

	glEnd();

	if (stDrawDesc.uiColorOffset != -1)
		glColor4f(_stCurrentState.stColor.r, _stCurrentState.stColor.g, _stCurrentState.stColor.b, _stCurrentState.stColor.a);

	return true;
}
#endif

DGLE_RESULT DGLE_API CCoreRendererGL::Draw(const TDrawDataDesc &stDrawDesc, E_CORE_RENDERER_DRAW_MODE eMode, uint uiCount)
{
	++_uiOverallDrawCalls;

	_TriangleStatistics(eMode, uiCount);

#ifdef USE_LEGACY_DRAW
	if (_LegacyDraw(stDrawDesc, eMode, uiCount))
		return S_OK;
#endif

	++_uiOverallDrawSetups;

	if (!_bVerticesBufferBindedFlag && GLEW_ARB_vertex_buffer_object)
	{
		_pStateMan->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		_pStateMan->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		_pCurBindedBuffer = NULL;
	}

	if (!_bStateFilterEnabled || (
			((!_bVerticesBufferBindedFlag || _pCurBindedBuffer == _pLastDrawnBuffer) && ( eMode != _eDrawMode || uiCount != _uiDrawCount || memcmp(&stDrawDesc, &_stDrawDataDesc, sizeof(TDrawDataDesc)) != 0 )) ||
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

		if (stDrawDesc.uiTexCoordOffset != -1)
		{
			_pStateMan->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, stDrawDesc.uiTexCoordStride, stDrawDesc.pData + stDrawDesc.uiTexCoordOffset);
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
			glVertexPointer(stDrawDesc.bVertexCoord2 ? 2 : 3, GL_FLOAT, stDrawDesc.uiVertexStride, stDrawDesc.pData);
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
	_stCurrentState.stColor = stColor;
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ToggleBlendState(bool bEnabled)
{
	_stCurrentState.stBlendDesc.bEnable = bEnabled;

	if (bEnabled)
		_pStateMan->glEnable(GL_BLEND);
	else
		_pStateMan->glDisable(GL_BLEND);

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::ToggleAlphaTestState(bool bEnabled)
{
	_stCurrentState.stRasterDesc.bAlphaTestEnable = bEnabled;

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

DGLE_RESULT DGLE_API CCoreRendererGL::SetBlendState(const TBlendStateDesc &stState)
{
	if (stState.bEnable)
		_pStateMan->glEnable(GL_BLEND);
	else
		_pStateMan->glDisable(GL_BLEND);

	_pStateMan->glBlendFunc(_GetGLBlendFactor(stState.eSrcFactor), _GetGLBlendFactor(stState.eDestFactor));

	_stCurrentState.stBlendDesc = stState;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetBlendState(TBlendStateDesc &stState)
{
	stState = _stCurrentState.stBlendDesc;
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

DGLE_RESULT DGLE_API CCoreRendererGL::BindTexture(ICoreTexture *pTex, uint uiTextureLayer)
{
	if (uiTextureLayer > (uint)_iMaxTexUnits)
		return E_INVALIDARG;

	if (pTex == NULL && !_stCurrentState.clActivatedTexUnits[uiTextureLayer])
		return S_OK;

	if (_iMaxTexUnits != 1)
		_pStateMan->glActiveTextureARB(GL_TEXTURE0_ARB + uiTextureLayer);
	
	if (pTex == NULL)
	{
		_pStateMan->glBindTexture(GL_TEXTURE_2D, 0);
		_stCurrentState.clActivatedTexUnits[uiTextureLayer] = false;
	}
	else
	{
		_pStateMan->glBindTexture(GL_TEXTURE_2D, ((CCoreTexture*)pTex)->GetTex());
		_stCurrentState.clActivatedTexUnits[uiTextureLayer] = true;
	}

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetDepthStencilState(const TDepthStencilDesc &stState)
{
	if (stState.bDepthTestEnable)
		_pStateMan->glEnable(GL_DEPTH_TEST);
	else
		_pStateMan->glDisable(GL_DEPTH_TEST);

	if (stState.bWriteToDepthBuffer)
		_pStateMan->glDepthMask(GL_TRUE);
	else
		_pStateMan->glDepthMask(GL_FALSE);

	_pStateMan->glDepthFunc(_GetGLComparsionMode(stState.eDepthFunc));

	_stCurrentState.stDepthStencilDesc = stState;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetDepthStencilState(TDepthStencilDesc &stState)
{
	stState = _stCurrentState.stDepthStencilDesc;
	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::SetRasterizerState(const TRasterizerStateDesc &stState)
{
	if (stState.bAlphaTestEnable)
		_pStateMan->glEnable(GL_ALPHA_TEST);
	else
		_pStateMan->glDisable(GL_ALPHA_TEST);

	_pStateMan->glAlphaFunc(_GetGLComparsionMode(stState.eAlphaTestFunc), stState.fAlphaTestRefValue);

	if (stState.bScissorEnable)
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

	_stCurrentState.stRasterDesc = stState;

	return S_OK;
}

DGLE_RESULT DGLE_API CCoreRendererGL::GetRasterizerState(TRasterizerStateDesc &stState)
{
	stState = _stCurrentState.stRasterDesc;
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

DGLE_RESULT DGLE_API CCoreRendererGL::GetDeviceMetric(E_CORE_RENDERER_METRIC_TYPE eMetric, int &iValue)
{
	switch (eMetric)
	{
	case CRMT_MAX_TEXTURE_RESOLUTION: iValue = _iMaxTexResolution; break;
	case CRMT_MAX_ANISOTROPY_LEVEL: iValue = _iMaxAnisotropy; break;
	case CRMT_MAX_LIGHTS_PER_PASS: iValue = _iMaxLight; break;
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
	case CRSF_BUILTIN_FSCREEN_MODE : bIsSupported = false; break;
	case CRSF_BUILTIN_STATE_FILTER : bIsSupported = true; break;
	case CRSF_MULTISAMPLING : bIsSupported = GLEW_ARB_multisample == GL_TRUE; break;
	case CRDF_VSYNC : 
#if defined(PLATFORM_WINDOWS)
		bIsSupported = WGLEW_EXT_swap_control == GL_TRUE; 
#endif
		break;
	case CRDF_PROGRAMMABLE_PIPELINE : bIsSupported = _bIsGLSLSupported == GL_TRUE; break;
	case CRSF_BGRA_DATA_FORMAT : bIsSupported = GLEW_EXT_bgra == GL_TRUE; break;
	case CRSF_TEXTURE_COMPRESSION : bIsSupported = GLEW_ARB_texture_compression == GL_TRUE; break;
	case CRSF_NON_POWER_OF_TWO_TEXTURES : bIsSupported = GLEW_ARB_texture_non_power_of_two == GL_TRUE; break;
	case CRSF_DEPTH_TEXTURES : bIsSupported = GLEW_ARB_depth_texture == GL_TRUE; break;
	case CRSF_TEXTURE_ANISOTROPY : bIsSupported = GLEW_EXT_texture_filter_anisotropic == GL_TRUE; break;
	case CRSF_TEXTURE_MIPMAP_GENERATION : bIsSupported = GLEW_SGIS_generate_mipmap == GL_TRUE || GLEW_ARB_framebuffer_object == GL_TRUE; break;
	case CRDF_TEXTURE_MIRRORED_REPEAT : bIsSupported = GLEW_VERSION_1_4 == GL_TRUE; break;
	case CRDF_TEXTURE_MIRROR_CLAMP : bIsSupported = GLEW_EXT_texture_mirror_clamp == GL_TRUE; break;
	case CRDF_GEOMETRY_BUFFER : bIsSupported = GLEW_ARB_vertex_buffer_object == GL_TRUE; break;

	default: 
		bIsSupported = false;
		return S_FALSE;
	}

	return S_OK;
}

void CCoreRendererGL::_ProfilerEventHandler(IBaseEvent *pEvent) const
{
	if (_iProfilerState == 0)
		return;

	TColor4 color;

	Core()->RenderProfilerTxt("===Core Renderer Profiler===", color);

	if (_iProfilerState > 0)
	{
		if (_bStateFilterEnabled)
		{
			Core()->RenderProfilerTxt(("Overall draw calls:" + UIntToStr(_uiOverallDrawCalls)).c_str(), _uiOverallDrawCalls > _sc_uiMaxDrawCallsPerFrame ? TColor4(255, 0, 0, 255) : TColor4());
			Core()->RenderProfilerTxt(("Overall triangles:" + UIntToStr(_uiOverallTrianglesCount)).c_str(), TColor4());

			string str("Draw states setups (unfiltered/overall):" + UIntToStr(_uiUnfilteredDrawSetups) + '/' + UIntToStr(_uiOverallDrawSetups));

			if (_uiOverallDrawSetups)
				str.append(" (").append(UIntToStr((_uiOverallDrawSetups - _uiUnfilteredDrawSetups) * 100 / _uiOverallDrawSetups)).append("% redundant)");

			Core()->RenderProfilerTxt(str.c_str(), TColor4());
		}

		_pStateMan->OutputProfileSummary();
	}

	if (_iProfilerState > 1)
	{
		Core()->RenderProfilerTxt("---- State Filter Stats ----", color);
		_pStateMan->OutputProfileData();
		Core()->RenderProfilerTxt("----------------------------", color);
	}

	_pStateMan->ResetProfileData();
}

void DGLE_API CCoreRendererGL::_s_ConPrintGLExts(void *pParameter, const char *pcParam)
{
	bool write = strlen(pcParam) != 0 && pcParam[0] == 'w';
	
	string res = string("------OpenGL Extensions------\n") + PTHIS(CCoreRendererGL)->_strOpenGLExtensions + "\n-----------------------------";

	if (write)
		LOG2(CCoreRendererGL, res, LT_INFO);
	else
		CON(CCoreRendererGL, res.c_str());
}

void DGLE_API CCoreRendererGL::_s_ProfilerEventHandler(void *pParameter, IBaseEvent *pEvent)
{
	PTHIS(CCoreRendererGL)->_ProfilerEventHandler(pEvent);
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