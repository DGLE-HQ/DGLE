/**
\author		Alexey Shaydurov aka ASH
\date		20.04.2013 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "StateManagerGL.h"

#ifndef NO_BUILTIN_RENDERER

using namespace std;

#define UNDEFINED_CAP_FATAL(cap) LOG("Undefined State Manager cap: " + UIntToStrX(cap) + "! This is program logic issue, please report to engine developers.", LT_FATAL);

inline uint8 CToggleState::GetCapIdx(GLenum cap) const
{
	uint8 idx = _sc_uiStatesCount;

	switch(cap)
	{
		case GL_TEXTURE_COORD_ARRAY: idx = 0; break;
		case GL_COLOR_ARRAY: idx = 1; break;
		case GL_NORMAL_ARRAY: idx = 2; break;
		case GL_VERTEX_ARRAY: idx = 3; break;

		case GL_TEXTURE_2D: idx = 4; break;
		case GL_NORMALIZE: idx = 5; break;
		case GL_COLOR_MATERIAL: idx = 6; break; 
		case GL_LIGHTING: idx = 7; break;
		case GL_BLEND: idx = 8; break;
		case GL_DEPTH_TEST: idx = 9; break;
		case GL_ALPHA_TEST: idx = 10; break;
		case GL_SCISSOR_TEST: idx = 11; break;
		case GL_CULL_FACE: idx = 12; break;
		default:
			UNDEFINED_CAP_FATAL(cap)
	}

	return idx;
}

inline GLenum CToggleState::GetCap(uint8 idx) const
{
	GLenum cap;

	switch(idx)
	{
		case 0: cap = GL_TEXTURE_COORD_ARRAY; break;
		case 1: cap = GL_COLOR_ARRAY; break;
		case 2: cap = GL_NORMAL_ARRAY; break;
		case 3: cap = GL_VERTEX_ARRAY; break;

		case 4: cap = GL_TEXTURE_2D; break;
		case 5: cap = GL_NORMALIZE; break;
		case 6: cap = GL_COLOR_MATERIAL; break; 
		case 7: cap = GL_LIGHTING; break;
		case 8: cap = GL_BLEND; break;
		case 9: cap = GL_DEPTH_TEST; break;
		case 10: cap = GL_ALPHA_TEST; break;
		case 11: cap = GL_SCISSOR_TEST; break;
		case 12: cap = GL_CULL_FACE; break;
		default:
			UNDEFINED_CAP_FATAL(idx)
	}

	return cap;
}

inline void CToggleState::glEnable(GLenum cap)
{
	++_profileData.glEnable.overallCallCount;
	if (_bState[GetCapIdx(cap)] != GL_TRUE)
	{
		_bState[GetCapIdx(cap)] = GL_TRUE;
		++_profileData.glEnable.unfilteredCallCount;
		::glEnable(cap);
	}
}

inline void CToggleState::glDisable(GLenum cap)
{
	++_profileData.glDisable.overallCallCount;
	if (_bState[GetCapIdx(cap)] != GL_FALSE)
	{
		_bState[GetCapIdx(cap)] = GL_FALSE;
		++_profileData.glDisable.unfilteredCallCount;
		::glDisable(cap);
	}
}

inline void CToggleState::glEnableClientState(GLenum array)
{
	++_profileData.glEnableClientState.overallCallCount;
	if (_bState[GetCapIdx(array)] != GL_TRUE)
	{
		_bState[GetCapIdx(array)] = GL_TRUE;
		++_profileData.glEnableClientState.unfilteredCallCount;
		::glEnableClientState(array);
	}
}

inline void CToggleState::glDisableClientState(GLenum array)
{
	++_profileData.glDisableClientState.overallCallCount;
	if (_bState[GetCapIdx(array)] != GL_FALSE)
	{
		_bState[GetCapIdx(array)] = GL_FALSE;
		++_profileData.glDisableClientState.unfilteredCallCount;
		::glDisableClientState(array);
	}
}

inline void CToggleState::glEnableVertexAttribArrayARB(GLuint index)
{
	if (index >= _sc_uiMaxVertexAttribs)
		UNDEFINED_CAP_FATAL(GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB);

	++_profileData.glEnableVertexAttribArrayARB.overallCallCount;
	if (_bAttrib[index] != GL_TRUE)
	{
		_bAttrib[index] = GL_TRUE;
		++_profileData.glEnableVertexAttribArrayARB.unfilteredCallCount;
		::glEnableVertexAttribArrayARB(index);
	}
}

inline void CToggleState::glDisableVertexAttribArrayARB(GLuint index)
{
	if (index >= _sc_uiMaxVertexAttribs)
		UNDEFINED_CAP_FATAL(GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB);

	++_profileData.glDisableVertexAttribArrayARB.overallCallCount;
	if (_bAttrib[index] != GL_FALSE)
	{
		_bAttrib[index] = GL_FALSE;
		++_profileData.glDisableVertexAttribArrayARB.unfilteredCallCount;
		::glDisableVertexAttribArrayARB(index);
	}
}

inline void CToggleState::Set(const CToggleState &previous)
{
	for (int i = 0; i < _sc_uiStatesCount; ++i)
		if (_bState[i] != previous._bState[i] && _bState[i] != 255)
		{
			if (i < 4) //GL_X_ARRAY
			{
				if (_bState[i] == GL_TRUE)
				{
					++_profileData.glEnableClientState.unfilteredCallCount;
					++_profileData.glEnableClientState.overallCallCount;
					::glEnableClientState(GetCap(i));
				}
				else
				{
					++_profileData.glDisableClientState.unfilteredCallCount;
					++_profileData.glDisableClientState.overallCallCount;
					::glDisableClientState(GetCap(i));
				}
			}
			else
				if (_bState[i] == GL_TRUE)
				{
					++_profileData.glEnable.unfilteredCallCount;
					++_profileData.glEnable.overallCallCount;
					::glEnable(GetCap(i));
				}
				else
				{
					++_profileData.glDisable.unfilteredCallCount;
					++_profileData.glDisable.overallCallCount;
					::glDisable(GetCap(i));
				}
		}

	if (!GLEW_ARB_vertex_shader)
		return;

	for (int i = 0; i < _iMaxAttribs; ++i)
		if (_bAttrib[i] != previous._bAttrib[i] && _bAttrib[i] != 255)
		{
			if (_bAttrib[i] == GL_TRUE)
			{
				++_profileData.glEnableVertexAttribArrayARB.unfilteredCallCount;
				++_profileData.glEnableVertexAttribArrayARB.overallCallCount;
				::glEnableVertexAttribArrayARB(i);
			}
			else
			{
				++_profileData.glDisableVertexAttribArrayARB.unfilteredCallCount;
				++_profileData.glDisableVertexAttribArrayARB.overallCallCount;
				::glDisableVertexAttribArrayARB(i);
			}
		}
}

inline GLboolean CToggleState::glIsEnabled(GLenum cap) const
{
	if (_bState[GetCapIdx(cap)] != 255)
		return _bState[GetCapIdx(cap)];
	else
		return ::glIsEnabled(cap);
}

inline void CToggleState::glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const
{
	if (GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB != pname)
		UNDEFINED_CAP_FATAL(pname);

	if (index >= _sc_uiMaxVertexAttribs)
		UNDEFINED_CAP_FATAL(GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB);

	if (_bAttrib[index] != 255)
		*params = _bAttrib[index];
	else
		::glGetVertexAttribiv(index, pname, params);
}

inline void CModeState::glFrontFace(GLenum mode)
{
	++_profileData.glFrontFace.overallCallCount;
	if (_mode[0] != mode)
	{
		++_profileData.glFrontFace.unfilteredCallCount;
		::glFrontFace(_mode[0] = mode);
	}
}

inline void CModeState::glCullFace(GLenum mode)
{
	++_profileData.glCullFace.overallCallCount;
	if (_mode[1] != mode)
	{
		++_profileData.glCullFace.unfilteredCallCount;
		::glCullFace(_mode[1] = mode);
	}
}

inline void CModeState::glDepthFunc(GLenum func)
{
	++_profileData.glDepthFunc.overallCallCount;
	if (_mode[2] != func)
	{
		++_profileData.glDepthFunc.unfilteredCallCount;
		::glDepthFunc(_mode[2] = func);
	}
}

inline void CModeState::glDepthMask(GLboolean flag)
{
	++_profileData.glDepthMask.overallCallCount;
	if (_mode[3] != flag)
	{
		++_profileData.glDepthMask.unfilteredCallCount;
		::glDepthMask(_mode[3] = flag);
	}
}

inline void CModeState::glGetBooleanv(GLenum pname, GLboolean *params) const
{
	if (_mode[3] != ~0)
		*params = _mode[3]; //GL_DEPTH_WRITEMASK
	else
		::glGetBooleanv(pname, params);
}

inline void CModeState::glGetIntegerv(GLenum pname, GLint *params) const
{
	switch(pname)
	{
	case GL_FRONT_FACE:
		if (_mode[0] != ~0)
		{
			*params = _mode[0];
			return;
		}
		break;
	case GL_CULL_FACE_MODE:
		if (_mode[1] != ~0)
		{
			*params = _mode[1];
			return;
		}
		break;
	case GL_DEPTH_FUNC:
		if (_mode[2] != ~0)
		{
			*params = _mode[2];
			return;
		}
		break;
	}

	::glGetIntegerv(pname, params);
}

inline void CModeState::Set(const CModeState &previous)
{
	++_profileData.glFrontFace.overallCallCount;
	++_profileData.glCullFace.overallCallCount;
	++_profileData.glDepthFunc.overallCallCount;
	++_profileData.glDepthMask.overallCallCount;

	if (_mode[0] != previous._mode[0] && _mode[0] != ~0)
	{
		++_profileData.glFrontFace.unfilteredCallCount;
		::glFrontFace(_mode[0]);
	}

	if (_mode[1] != previous._mode[1] && _mode[1] != ~0)
	{
		++_profileData.glCullFace.unfilteredCallCount;
		::glCullFace(_mode[1]);
	}

	if (_mode[2] != previous._mode[2] && _mode[2] != ~0)
	{
		++_profileData.glDepthFunc.unfilteredCallCount;
		::glDepthFunc(_mode[2]);
	}

	if (_mode[3] != previous._mode[3] && _mode[3] != ~0)
	{
		++_profileData.glDepthMask.unfilteredCallCount;
		::glDepthMask(_mode[3]);
	}
}

inline void CBindBufferState::glBindBufferARB(GLenum target, GLuint buffer)
{
	if (target != GL_ARRAY_BUFFER_ARB && target != GL_ELEMENT_ARRAY_BUFFER_ARB)
		UNDEFINED_CAP_FATAL(target);

	++_profileData.glBindBufferARB.overallCallCount;
	if ((target == GL_ARRAY_BUFFER_ARB && _uiArrayBuffer != buffer) || (target == GL_ELEMENT_ARRAY_BUFFER_ARB && _uiElementBuffer != buffer))
	{
		if (target == GL_ARRAY_BUFFER_ARB) _uiArrayBuffer = buffer;
		else
			if (target == GL_ELEMENT_ARRAY_BUFFER_ARB) _uiElementBuffer = buffer;

		++_profileData.glBindBufferARB.unfilteredCallCount;
		::glBindBufferARB(target, buffer);
	}
}

inline void CBindBufferState::Set(const CBindBufferState &previous)
{
	if (!GLEW_ARB_vertex_buffer_object)
		return;

	_profileData.glBindBufferARB.overallCallCount += 2;


	if (_uiArrayBuffer != previous._uiArrayBuffer && _uiArrayBuffer != ~0)
	{
		++_profileData.glBindBufferARB.unfilteredCallCount;
		::glBindBufferARB(GL_ARRAY_BUFFER_ARB, _uiArrayBuffer);
	}

	if (_uiElementBuffer != previous._uiElementBuffer && _uiElementBuffer != ~0)
	{
		++_profileData.glBindBufferARB.unfilteredCallCount;
		::glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _uiElementBuffer);
	}
}

inline void CValueState::glLineWidth(GLfloat width)
{
	++_profileData.glLineWidth.overallCallCount;
	if (_fLineWidth != width)
	{
		++_profileData.glLineWidth.unfilteredCallCount;
		::glLineWidth(_fLineWidth = width);
	}
}

inline void CValueState::glPointSize(GLfloat size)
{
	++_profileData.glPointSize.overallCallCount;
	if (_fPointSize != size)
	{
		++_profileData.glPointSize.unfilteredCallCount;
		::glPointSize(_fPointSize = size);
	}
}

inline void CValueState::glGetFloatv(GLenum pname, GLfloat *params) const
{
	switch (pname)
	{
	case GL_POINT_SIZE:
		if (_fPointSize != (std::numeric_limits<float>::infinity)())
		{
			*params = _fPointSize;
			return;
		}
		break;
	case GL_LINE_WIDTH:
		if (_fLineWidth != (std::numeric_limits<float>::infinity)())
		{
			*params = _fLineWidth;
			return;
		}
		break;
	}

	::glGetFloatv(pname, params);
}

inline void CValueState::Set(const CValueState &previous)
{
	++_profileData.glLineWidth.overallCallCount;
	++_profileData.glPointSize.overallCallCount;

	if (_fLineWidth != previous._fLineWidth && _fLineWidth != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glLineWidth.unfilteredCallCount;
		::glLineWidth(_fLineWidth);
	}

	if (_fPointSize != previous._fPointSize && _fPointSize != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glPointSize.unfilteredCallCount;
		::glPointSize(_fPointSize);
	}
}

inline void CPolygonModeState::glPolygonMode(GLenum face, GLenum mode)
{
	if (face != GL_FRONT_AND_BACK)
		UNDEFINED_CAP_FATAL(face);

	++_profileData.glPolygonMode.overallCallCount;
	if (mode != _mode)
	{
		++_profileData.glPolygonMode.unfilteredCallCount;
		::glPolygonMode(face, _mode = mode);
	}
}

inline void CPolygonModeState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (_mode != ~0)
		*params = _mode; //GL_POLYGON_MODE
	else
		::glGetIntegerv(pname, params);
}

inline void CPolygonModeState::Set(const CPolygonModeState &previous)
{
	++_profileData.glPolygonMode.overallCallCount;
	
	if (_mode != previous._mode && _mode != ~0)
	{
		++_profileData.glPolygonMode.unfilteredCallCount;
		::glPolygonMode(GL_FRONT_AND_BACK, _mode);
	}
}

inline void CViewportState::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	++_profileData.glViewport.overallCallCount;
	
	if (x != _viewport[0] || y != _viewport[1] || width != _viewport[2] || height != _viewport[3])
	{
		++_profileData.glViewport.unfilteredCallCount;
		::glViewport(_viewport[0] = x, _viewport[1] = y, _viewport[2] = width, _viewport[3] = height);
	}
}

inline void CViewportState::glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	++_profileData.glScissor.overallCallCount;
	
	if (x != _scissor[0] || y != _scissor[1] || width != _scissor[2] || height != _scissor[3])
	{
		++_profileData.glScissor.unfilteredCallCount;
		::glScissor(_scissor[0] = x, _scissor[1] = y, _scissor[2] = width, _scissor[3] = height);
	}
}

inline void CViewportState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (pname == GL_VIEWPORT)
	{
		if (_viewport[0] != ~0)
		{
			params[0] = _viewport[0];
			params[1] = _viewport[1];
			params[2] = _viewport[2];
			params[3] = _viewport[3];
			return;
		}
	}
	else
		if (_scissor[0] != ~0)
		{
			params[0] = _scissor[0];
			params[1] = _scissor[1];
			params[2] = _scissor[2];
			params[3] = _scissor[3];
			return;
		}

	::glGetIntegerv(pname, params);
}

inline void CViewportState::Set(const CViewportState &previous)
{
	++_profileData.glViewport.overallCallCount;
	
	if ((_viewport[0] != previous._viewport[0] || _viewport[1] != previous._viewport[1] ||
		_viewport[2] != previous._viewport[2] || _viewport[3] != previous._viewport[3]) &&
		_viewport[0] != ~0)
	{
		++_profileData.glViewport.unfilteredCallCount;
		::glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
	}

	++_profileData.glScissor.overallCallCount;
	
	if ((_scissor[0] != previous._scissor[0] || _scissor[1] != previous._scissor[1] ||
		_scissor[2] != previous._scissor[2] || _scissor[3] != previous._scissor[3]) &&
		_scissor[0] != ~0)
	{
		++_profileData.glScissor.unfilteredCallCount;
		::glScissor(_scissor[0], _scissor[1], _scissor[2], _scissor[3]);
	}
}

inline void CMatrixState::glLoadMatrixf(const GLfloat *m)
{
	++_profileData.glLoadMatrixf.overallCallCount;
	++_profileData.glLoadMatrixf.unfilteredCallCount;

	::glLoadMatrixf(m);

	GLfloat *mat;

	switch (_mode)
	{
	case GL_PROJECTION: mat = _proj; break;
	case GL_TEXTURE: mat = _tex; break;
	case GL_MODELVIEW: mat = _mview; break;
	}

	mat[0] = m[0]; mat[1] = m[1]; mat[2] = m[2]; mat[3] = m[3];
	mat[4] = m[4]; mat[5] = m[5]; mat[6] = m[6]; mat[7] = m[7];
	mat[8] = m[8]; mat[9] = m[9]; mat[10] = m[10]; mat[11] = m[11];
	mat[12] = m[12]; mat[13] = m[13]; mat[14] = m[14]; mat[15] = m[15];
}

inline void CMatrixState::glMatrixMode(GLenum mode)
{
	++_profileData.glMatrixMode.overallCallCount;
	
	if (_mode != mode)
	{
		++_profileData.glMatrixMode.unfilteredCallCount;
		::glMatrixMode(_mode = mode);
	}
}

inline void CMatrixState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (_mode != ~0)
		*params = _mode; //GL_MATRIX_MODE 
	else
		::glGetIntegerv(pname, params);
}

inline void CMatrixState::glGetFloatv(GLenum pname, GLfloat *params) const
{
	const GLfloat *m;

	switch (pname)
	{
	case GL_PROJECTION_MATRIX:
		if (_proj[0] != (std::numeric_limits<float>::infinity)())
		{
			m = _proj;
			goto copy;
			return;
		}
		break;
	case GL_TEXTURE_MATRIX:
		if (_tex[0] != (std::numeric_limits<float>::infinity)())
		{
			m = _tex;
			goto copy;
			return;
		}
		break;
	case GL_MODELVIEW_MATRIX:
		if (_mview[0] != (std::numeric_limits<float>::infinity)())
		{
			m = _mview;
			goto copy;
			return;
		}
		break;
	}

	::glGetFloatv(pname, params);

	return;

copy:
	params[0] = m[0]; params[1] = m[1]; params[2] = m[2]; params[3] = m[3];
	params[4] = m[4]; params[5] = m[5]; params[6] = m[6]; params[7] = m[7];
	params[8] = m[8]; params[9] = m[9]; params[10] = m[10]; params[11] = m[11];
	params[12] = m[12]; params[13] = m[13]; params[14] = m[14]; params[15] = m[15];
}

inline void CMatrixState::Set(const CMatrixState &previous)
{
	_profileData.glLoadMatrixf.overallCallCount += 3;
	_profileData.glMatrixMode.overallCallCount += 4;
	
	if (_tex[0] != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glMatrixMode.unfilteredCallCount;
		++_profileData.glLoadMatrixf.unfilteredCallCount;
		::glMatrixMode(GL_TEXTURE);
		::glLoadMatrixf(_tex);
	}

	if (_proj[0] != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glMatrixMode.unfilteredCallCount;
		++_profileData.glLoadMatrixf.unfilteredCallCount;
		::glMatrixMode(GL_PROJECTION);
		::glLoadMatrixf(_proj);
	}

	if (_mview[0] != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glMatrixMode.unfilteredCallCount;
		++_profileData.glLoadMatrixf.unfilteredCallCount;
		::glMatrixMode(GL_MODELVIEW);
		::glLoadMatrixf(_mview);
	}
	
	if (_mode != GL_MODELVIEW && _mode != ~0)
	{
		++_profileData.glMatrixMode.unfilteredCallCount;
		::glMatrixMode(_mode);
	}
}

inline void CBlendFuncState::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	++_profileData.glBlendFunc.overallCallCount;
	if (sfactor != _srcFactor || dfactor != _dstFactor)
	{
		++_profileData.glBlendFunc.unfilteredCallCount;
		::glBlendFunc(_srcFactor = sfactor, _dstFactor = dfactor);
	}
}

inline void CBlendFuncState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (pname == GL_BLEND_DST)
	{
		if (_dstFactor != ~0)
		{
			*params = _dstFactor;
			return;
		}
	}
	else //GL_BLEND_SRC
		if (_srcFactor != ~0)
		{
			*params = _srcFactor;
			return;
		}

	::glGetIntegerv(pname, params);
}

inline void CBlendFuncState::Set(const CBlendFuncState &previous)
{
	++_profileData.glBlendFunc.overallCallCount;

	if ((_srcFactor != previous._srcFactor || _dstFactor != previous._dstFactor) && (_srcFactor != ~0 && _dstFactor != ~0))
	{
		++_profileData.glBlendFunc.unfilteredCallCount;
		::glBlendFunc(_srcFactor, _dstFactor);
	}
}

inline void CAlphaFuncState::glAlphaFunc(GLenum func, GLclampf ref)
{
	++_profileData.glAlphaFunc.overallCallCount;
	if (func != _func || ref != _ref)
	{
		++_profileData.glAlphaFunc.unfilteredCallCount;
		::glAlphaFunc(_func = func, _ref = ref);
	}
}

inline void CAlphaFuncState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (_func != ~0)
		*params = _func; //GL_ALPHA_TEST_FUNC
	else
		::glGetIntegerv(pname, params);
}

inline void CAlphaFuncState::glGetFloatv(GLenum pname, GLfloat *params) const
{
	if (_ref != (std::numeric_limits<float>::infinity)())
		*params = _ref; //GL_ALPHA_TEST_REF
	else
		::glGetFloatv(pname, params);
}

inline void CAlphaFuncState::Set(const CAlphaFuncState &previous)
{
	++_profileData.glAlphaFunc.overallCallCount;

	if ((_func != previous._func || _ref != previous._ref) && (_func != ~0 && _ref != (std::numeric_limits<float>::infinity)()))
	{
		++_profileData.glAlphaFunc.unfilteredCallCount;
		::glAlphaFunc(_func, _ref);
	}
}

inline void CColorState::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	++_profileData.glColor4f.overallCallCount;
	if (red != _color[0] || green != _color[1] || blue != _color[2] || alpha != _color[3])
	{
		++_profileData.glColor4f.unfilteredCallCount;
		::glColor4f(_color[0] = red, _color[1] = green, _color[2] = blue, _color[3] = alpha);
	}
}

inline void CColorState::glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	++_profileData.glClearColor.overallCallCount;
	if (red != _clearColor[0] || green != _clearColor[1] || blue != _clearColor[2] || alpha != _clearColor[3])
	{
		++_profileData.glClearColor.unfilteredCallCount;
		::glClearColor(_clearColor[0] = red, _clearColor[1] = green, _clearColor[2] = blue, _clearColor[3] = alpha);
	}
}

inline void CColorState::glGetFloatv(GLenum pname, GLfloat *params) const
{
	if (pname == GL_COLOR_CLEAR_VALUE)
	{
		if (_clearColor[0] != (std::numeric_limits<float>::infinity)())
		{
			params[0] = _clearColor[0];
			params[1] = _clearColor[1];
			params[2] = _clearColor[2];
			params[3] = _clearColor[3];
			return;
		}
	}
	else //GL_CURRENT_COLOR
		if (_color[0] != (std::numeric_limits<float>::infinity)())
		{
			params[0] = _color[0];
			params[1] = _color[1];
			params[2] = _color[2];
			params[3] = _color[3];
			return;
		}

	::glGetFloatv(pname, params);
}

inline void CColorState::Set(const CColorState &previous)
{
	++_profileData.glColor4f.overallCallCount;

	if ((_color[3] != previous._color[3] || _color[2] != previous._color[2] ||
		_color[1] != previous._color[1] || _color[0] != previous._color[0]) &&
		_color[0] != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glColor4f.unfilteredCallCount;
		::glColor4f(_color[0], _color[1], _color[2], _color[3]);
	}

	++_profileData.glClearColor.overallCallCount;

	if ((_clearColor[0] != previous._clearColor[0] || _clearColor[1] != previous._clearColor[1] ||
		_clearColor[2] != previous._clearColor[2] || _clearColor[3] != previous._clearColor[3]) &&
		_clearColor[0] != (std::numeric_limits<float>::infinity)())
	{
		++_profileData.glClearColor.unfilteredCallCount;
		::glClearColor(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]);
	}
}

inline void CTextureState::glBindTexture(GLenum target, GLuint texture)
{
	if (target != GL_TEXTURE_2D)
		UNDEFINED_CAP_FATAL(target)

	++_profileData.glBindTexture.overallCallCount;

	if (_textureUnit == ~0)
	{
		if (GLEW_ARB_multitexture || GLEW_ARB_shading_language_100)
			::glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, (GLint *)&_textureUnit);
		else
			_textureUnit = GL_TEXTURE0_ARB;
	}

	TTexture &cur_texture =_textures[_textureUnit - GL_TEXTURE0_ARB];

	if (texture != cur_texture.texture)
	{
		++_profileData.glBindTexture.unfilteredCallCount;
		::glBindTexture(target, cur_texture.texture = texture);
		
		if (!GLEW_ARB_shading_language_100 && _textureUnit != GL_TEXTURE0_ARB)
		{
			if (texture != 0 && !cur_texture.enabled)
			{
				cur_texture.enabled = true;
				::glEnable(GL_TEXTURE_2D);
			}
			else
				if (texture == 0 && cur_texture.enabled)
				{
					cur_texture.enabled = false;
					::glDisable(GL_TEXTURE_2D);
				}
		}
		else
			cur_texture.enabled = texture != 0;
	}
}

inline void CTextureState::glActiveTextureARB(GLenum texture)
{
	++_profileData.glActiveTextureARB.overallCallCount;

	if (texture != _textureUnit)
	{
		++_profileData.glActiveTextureARB.unfilteredCallCount;
		::glActiveTextureARB(_textureUnit = texture);
	}
}

inline void CTextureState::Set(const CTextureState &previous)
{
	for (int i = (int)_textures.size() - 1; i > -1; --i)
		if (!(_textures[i].texture == ~0 || (_textures[i].enabled == previous._textures[i].enabled && _textures[i].texture == previous._textures[i].texture)))
		{
			glActiveTextureARB(GL_TEXTURE0_ARB + i);

			if (!GLEW_ARB_shading_language_100 && i != 0)
			{
				if (_textures[i].enabled)
					::glEnable(GL_TEXTURE_2D);
				else
					::glDisable(GL_TEXTURE_2D);
			}

			::glBindTexture(GL_TEXTURE_2D, _textures[i].texture);
		}
}

inline void CTextureState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (pname == GL_TEXTURE_2D_BINDING_EXT)
	{
		if (_textures[_textureUnit - GL_TEXTURE0_ARB].texture != ~0)
		{
			*params = _textures[_textureUnit - GL_TEXTURE0_ARB].texture;
			return;
		}
	}
	else //GL_ACTIVE_TEXTURE_ARB
		if (_textureUnit != ~0)
		{
			*params = _textureUnit;
			return;
		}

	::glGetIntegerv(pname, params);
}

inline void CProgramState::glUseProgramObjectARB(GLhandleARB programObj)
{
	++_profileData.glUseProgramObjectARB.overallCallCount;
	if (programObj != _programObj)
	{
		++_profileData.glUseProgramObjectARB.unfilteredCallCount;
		::glUseProgramObjectARB(_programObj = programObj);
	}
}

inline void CProgramState::glGetIntegerv(GLenum pname, GLint *params) const
{
	if (_programObj != ~0)
		*params = _programObj; //GL_CURRENT_PROGRAM
	else
		::glGetIntegerv(pname, params);
}

inline void CProgramState::Set(const CProgramState &previous)
{
	if (GLEW_ARB_shader_objects)
	{
		++_profileData.glUseProgramObjectARB.overallCallCount;
		
		if (_programObj != previous._programObj && _programObj != ~0)
		{
			++_profileData.glUseProgramObjectARB.unfilteredCallCount;
			::glUseProgramObjectARB(_programObj);
		}
	}
}

void CStateManager<false>::Push()
{
	::glPushAttrib(GL_ALL_ATTRIB_BITS);
	::glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
}

void CStateManager<false>::Pop()
{
	::glPopAttrib();
	::glPopClientAttrib();
}

void CStateManager<false>::glEnable(GLenum cap)
{
	::glEnable(cap);
}

void CStateManager<false>::glDisable(GLenum cap)
{
	::glDisable(cap);
}

void CStateManager<false>::glEnableClientState(GLenum array)
{
	::glEnableClientState(array);
}

void CStateManager<false>::glDisableClientState(GLenum array)
{
	::glDisableClientState(array);
}

void CStateManager<false>::glEnableVertexAttribArrayARB(GLuint index)
{
	::glEnableVertexAttribArrayARB(index);
}

void CStateManager<false>::glDisableVertexAttribArrayARB(GLuint index)
{
	::glDisableVertexAttribArrayARB(index);
}

void CStateManager<false>::glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const
{
	::glGetVertexAttribiv(index, pname, params);
}

GLboolean CStateManager<false>::glIsEnabled(GLenum cap) const
{
	return ::glIsEnabled(cap);
}

void CStateManager<false>::glGetIntegerv(GLenum pname, GLint *params) const
{
	::glGetIntegerv(pname, params);
}

void CStateManager<false>::glGetFloatv(GLenum pname, GLfloat *params) const
{
	::glGetFloatv(pname, params);
}

void CStateManager<false>::glGetBooleanv(GLenum pname, GLboolean *params) const
{
	::glGetBooleanv(pname, params);
}

void CStateManager<false>::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	::glBlendFunc(sfactor, dfactor);
}

void CStateManager<false>::glAlphaFunc(GLenum func, GLclampf ref)
{
	::glAlphaFunc(func, ref);
}

void CStateManager<false>::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	::glColor4f(red, green, blue, alpha);
}

void CStateManager<false>::glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	::glClearColor(red, green, blue, alpha);
}

void CStateManager<false>::glBindTexture(GLenum target, GLuint texture)
{
	::glBindTexture(target, texture);
}

void CStateManager<false>::glActiveTextureARB(GLenum texture)
{
	::glActiveTextureARB(texture);
	
	// This is not very good solution but for compatibility with cached state manager.
	if (!GLEW_ARB_shading_language_100 && texture != GL_TEXTURE0_ARB) ::glEnable(GL_TEXTURE_2D);
}

void CStateManager<false>::glLoadMatrixf(const GLfloat *m)
{
	::glLoadMatrixf(m);
}

void CStateManager<false>::glMatrixMode(GLenum mode)
{
	::glMatrixMode(mode);
}

void CStateManager<false>::glUseProgramObjectARB(GLhandleARB programObj)
{
	::glUseProgramObjectARB(programObj);
}

void CStateManager<false>::glFrontFace(GLenum mode)
{
	::glFrontFace(mode);
}

void CStateManager<false>::glCullFace(GLenum mode)
{
	::glCullFace(mode);
}

void CStateManager<false>::glPolygonMode(GLenum face, GLenum mode)
{
	::glPolygonMode(face, mode);
}

void CStateManager<false>::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	::glViewport(x, y, width, height);
}

void CStateManager<false>::glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	::glScissor(x, y, width, height);
}

void CStateManager<false>::glDepthFunc(GLenum func)
{
	::glDepthFunc(func);
}

void CStateManager<false>::glDepthMask(GLboolean flag)
{
	::glDepthMask(flag);
}

void CStateManager<false>::glLineWidth(GLfloat width)
{
	::glLineWidth(width);
}

void CStateManager<false>::glPointSize(GLfloat size)
{
	::glPointSize(size);
}

void CStateManager<false>::glBindBufferARB(GLenum target, GLuint buffer)
{
	::glBindBufferARB(target, buffer);
}

void CStateManager<false>::ResetProfileData()
{}

void CStateManager<false>::OutputProfileData()
{
	Core()->RenderProfilerText("No data available.", ColorWhite());
}

void CStateManager<false>::OutputProfileSummary()
{
	Core()->RenderProfilerText("Filtering is disabled.", ColorWhite());
}

void CStateManager<true>::Push()
{
	_stateStack.push(_curState);
}

void CStateManager<true>::Pop()
{
	TState prev(_curState);

	_curState = _stateStack.top();
	_stateStack.pop();

	_curState.toggleState.Set(prev.toggleState);
	_curState.blendFuncState.Set(prev.blendFuncState);
	_curState.alphaFuncState.Set(prev.alphaFuncState);
	_curState.colorState.Set(prev.colorState);
	_curState.textureState.Set(prev.textureState);
	_curState.programState.Set(prev.programState);
	_curState.polygonModeState.Set(prev.polygonModeState);
	_curState.viewportState.Set(prev.viewportState);
	_curState.modeState.Set(prev.modeState);
	_curState.bindBufferState.Set(prev.bindBufferState);
	_curState.valueState.Set(prev.valueState);
}

void CStateManager<true>::glEnable(GLenum cap)
{
	_curState.toggleState.glEnable(cap);
}

void CStateManager<true>::glDisable(GLenum cap)
{
	_curState.toggleState.glDisable(cap);
}

void CStateManager<true>::glEnableClientState(GLenum array)
{
	_curState.toggleState.glEnableClientState(array);
}

void CStateManager<true>::glDisableClientState(GLenum array)
{
	_curState.toggleState.glDisableClientState(array);
}

void CStateManager<true>::glEnableVertexAttribArrayARB(GLuint index)
{
	_curState.toggleState.glEnableVertexAttribArrayARB(index);
}

void CStateManager<true>::glDisableVertexAttribArrayARB(GLuint index)
{
	_curState.toggleState.glDisableVertexAttribArrayARB(index);
}

void CStateManager<true>::glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const
{
	_curState.toggleState.glGetVertexAttribiv(index, pname, params);
}

GLboolean CStateManager<true>::glIsEnabled(GLenum cap) const
{
	return _curState.toggleState.glIsEnabled(cap);
}

void CStateManager<true>::glGetIntegerv(GLenum pname, GLint *params) const
{
	switch(pname)
	{
	case GL_FRONT_FACE:
	case GL_CULL_FACE_MODE:
	case GL_DEPTH_FUNC:
		_curState.modeState.glGetIntegerv(pname, params);
		break;
	case GL_POLYGON_MODE:
		_curState.polygonModeState.glGetIntegerv(pname, params);
		break;
	case GL_BLEND_SRC:
	case GL_BLEND_DST:
		_curState.blendFuncState.glGetIntegerv(pname, params);
		break;
	case GL_ALPHA_TEST_FUNC:
		_curState.alphaFuncState.glGetIntegerv(pname, params);
		break;
	case GL_VIEWPORT:
	case GL_SCISSOR_BOX:
		_curState.viewportState.glGetIntegerv(pname, params);
		break;
	case GL_MATRIX_MODE:
		_curState.matrixState.glGetIntegerv(pname, params);
		break;
	case GL_TEXTURE_2D_BINDING_EXT:
	case GL_ACTIVE_TEXTURE:
		_curState.textureState.glGetIntegerv(pname, params);
		break;
	case GL_CURRENT_PROGRAM:
		_curState.programState.glGetIntegerv(pname, params);
		break;
	default:
		UNDEFINED_CAP_FATAL(pname)
	}
}

void CStateManager<true>::glGetFloatv(GLenum pname, GLfloat *params) const
{
	switch(pname)
	{
	case GL_ALPHA_TEST_REF:
		_curState.alphaFuncState.glGetFloatv(pname, params);
		break;
	case GL_LINE_WIDTH:
	case GL_POINT_SIZE:
		_curState.valueState.glGetFloatv(pname, params);
		break;
	case GL_CURRENT_COLOR:
	case GL_COLOR_CLEAR_VALUE:
		_curState.colorState.glGetFloatv(pname, params);
		break;
	case GL_PROJECTION_MATRIX:
	case GL_TEXTURE_MATRIX:
	case GL_MODELVIEW_MATRIX:
		_curState.matrixState.glGetFloatv(pname, params);
		break;
	default:
		UNDEFINED_CAP_FATAL(pname)
	}
}

void CStateManager<true>::glGetBooleanv(GLenum pname, GLboolean *params) const
{
	switch(pname)
	{
	case GL_DEPTH_WRITEMASK:
		_curState.modeState.glGetBooleanv(pname, params);
		break;
	default:
		UNDEFINED_CAP_FATAL(pname)
	}
}

void CStateManager<true>::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	_curState.blendFuncState.glBlendFunc(sfactor, dfactor);
}

void CStateManager<true>::glAlphaFunc(GLenum func, GLclampf ref)
{
	_curState.alphaFuncState.glAlphaFunc(func, ref);
}

void CStateManager<true>::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	_curState.colorState.glColor4f(red, green, blue, alpha);
}

void CStateManager<true>::glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	_curState.colorState.glClearColor(red, green, blue, alpha);
}

void CStateManager<true>::glBindTexture(GLenum target, GLuint texture)
{
	_curState.textureState.glBindTexture(target, texture);
}

void CStateManager<true>::glActiveTextureARB(GLenum texture)
{
	_curState.textureState.glActiveTextureARB(texture);
}

void CStateManager<true>::glLoadMatrixf(const GLfloat *m)
{
	_curState.matrixState.glLoadMatrixf(m);
}

void CStateManager<true>::glMatrixMode(GLenum mode)
{
	_curState.matrixState.glMatrixMode(mode);
}

void CStateManager<true>::glUseProgramObjectARB(GLhandleARB programObj)
{
	_curState.programState.glUseProgramObjectARB(programObj);
}

void CStateManager<true>::glFrontFace(GLenum mode)
{
	_curState.modeState.glFrontFace(mode);
}

void CStateManager<true>::glCullFace(GLenum mode)
{
	_curState.modeState.glCullFace(mode);
}

void CStateManager<true>::glPolygonMode(GLenum face, GLenum mode)
{
	_curState.polygonModeState.glPolygonMode(face, mode);
}

void CStateManager<true>::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	_curState.viewportState.glViewport(x, y, width, height);
}

void CStateManager<true>::glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	_curState.viewportState.glScissor(x, y, width, height);
}

void CStateManager<true>::glDepthFunc(GLenum func)
{
	_curState.modeState.glDepthFunc(func);
}

void CStateManager<true>::glDepthMask(GLboolean flag)
{
	_curState.modeState.glDepthMask(flag);
}

void CStateManager<true>::ResetProfileData()
{
	_profileData.~TProfileData();
	new(&_profileData) TProfileData;
}

void CStateManager<true>::glLineWidth(GLfloat width)
{
	_curState.valueState.glLineWidth(width);
}

void CStateManager<true>::glPointSize(GLfloat size)
{
	_curState.valueState.glPointSize(size);
}

void CStateManager<true>::glBindBufferARB(GLenum target, GLuint buffer)
{
	_curState.bindBufferState.glBindBufferARB(target, buffer);
}

void CStateManager<true>::_OutputFunctionProfileData(const string &functionName, const ::TProfileData &profileData)
{
	string str(functionName + UIntToStr(profileData.unfilteredCallCount) + '/' + UIntToStr(profileData.overallCallCount));
	if (profileData.overallCallCount)
		str.append(" (").append(UIntToStr((profileData.overallCallCount - profileData.unfilteredCallCount) * 100 / profileData.overallCallCount)).append("% redundant)");
	Core()->RenderProfilerText(str.c_str(), ColorWhite());
}

void CStateManager<true>::OutputProfileData()
{
	Core()->RenderProfilerText("OpenGL functions calls (unfiltered/overall)", ColorWhite());
	_OutputFunctionProfileData("glEnable...................", _profileData.toggleStateProfileData.glEnable);
	_OutputFunctionProfileData("glDisable..................", _profileData.toggleStateProfileData.glDisable);
	_OutputFunctionProfileData("glEnableClientState........", _profileData.toggleStateProfileData.glEnableClientState);
	_OutputFunctionProfileData("glDisableClientState.......", _profileData.toggleStateProfileData.glDisableClientState);
	_OutputFunctionProfileData("glEnableVertexAttribArray..", _profileData.toggleStateProfileData.glEnableVertexAttribArrayARB);
	_OutputFunctionProfileData("glDisableVertexAttribArray.", _profileData.toggleStateProfileData.glDisableVertexAttribArrayARB);
	_OutputFunctionProfileData("glBlendFunc................", _profileData.blendFuncStateProfileData.glBlendFunc);
	_OutputFunctionProfileData("glAlphaFunc................", _profileData.alphaFuncStateProfileData.glAlphaFunc);
	_OutputFunctionProfileData("glColor4f..................", _profileData.colorStateProfileData.glColor4f);
	_OutputFunctionProfileData("glClearColor...............", _profileData.colorStateProfileData.glClearColor);
	_OutputFunctionProfileData("glBindBuffer...............", _profileData.bindBufferStateProfilerData.glBindBufferARB);
	_OutputFunctionProfileData("glBindTexture..............", _profileData.textureStateProfileData.glBindTexture);
	_OutputFunctionProfileData("glActiveTexture............", _profileData.textureStateProfileData.glActiveTextureARB);
	_OutputFunctionProfileData("glUseProgramObject.........", _profileData.programStateProfileData.glUseProgramObjectARB);
	_OutputFunctionProfileData("glLoadMatrixf..............", _profileData.matrxStateProfilerData.glLoadMatrixf);
	_OutputFunctionProfileData("glMatrixMode...............", _profileData.matrxStateProfilerData.glMatrixMode);	
	_OutputFunctionProfileData("glPolygonMode..............", _profileData.polygonModeStateProfileData.glPolygonMode);
	_OutputFunctionProfileData("glCullFace.................", _profileData.modeStateProfilerData.glCullFace);
	_OutputFunctionProfileData("glFrontFace................", _profileData.modeStateProfilerData.glFrontFace);
	_OutputFunctionProfileData("glDepthFunc................", _profileData.modeStateProfilerData.glDepthFunc);
	_OutputFunctionProfileData("glDepthMask................", _profileData.modeStateProfilerData.glDepthMask);
	_OutputFunctionProfileData("glViewport.................", _profileData.viewportStateProfileData.glViewport);
	_OutputFunctionProfileData("glScissor..................", _profileData.viewportStateProfileData.glScissor);
	_OutputFunctionProfileData("glLineWidth................", _profileData.valueStateProfilerData.glLineWidth);
	_OutputFunctionProfileData("glPointSize................", _profileData.valueStateProfilerData.glPointSize);
}

void CStateManager<true>::OutputProfileSummary()
{
	::TProfileData summary;

	summary.unfilteredCallCount =
		_profileData.toggleStateProfileData.glEnable.unfilteredCallCount +
		_profileData.toggleStateProfileData.glDisable.unfilteredCallCount +
		_profileData.toggleStateProfileData.glEnableClientState.unfilteredCallCount +
		_profileData.toggleStateProfileData.glDisableClientState.unfilteredCallCount +
		_profileData.toggleStateProfileData.glEnableVertexAttribArrayARB.unfilteredCallCount +
		_profileData.toggleStateProfileData.glDisableVertexAttribArrayARB.unfilteredCallCount +
		_profileData.blendFuncStateProfileData.glBlendFunc.unfilteredCallCount +
		_profileData.alphaFuncStateProfileData.glAlphaFunc.unfilteredCallCount +
		_profileData.colorStateProfileData.glColor4f.unfilteredCallCount +
		_profileData.colorStateProfileData.glClearColor.unfilteredCallCount +
		_profileData.textureStateProfileData.glBindTexture.unfilteredCallCount +
		_profileData.textureStateProfileData.glActiveTextureARB.unfilteredCallCount +
		_profileData.programStateProfileData.glUseProgramObjectARB.unfilteredCallCount +
		_profileData.polygonModeStateProfileData.glPolygonMode.unfilteredCallCount +
		_profileData.viewportStateProfileData.glViewport.unfilteredCallCount +
		_profileData.viewportStateProfileData.glScissor.unfilteredCallCount +
		_profileData.matrxStateProfilerData.glLoadMatrixf.unfilteredCallCount +
		_profileData.matrxStateProfilerData.glMatrixMode.unfilteredCallCount +
		_profileData.modeStateProfilerData.glCullFace.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glFrontFace.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glDepthFunc.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glDepthMask.unfilteredCallCount +
		_profileData.valueStateProfilerData.glLineWidth.unfilteredCallCount +
		_profileData.valueStateProfilerData.glPointSize.unfilteredCallCount +
		_profileData.bindBufferStateProfilerData.glBindBufferARB.unfilteredCallCount;

	summary.overallCallCount =
		_profileData.toggleStateProfileData.glEnable.overallCallCount +
		_profileData.toggleStateProfileData.glDisable.overallCallCount +
		_profileData.toggleStateProfileData.glEnableClientState.overallCallCount +
		_profileData.toggleStateProfileData.glDisableClientState.overallCallCount +
		_profileData.toggleStateProfileData.glEnableVertexAttribArrayARB.overallCallCount +
		_profileData.toggleStateProfileData.glDisableVertexAttribArrayARB.overallCallCount +
		_profileData.blendFuncStateProfileData.glBlendFunc.overallCallCount +
		_profileData.alphaFuncStateProfileData.glAlphaFunc.overallCallCount +
		_profileData.colorStateProfileData.glColor4f.overallCallCount +
		_profileData.colorStateProfileData.glClearColor.overallCallCount +
		_profileData.textureStateProfileData.glBindTexture.overallCallCount +
		_profileData.textureStateProfileData.glActiveTextureARB.overallCallCount +
		_profileData.programStateProfileData.glUseProgramObjectARB.overallCallCount +
		_profileData.polygonModeStateProfileData.glPolygonMode.overallCallCount +
		_profileData.viewportStateProfileData.glViewport.overallCallCount +
		_profileData.matrxStateProfilerData.glLoadMatrixf.overallCallCount +
		_profileData.matrxStateProfilerData.glMatrixMode.overallCallCount +
		_profileData.viewportStateProfileData.glScissor.overallCallCount +
		_profileData.modeStateProfilerData.glCullFace.overallCallCount + 
		_profileData.modeStateProfilerData.glFrontFace.overallCallCount + 
		_profileData.modeStateProfilerData.glDepthFunc.overallCallCount + 
		_profileData.modeStateProfilerData.glDepthMask.overallCallCount +
		_profileData.valueStateProfilerData.glLineWidth.overallCallCount +
		_profileData.valueStateProfilerData.glPointSize.overallCallCount +
		_profileData.bindBufferStateProfilerData.glBindBufferARB.overallCallCount;

	_OutputFunctionProfileData("Overall OpenGL functions calls (unfiltered/overall):", summary);
}

#endif