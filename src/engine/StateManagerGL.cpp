/**
\author		Alexey Shaydurov aka ASH
\date		05.05.2012 (c)Andrey Korotkov

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
		case GL_POINT_SMOOTH: idx = 13; break;
		case GL_LINE_SMOOTH: idx = 14; break;
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
		case 13: cap = GL_POINT_SMOOTH; break;
		case 14: cap = GL_LINE_SMOOTH; break;
		default:
			UNDEFINED_CAP_FATAL(idx)
	}

	return cap;
}

inline void CToggleState::glEnable(GLenum cap)
{
	++_profileData.glEnableProfileData.overallCallCount;
	if (_bState[GetCapIdx(cap)] != GL_TRUE)
	{
		_bState[GetCapIdx(cap)] = GL_TRUE;
		++_profileData.glEnableProfileData.unfilteredCallCount;
		::glEnable(cap);
	}
}

inline void CToggleState::glDisable(GLenum cap)
{
	++_profileData.glDisableProfileData.overallCallCount;
	if (_bState[GetCapIdx(cap)] != GL_FALSE)
	{
		_bState[GetCapIdx(cap)] = GL_FALSE;
		++_profileData.glDisableProfileData.unfilteredCallCount;
		::glDisable(cap);
	}
}

inline void CToggleState::glEnableClientState(GLenum array)
{
	++_profileData.glEnableClientStateProfileData.overallCallCount;
	if (_bState[GetCapIdx(array)] != GL_TRUE)
	{
		_bState[GetCapIdx(array)] = GL_TRUE;
		++_profileData.glEnableClientStateProfileData.unfilteredCallCount;
		::glEnableClientState(array);
	}
}

inline void CToggleState::glDisableClientState(GLenum array)
{
	++_profileData.glDisableClientStateProfileData.overallCallCount;
	if (_bState[GetCapIdx(array)] != GL_FALSE)
	{
		_bState[GetCapIdx(array)] = GL_FALSE;
		++_profileData.glDisableClientStateProfileData.unfilteredCallCount;
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
	{
		if (_bState[i] == previous._bState[i] || _bState[i] == -1) continue;

		if (i < 4) //GL_X_ARRAY
		{
			if (_bState[i] == GL_TRUE)
			{
				++_profileData.glEnableClientStateProfileData.unfilteredCallCount;
				++_profileData.glEnableClientStateProfileData.overallCallCount;
				::glEnableClientState(GetCap(i));
			}
			else
			{
				++_profileData.glDisableClientStateProfileData.unfilteredCallCount;
				++_profileData.glDisableClientStateProfileData.overallCallCount;
				::glDisableClientState(GetCap(i));
			}
		}
		else
			if (_bState[i] == GL_TRUE)
			{
				++_profileData.glEnableProfileData.unfilteredCallCount;
				++_profileData.glEnableProfileData.overallCallCount;
				::glEnable(GetCap(i));
			}
			else
			{
				++_profileData.glDisableProfileData.unfilteredCallCount;
				++_profileData.glDisableProfileData.overallCallCount;
				::glDisable(GetCap(i));
			}
	}

	if (!GLEW_ARB_vertex_shader)
		return;

	for (int i = 0; i < _iMaxAttribs; ++i)
		if (_bAttrib[i] != previous._bAttrib[i])
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
	return _bState[GetCapIdx(cap)];
}

inline void CToggleState::glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) const
{
	if (GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB != pname)
		UNDEFINED_CAP_FATAL(pname);

	if (index >= _sc_uiMaxVertexAttribs)
		UNDEFINED_CAP_FATAL(GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB);

	GLint val = _bAttrib[index];

	memcpy(params, &val, sizeof(GLint));
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
	GLboolean result = _mode[3]; //GL_DEPTH_WRITEMASK
	memcpy(params, &result, sizeof(GLboolean));
}

inline void CModeState::glGetIntegerv(GLenum pname, GLint *params) const
{
	GLint result;

	switch(pname)
	{
	case GL_FRONT_FACE: result = _mode[0]; break;
	case GL_CULL_FACE_MODE: result = _mode[1]; break;
	case GL_DEPTH_FUNC: result = _mode[2]; break;
	}

	memcpy(params, &result, sizeof(GLint));
}

inline void CModeState::Set(const CModeState &previous)
{
	++_profileData.glFrontFace.overallCallCount;
	++_profileData.glCullFace.overallCallCount;
	++_profileData.glDepthFunc.overallCallCount;
	++_profileData.glDepthMask.overallCallCount;

	if (_mode[0] != previous._mode[0])
	{
		++_profileData.glFrontFace.unfilteredCallCount;
		::glFrontFace(_mode[0]);
	}

	if (_mode[1] != previous._mode[1])
	{
		++_profileData.glCullFace.unfilteredCallCount;
		::glCullFace(_mode[1]);
	}

	if (_mode[2] != previous._mode[2])
	{
		++_profileData.glDepthFunc.unfilteredCallCount;
		::glDepthFunc(_mode[2]);
	}

	if (_mode[3] != previous._mode[3])
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


	if (_uiArrayBuffer != previous._uiArrayBuffer)
	{
		++_profileData.glBindBufferARB.unfilteredCallCount;
		::glBindBufferARB(GL_ARRAY_BUFFER_ARB, _uiArrayBuffer);
	}

	if (_uiElementBuffer != previous._uiElementBuffer)
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

inline void CValueState::Set(const CValueState &previous)
{
	++_profileData.glLineWidth.overallCallCount;
	++_profileData.glPointSize.overallCallCount;

	if (_fLineWidth != previous._fLineWidth)
	{
		++_profileData.glLineWidth.unfilteredCallCount;
		::glLineWidth(_fLineWidth);
	}

	if (_fPointSize != previous._fPointSize)
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
	GLint result = _mode; //GL_POLYGON_MODE
	memcpy(params, &result, sizeof(GLint));
}

inline void CPolygonModeState::Set(const CPolygonModeState &previous)
{
	++_profileData.glPolygonMode.overallCallCount;
	
	if (_mode != previous._mode)
	{
		++_profileData.glPolygonMode.unfilteredCallCount;
		::glPolygonMode(GL_FRONT_AND_BACK, _mode);
	}
}

inline void CBlendFuncState::glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	++_profileData.glBlendFuncProfileData.overallCallCount;
	if (sfactor != _srcFactor || dfactor != _dstFactor)
	{
		++_profileData.glBlendFuncProfileData.unfilteredCallCount;
		::glBlendFunc(_srcFactor = sfactor, _dstFactor = dfactor);
	}
}

inline void CBlendFuncState::glGetIntegerv(GLenum pname, GLint *params) const
{
	GLint result = _srcFactor; //GL_BLEND_SRC

	if (pname == GL_BLEND_DST) result = _dstFactor;

	memcpy(params, &result, sizeof(GLint));
}

inline void CBlendFuncState::Set(const CBlendFuncState &previous)
{
	++_profileData.glBlendFuncProfileData.overallCallCount;

	if (_srcFactor != previous._srcFactor || _dstFactor != previous._dstFactor)
	{
		++_profileData.glBlendFuncProfileData.unfilteredCallCount;
		::glBlendFunc(_srcFactor, _dstFactor);
	}
}

inline void CAlphaFuncState::glAlphaFunc(GLenum func, GLclampf ref)
{
	++_profileData.glAlphaFuncProfileData.overallCallCount;
	if (func != _func || ref != _ref)
	{
		++_profileData.glAlphaFuncProfileData.unfilteredCallCount;
		::glAlphaFunc(_func = func, _ref = ref);
	}
}

inline void CAlphaFuncState::glGetIntegerv(GLenum pname, GLint *params) const
{
	GLint result = _func; //GL_ALPHA_TEST_FUNC
	memcpy(params, &result, sizeof(GLint));
}

inline void CAlphaFuncState::glGetFloatv(GLenum pname, GLfloat *params) const
{
	GLfloat result = _ref; //GL_ALPHA_TEST_REF
	memcpy(params, &result, sizeof(GLfloat));
}

inline void CAlphaFuncState::Set(const CAlphaFuncState &previous)
{
	++_profileData.glAlphaFuncProfileData.overallCallCount;

	if (_func != previous._func || _ref != previous._ref)
	{
		++_profileData.glAlphaFuncProfileData.unfilteredCallCount;
		::glAlphaFunc(_func, _ref);
	}
}

inline void CColorState::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	++_profileData.glColor4fProfileData.overallCallCount;
	if (red != _red || green != _green || blue != _blue || alpha != _alpha)
	{
		++_profileData.glColor4fProfileData.unfilteredCallCount;
		::glColor4f(_red = red, _green = green, _blue = blue, _alpha = alpha);
	}
}

inline void CColorState::Set(const CColorState &previous)
{
	++_profileData.glColor4fProfileData.overallCallCount;

	if (_alpha != previous._alpha || _red != previous._red || _green != previous._green || _blue != previous._blue)
	{
		++_profileData.glColor4fProfileData.unfilteredCallCount;
		::glColor4f(_red, _green, _blue, _alpha);
	}
}

inline void CTextureState::glBindTexture(GLenum target, GLuint texture)
{
	if (target != GL_TEXTURE_2D)
		UNDEFINED_CAP_FATAL(target)

	++_profileData.glBindTextureProfileData.overallCallCount;

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
		++_profileData.glBindTextureProfileData.unfilteredCallCount;
		::glBindTexture(target, cur_texture.texture = texture);
		
		if (!GLEW_ARB_shading_language_100 && _textureUnit != GL_TEXTURE0_ARB)
		{
			if (texture != 0 && !_textures[_textureUnit - GL_TEXTURE0_ARB].enabled)
			{
				_textures[_textureUnit - GL_TEXTURE0_ARB].enabled = true;
				::glEnable(GL_TEXTURE_2D);
			}
			else
				if (texture == 0 && _textures[_textureUnit - GL_TEXTURE0_ARB].enabled)
				{
					_textures[_textureUnit - GL_TEXTURE0_ARB].enabled = false;
					::glDisable(GL_TEXTURE_2D);
				}
		}
		else
			_textures[_textureUnit - GL_TEXTURE0_ARB].enabled = texture != 0;
	}
}

inline void CTextureState::glActiveTextureARB(GLenum texture)
{
	++_profileData.glActiveTextureARBProfileData.overallCallCount;

	if (texture != _textureUnit)
	{
		++_profileData.glActiveTextureARBProfileData.unfilteredCallCount;
		::glActiveTextureARB(_textureUnit = texture);
	}
}

inline void CTextureState::Set(const CTextureState &previous)
{
	for (int i = (int)_textures.size() - 1; i > -1; --i)
	{
		if (_textures[i].texture == ~0 || (_textures[i].enabled == previous._textures[i].enabled && _textures[i].texture == previous._textures[i].texture))
			continue;

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

inline void CProgramState::glUseProgramObjectARB(GLhandleARB programObj)
{
	++_profileData.glUseProgramObjectARBProfileData.overallCallCount;
	if (programObj != _programObj)
	{
		++_profileData.glUseProgramObjectARBProfileData.unfilteredCallCount;
		::glUseProgramObjectARB(_programObj = programObj);
	}
}

inline void CProgramState::Set(const CProgramState &previous)
{
	if (GLEW_ARB_shader_objects)
	{
		++_profileData.glUseProgramObjectARBProfileData.overallCallCount;
		
		if (_programObj != previous._programObj)
		{
			++_profileData.glUseProgramObjectARBProfileData.unfilteredCallCount;
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
	Core()->RenderProfilerText("No data available.", TColor4());
}

void CStateManager<false>::OutputProfileSummary()
{
	Core()->RenderProfilerText("Filtering is disabled.", TColor4());
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

void CStateManager<true>::glBindTexture(GLenum target, GLuint texture)
{
	_curState.textureState.glBindTexture(target, texture);
}

void CStateManager<true>::glActiveTextureARB(GLenum texture)
{
	_curState.textureState.glActiveTextureARB(texture);
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
	Core()->RenderProfilerText(str.c_str(), TColor4());
}

void CStateManager<true>::OutputProfileData()
{
	Core()->RenderProfilerText("OpenGL functions calls (unfiltered/overall)", TColor4());
	_OutputFunctionProfileData("glEnable...................", _profileData.toggleStateProfileData.glEnableProfileData);
	_OutputFunctionProfileData("glDisable..................", _profileData.toggleStateProfileData.glDisableProfileData);
	_OutputFunctionProfileData("glEnableClientState........", _profileData.toggleStateProfileData.glEnableClientStateProfileData);
	_OutputFunctionProfileData("glDisableClientState.......", _profileData.toggleStateProfileData.glDisableClientStateProfileData);
	_OutputFunctionProfileData("glEnableVertexAttribArray..", _profileData.toggleStateProfileData.glEnableVertexAttribArrayARB);
	_OutputFunctionProfileData("glDisableVertexAttribArray.", _profileData.toggleStateProfileData.glDisableVertexAttribArrayARB);
	_OutputFunctionProfileData("glBlendFunc................", _profileData.blendFuncStateProfileData.glBlendFuncProfileData);
	_OutputFunctionProfileData("glAlphaFunc................", _profileData.alphaFuncStateProfileData.glAlphaFuncProfileData);
	_OutputFunctionProfileData("glColor4f..................", _profileData.colorStateProfileData.glColor4fProfileData);
	_OutputFunctionProfileData("glBindTexture..............", _profileData.textureStateProfileData.glBindTextureProfileData);
	_OutputFunctionProfileData("glActiveTexture............", _profileData.textureStateProfileData.glActiveTextureARBProfileData);
	_OutputFunctionProfileData("glUseProgramObject.........", _profileData.programStateProfileData.glUseProgramObjectARBProfileData);
	_OutputFunctionProfileData("glPolygonMode..............", _profileData.polygonModeStateProfileData.glPolygonMode);
	_OutputFunctionProfileData("glCullFace.................", _profileData.modeStateProfilerData.glCullFace);
	_OutputFunctionProfileData("glFrontFace................", _profileData.modeStateProfilerData.glFrontFace);
	_OutputFunctionProfileData("glDepthFunc................", _profileData.modeStateProfilerData.glDepthFunc);
	_OutputFunctionProfileData("glDepthMask................", _profileData.modeStateProfilerData.glDepthMask);
	_OutputFunctionProfileData("glBindBuffer...............", _profileData.bindBufferStateProfilerData.glBindBufferARB);
	_OutputFunctionProfileData("glLineWidth................", _profileData.valueStateProfilerData.glLineWidth);
	_OutputFunctionProfileData("glPointSize................", _profileData.valueStateProfilerData.glPointSize);
}

void CStateManager<true>::OutputProfileSummary()
{
	::TProfileData summary;

	summary.unfilteredCallCount =
		_profileData.toggleStateProfileData.glEnableProfileData.unfilteredCallCount +
		_profileData.toggleStateProfileData.glDisableProfileData.unfilteredCallCount +
		_profileData.toggleStateProfileData.glEnableClientStateProfileData.unfilteredCallCount +
		_profileData.toggleStateProfileData.glDisableClientStateProfileData.unfilteredCallCount +
		_profileData.toggleStateProfileData.glEnableVertexAttribArrayARB.unfilteredCallCount +
		_profileData.toggleStateProfileData.glDisableVertexAttribArrayARB.unfilteredCallCount +
		_profileData.blendFuncStateProfileData.glBlendFuncProfileData.unfilteredCallCount +
		_profileData.alphaFuncStateProfileData.glAlphaFuncProfileData.unfilteredCallCount +
		_profileData.colorStateProfileData.glColor4fProfileData.unfilteredCallCount +
		_profileData.textureStateProfileData.glBindTextureProfileData.unfilteredCallCount +
		_profileData.textureStateProfileData.glActiveTextureARBProfileData.unfilteredCallCount +
		_profileData.programStateProfileData.glUseProgramObjectARBProfileData.unfilteredCallCount +
		_profileData.polygonModeStateProfileData.glPolygonMode.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glCullFace.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glFrontFace.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glDepthFunc.unfilteredCallCount + 
		_profileData.modeStateProfilerData.glDepthMask.unfilteredCallCount +
		_profileData.valueStateProfilerData.glLineWidth.unfilteredCallCount +
		_profileData.valueStateProfilerData.glPointSize.unfilteredCallCount +
		_profileData.bindBufferStateProfilerData.glBindBufferARB.unfilteredCallCount;

	summary.overallCallCount =
		_profileData.toggleStateProfileData.glEnableProfileData.overallCallCount +
		_profileData.toggleStateProfileData.glDisableProfileData.overallCallCount +
		_profileData.toggleStateProfileData.glEnableClientStateProfileData.overallCallCount +
		_profileData.toggleStateProfileData.glDisableClientStateProfileData.overallCallCount +
		_profileData.toggleStateProfileData.glEnableVertexAttribArrayARB.overallCallCount +
		_profileData.toggleStateProfileData.glDisableVertexAttribArrayARB.overallCallCount +
		_profileData.blendFuncStateProfileData.glBlendFuncProfileData.overallCallCount +
		_profileData.alphaFuncStateProfileData.glAlphaFuncProfileData.overallCallCount +
		_profileData.colorStateProfileData.glColor4fProfileData.overallCallCount +
		_profileData.textureStateProfileData.glBindTextureProfileData.overallCallCount +
		_profileData.textureStateProfileData.glActiveTextureARBProfileData.overallCallCount +
		_profileData.programStateProfileData.glUseProgramObjectARBProfileData.overallCallCount +
		_profileData.polygonModeStateProfileData.glPolygonMode.overallCallCount + 
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