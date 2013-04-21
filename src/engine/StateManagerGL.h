/**
\author		Alexey Shaydurov aka ASH
\date		19.04.2013 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include "Common.h"

#ifndef NO_BUILTIN_RENDERER

#include "platform\windows\BaseRendererGL.h"

struct TProfileData
{
	TProfileData(): unfilteredCallCount(0), overallCallCount(0) {}
	uint unfilteredCallCount, overallCallCount;
};

class CToggleState : public CInstancedObj
{
	static const uint8 _sc_uiStatesCount = 13, // depends on CToggleState::GetCapIdx supported caps count
		_sc_uiMaxVertexAttribs = 16;

public:
	struct TProfileData
	{
		::TProfileData
			glEnable,
			glDisable,
			glEnableClientState,
			glDisableClientState,
			glEnableVertexAttribArrayARB,
			glDisableVertexAttribArrayARB;
	};

	inline CToggleState(uint uiInstIdx, TProfileData &profileData):
	CInstancedObj(uiInstIdx), _profileData(profileData), _iMaxAttribs(0)
	{
		if (!GLEW_ARB_vertex_shader)
		{
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_iMaxAttribs);

			if (_iMaxAttribs > _sc_uiMaxVertexAttribs)
				_iMaxAttribs = _sc_uiMaxVertexAttribs;
		}

		memset(_bState, 255, sizeof(GLboolean) * _sc_uiStatesCount);
		memset(_bAttrib, 255, sizeof(GLboolean) * _sc_uiMaxVertexAttribs);
	}

	inline void glEnable(GLenum cap);
	inline void glDisable(GLenum cap);
	inline void glEnableClientState(GLenum array);
	inline void glDisableClientState(GLenum array);
	inline void glEnableVertexAttribArrayARB(GLuint index);
	inline void glDisableVertexAttribArrayARB(GLuint index);
	inline GLboolean glIsEnabled(GLenum cap) const;
	inline void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const;

	inline void Set(const CToggleState &previous);

	inline CToggleState &operator =(const CToggleState &right)
	{
		_iMaxAttribs = right._iMaxAttribs;
		memcpy(_bState, right._bState, sizeof(GLboolean) * _sc_uiStatesCount);
		memcpy(_bAttrib, right._bAttrib, sizeof(GLboolean) * _sc_uiMaxVertexAttribs);
		return *this;
	}
private:
	inline uint8 GetCapIdx(GLenum cap) const;
	inline GLenum GetCap(uint8 idx) const;

	TProfileData &_profileData;
	GLint _iMaxAttribs;
	GLboolean _bState[_sc_uiStatesCount], _bAttrib[_sc_uiMaxVertexAttribs];
};

class CValueState
{
public:
	struct TProfileData
	{
		::TProfileData
			glLineWidth,
			glPointSize;
	};

	inline CValueState(TProfileData &profileData):
	_profileData(profileData),
	_fLineWidth((std::numeric_limits<float>::infinity)()),
	_fPointSize((std::numeric_limits<float>::infinity)())
	{}

	inline void glLineWidth(GLfloat width);
	inline void glPointSize(GLfloat size);	
	inline void glGetFloatv(GLenum pname, GLfloat *params) const;

	inline void Set(const CValueState &previous);

	inline CValueState &operator =(const CValueState &right)
	{
		_fLineWidth = right._fLineWidth;
		_fPointSize = right._fPointSize;
		return *this;
	}
private:
	GLfloat _fLineWidth, _fPointSize;
	TProfileData &_profileData;
};

class CModeState
{
	static const uint8 _sc_uiModeCount = 4;

public:
	struct TProfileData
	{
		::TProfileData
			glFrontFace,
			glCullFace,
			glDepthFunc,
			glDepthMask;
	};

	inline CModeState(TProfileData &profileData):
	_profileData(profileData)
	{
		memset(_mode, ~0, _sc_uiModeCount * sizeof(GLenum));
	}

	inline void glFrontFace(GLenum mode);
	inline void glCullFace(GLenum mode);
	inline void glDepthFunc(GLenum func);
	inline void glDepthMask(GLboolean flag);
	inline void glGetBooleanv(GLenum pname, GLboolean *params) const;
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CModeState &previous);

	inline CModeState &operator =(const CModeState &right)
	{
		memcpy(_mode, right._mode, sizeof(GLenum)*_sc_uiModeCount);
		return *this;
	}
private:
	GLenum _mode[_sc_uiModeCount];
	TProfileData &_profileData;
};

class CBindBufferState : public CInstancedObj
{
public:
	struct TProfileData
	{
		::TProfileData glBindBufferARB;
	};

	inline CBindBufferState(uint uiInstIdx, TProfileData &profileData):
	CInstancedObj(uiInstIdx), _profileData(profileData),
	_uiArrayBuffer(~0), _uiElementBuffer(~0)
	{}

	inline void glBindBufferARB(GLenum target, GLuint buffer);
	
	inline void Set(const CBindBufferState &previous);

	inline CBindBufferState &operator =(const CBindBufferState &right)
	{
		_uiArrayBuffer = right._uiArrayBuffer;
		_uiElementBuffer = right._uiElementBuffer;
		return *this;
	}
private:
	GLuint _uiArrayBuffer, _uiElementBuffer;
	TProfileData &_profileData;
};

class CPolygonModeState : public CInstancedObj
{
public:
	struct TProfileData
	{
		::TProfileData glPolygonMode;
	};

	inline CPolygonModeState(uint uiInstIdx, TProfileData &profileData):
	CInstancedObj(uiInstIdx), _profileData(profileData), _mode(~0)
	{}

	inline void glPolygonMode(GLenum face, GLenum mode);
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CPolygonModeState &previous);

	inline CPolygonModeState &operator =(const CPolygonModeState &right)
	{
		_mode = right._mode;
		return *this;
	}
private:
	GLenum _mode;
	TProfileData &_profileData;
};

class CViewportState : public CInstancedObj
{
public:
	struct TProfileData
	{
		::TProfileData glViewport,
			glScissor;
	};

	inline CViewportState(uint uiInstIdx, TProfileData &profileData):
	CInstancedObj(uiInstIdx), _profileData(profileData)
	{
		assert(typeid(GLint) == typeid(GLsizei));
		memset(_viewport, ~0, 4 * sizeof(GLint));
		memset(_scissor, ~0, 4 * sizeof(GLint));
	}

	inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	inline void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CViewportState &previous);

	inline CViewportState &operator =(const CViewportState &right)
	{
		return *this;
	}
private:
	GLint _viewport[4], _scissor[4];
	TProfileData &_profileData;
};

class CMatrixState : public CInstancedObj
{
public:
	struct TProfileData
	{
		::TProfileData glLoadMatrixf, glMatrixMode;
	};

	inline CMatrixState(uint uiInstIdx, TProfileData &profileData):
	CInstancedObj(uiInstIdx), _profileData(profileData), _mode(~0)
	{
		memset(_proj, ~0, 16 * sizeof(GLfloat));
		memset(_mview, ~0, 16 * sizeof(GLfloat));
		memset(_tex, ~0, 16 * sizeof(GLfloat));
		
		_proj[0] = (std::numeric_limits<float>::infinity)();
		_mview[0] = (std::numeric_limits<float>::infinity)();
		_tex[0] = (std::numeric_limits<float>::infinity)();
	}

	inline void glLoadMatrixf(const GLfloat *m);
	inline void glMatrixMode(GLenum mode);
	inline void glGetFloatv(GLenum pname, GLfloat *params) const;
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CMatrixState &previous);

	inline CMatrixState &operator =(const CMatrixState &right)
	{
		return *this;
	}
private:
	GLenum _mode;
	GLfloat _proj[16], _mview[16], _tex[16];
	TProfileData &_profileData;
};

class CBlendFuncState
{
public:
	struct TProfileData
	{
		::TProfileData
			glBlendFunc;
	};

	inline CBlendFuncState(TProfileData &profileData):
	_profileData(profileData), _srcFactor(~0), _dstFactor(~0)
	{}

	inline void glBlendFunc(GLenum sfactor, GLenum dfactor);
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CBlendFuncState &previous);

	inline CBlendFuncState &operator =(const CBlendFuncState &right)
	{
		_srcFactor = right._srcFactor;
		_dstFactor = right._dstFactor;
		return *this;
	}
private:
	GLenum _srcFactor, _dstFactor;
	TProfileData &_profileData;
};

class CAlphaFuncState
{
public:
	struct TProfileData
	{
		::TProfileData glAlphaFunc;
	};

	inline CAlphaFuncState(TProfileData &profileData):
	_profileData(profileData), _func(~0), _ref((std::numeric_limits<float>::infinity)())
	{}

	inline void glAlphaFunc(GLenum func, GLclampf ref);

	inline void glGetIntegerv(GLenum pname, GLint *params) const;
	inline void glGetFloatv(GLenum pname, GLfloat *params) const;

	inline void Set(const CAlphaFuncState &previous);

	inline CAlphaFuncState &operator =(const CAlphaFuncState &right)
	{
		_func = right._func;
		_ref = right._ref;
		return *this;
	}
private:
	GLenum  _func;
	GLclampf _ref;
	TProfileData &_profileData;
};

class CColorState
{
public:
	struct TProfileData
	{
		::TProfileData glColor4f,
			glClearColor;
	};

	inline CColorState(TProfileData &profileData):
	_profileData(profileData)
	{
		_color[0] = _color[1] = _color[2] = _color[3] = (std::numeric_limits<float>::infinity)();
		_clearColor[0] = _clearColor[1] = _clearColor[2] = _clearColor[3] = (std::numeric_limits<float>::infinity)();
	}

	inline void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	inline void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	inline void glGetFloatv(GLenum pname, GLfloat *params) const;

	inline void Set(const CColorState &previous);

	inline CColorState &operator =(const CColorState &right)
	{
		_color[0] = right._color[0];
		_color[1] = right._color[1];
		_color[2] = right._color[2];		
		_color[3] = right._color[3];
		
		_clearColor[0] = right._clearColor[0];
		_clearColor[1] = right._clearColor[1];
		_clearColor[2] = right._clearColor[2];
		_clearColor[3] = right._clearColor[3];
		
		return *this;
	}
private:
	GLfloat _color[4], _clearColor[4];
	TProfileData &_profileData;
};

class CTextureState : public CInstancedObj
{
public:
	struct TProfileData
	{
		::TProfileData
			glBindTexture,
			glActiveTextureARB;
	};

	inline CTextureState(uint uiInstIdx, GLint maxTextureUnits, TProfileData &profileData):
	CInstancedObj(uiInstIdx), _textures(maxTextureUnits), _profileData(profileData),
	_textureUnit(maxTextureUnits == 1 ? GL_TEXTURE0_ARB/*aviods ::glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, (GLint *)&_textureUnit) call in CTextureState::glBindTexture() if multitexturing does not supported*/ : ~0)
	{}

	inline void glBindTexture(GLenum target, GLuint texture);
	inline void glActiveTextureARB(GLenum texture);
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CTextureState &previous);

	inline CTextureState &operator =(const CTextureState &right)
	{
		_textures = right._textures;
		_textureUnit = right._textureUnit;
		return *this;
	}
private:
	struct TTexture
	{
		TTexture():texture(~0), enabled(false){}
		GLuint texture;
		bool enabled;
	};
	std::vector<TTexture> _textures;
	GLenum _textureUnit;
	TProfileData &_profileData;
};

class CProgramState
{
public:
	struct TProfileData
	{
		::TProfileData glUseProgramObjectARB;
	};

	inline CProgramState(TProfileData &profileData):
	_profileData(profileData), _programObj(~0)
	{}

	inline void glUseProgramObjectARB(GLhandleARB programObj);
	inline void glGetIntegerv(GLenum pname, GLint *params) const;

	inline void Set(const CProgramState &previous);

	inline CProgramState &operator =(const CProgramState &right)
	{
		_programObj = right._programObj;
		return *this;
	}
private:
	GLhandleARB _programObj;
	TProfileData &_profileData;
};

class IStateManager
{
public:
	virtual void Push() = 0;
	virtual void Pop() = 0;
	
	virtual void glEnable(GLenum cap) = 0;
	virtual void glDisable(GLenum cap) = 0;
	virtual void glEnableClientState(GLenum array) = 0;
	virtual void glDisableClientState(GLenum array) = 0;
	virtual void glEnableVertexAttribArrayARB(GLuint index) = 0;
	virtual void glDisableVertexAttribArrayARB(GLuint index) = 0;
	virtual void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const = 0;
	virtual GLboolean glIsEnabled(GLenum cap) const = 0;
	virtual void glGetIntegerv(GLenum pname, GLint *params) const = 0;
	virtual void glGetFloatv(GLenum pname, GLfloat *params) const = 0;
	virtual void glGetBooleanv(GLenum pname, GLboolean *params) const = 0;
	virtual void glBlendFunc(GLenum sfactor, GLenum dfactor) = 0;
	virtual void glAlphaFunc(GLenum func, GLclampf ref) = 0;
	virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
	virtual void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
	virtual void glBindTexture(GLenum target, GLuint texture) = 0;
	virtual void glActiveTextureARB(GLenum texture) = 0;
	virtual void glLoadMatrixf(const GLfloat *m) = 0;
	virtual void glMatrixMode(GLenum mode) = 0;
	virtual void glUseProgramObjectARB(GLhandleARB programObj) = 0;
	virtual void glFrontFace(GLenum mode) = 0;
	virtual void glCullFace(GLenum mode) = 0;
	virtual void glPolygonMode(GLenum face, GLenum mode) = 0;
	virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
	virtual void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
	virtual void glDepthFunc(GLenum func) = 0;
	virtual void glDepthMask(GLboolean flag) = 0;
	virtual void glLineWidth(GLfloat width) = 0;
	virtual void glPointSize(GLfloat size) = 0;
	virtual void glBindBufferARB(GLenum target, GLuint buffer) = 0;

	virtual void ResetProfileData() = 0;
	virtual void OutputProfileData() = 0;
	virtual void OutputProfileSummary() = 0;
};

template<bool cached>
class CStateManager;

template<>
class CStateManager<false>: public IStateManager, public CInstancedObj
{
public:
	CStateManager(uint uiInstIdx):CInstancedObj(uiInstIdx){}

	void Push();
	void Pop();

	void glEnable(GLenum cap);
	void glDisable(GLenum cap);
	void glEnableClientState(GLenum array);
	void glDisableClientState(GLenum array);
	void glEnableVertexAttribArrayARB(GLuint index);
	void glDisableVertexAttribArrayARB(GLuint index);
	void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const;
	GLboolean glIsEnabled(GLenum cap) const;
	void glBlendFunc(GLenum sfactor, GLenum dfactor);
	void glGetIntegerv(GLenum pname, GLint *params) const;
	void glGetFloatv(GLenum pname, GLfloat *params) const;
	void glGetBooleanv(GLenum pname, GLboolean *params) const;
	void glAlphaFunc(GLenum func, GLclampf ref);
	void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void glBindTexture(GLenum target, GLuint texture);
	void glActiveTextureARB(GLenum texture);
	void glLoadMatrixf(const GLfloat *m);
	void glMatrixMode(GLenum mode);
	void glUseProgramObjectARB(GLhandleARB programObj);
	void glFrontFace(GLenum mode);
	void glCullFace(GLenum mode);
	void glPolygonMode(GLenum face, GLenum mode);
	void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
	void glDepthFunc(GLenum func);
	void glDepthMask(GLboolean flag);
	void glLineWidth(GLfloat width);
	void glPointSize(GLfloat size);
	void glBindBufferARB(GLenum target, GLuint buffer);

	void ResetProfileData();
	void OutputProfileData();
	void OutputProfileSummary();
};

template<>
class CStateManager<true>: public CInstancedObj, public IStateManager
{
public:
	CStateManager(uint uiInstIdx, GLint maxTextureUnits):
	CInstancedObj(uiInstIdx), _curState(uiInstIdx, maxTextureUnits, _profileData)
	{}

	void Push();
	void Pop();

	void glEnable(GLenum cap);
	void glDisable(GLenum cap);
	void glEnableClientState(GLenum array);
	void glDisableClientState(GLenum array);
	void glEnableVertexAttribArrayARB(GLuint index);
	void glDisableVertexAttribArrayARB(GLuint index);
	void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const;
	GLboolean glIsEnabled(GLenum cap) const;
	void glGetIntegerv(GLenum pname, GLint *params) const;
	void glGetFloatv(GLenum pname, GLfloat *params) const;
	void glGetBooleanv(GLenum pname, GLboolean *params) const;
	void glBlendFunc(GLenum sfactor, GLenum dfactor);
	void glAlphaFunc(GLenum func, GLclampf ref);
	void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void glBindTexture(GLenum target, GLuint texture);
	void glActiveTextureARB(GLenum texture);
	void glLoadMatrixf(const GLfloat *m);
	void glMatrixMode(GLenum mode);
	void glUseProgramObjectARB(GLhandleARB programObj);
	void glFrontFace(GLenum mode);
	void glCullFace(GLenum mode);
	void glPolygonMode(GLenum face, GLenum mode);
	void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
	void glDepthFunc(GLenum func);
	void glDepthMask(GLboolean flag);
	void glLineWidth(GLfloat width);
	void glPointSize(GLfloat size);
	void glBindBufferARB(GLenum target, GLuint buffer);

	void ResetProfileData();
	void OutputProfileData();
	void OutputProfileSummary();

private:
	struct TProfileData
	{
		CToggleState::		TProfileData toggleStateProfileData;
		CBlendFuncState::	TProfileData blendFuncStateProfileData;
		CAlphaFuncState::	TProfileData alphaFuncStateProfileData;
		CColorState::		TProfileData colorStateProfileData;
		CTextureState::		TProfileData textureStateProfileData;
		CProgramState::		TProfileData programStateProfileData;
		CPolygonModeState:: TProfileData polygonModeStateProfileData;
		CViewportState::	TProfileData viewportStateProfileData;
		CMatrixState::		TProfileData matrxStateProfilerData;
		CModeState::		TProfileData modeStateProfilerData;
		CValueState::		TProfileData valueStateProfilerData;
		CBindBufferState::	TProfileData bindBufferStateProfilerData;
	} _profileData;
	
	struct TState
	{
		TState(uint uiInstIdx, GLint maxTextureUnits, TProfileData &profileData):
		toggleState(uiInstIdx, profileData.toggleStateProfileData),
		blendFuncState(profileData.blendFuncStateProfileData),
		alphaFuncState(profileData.alphaFuncStateProfileData),
		colorState(profileData.colorStateProfileData),
		textureState(uiInstIdx, maxTextureUnits, profileData.textureStateProfileData),
		programState(profileData.programStateProfileData),
		polygonModeState(uiInstIdx, profileData.polygonModeStateProfileData),
		viewportState(uiInstIdx, profileData.viewportStateProfileData),
		matrixState(uiInstIdx, profileData.matrxStateProfilerData),
		modeState(profileData.modeStateProfilerData),
		valueState(profileData.valueStateProfilerData),
		bindBufferState(uiInstIdx, profileData.bindBufferStateProfilerData)
		{}

		CToggleState toggleState;
		CBlendFuncState blendFuncState;
		CAlphaFuncState alphaFuncState;
		CColorState colorState;
		CTextureState textureState;
		CProgramState programState;
		CPolygonModeState polygonModeState;
		CViewportState viewportState;
		CMatrixState matrixState;
		CModeState modeState;
		CBindBufferState bindBufferState;
		CValueState valueState;
	} _curState;

	std::stack<TState> _stateStack;

	void _OutputFunctionProfileData(const std::string &functionName, const ::TProfileData &profileData);
};

#endif