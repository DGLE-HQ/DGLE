/* gl.h
** 
** The contents of this file are subject to the GLX Public License Version 1.0
** (the "License"). You may not use this file except in compliance with the
** License. You may obtain a copy of the License at Silicon Graphics, Inc.,
** attn: Legal Services, 2011 N. Shoreline Blvd., Mountain View, CA 94043
** or at http://www.sgi.com/software/opensource/glx/license.html.
** 
** Software distributed under the License is distributed on an "AS IS"
** basis. ALL WARRANTIES ARE DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY
** IMPLIED WARRANTIES OF MERCHANTABILITY, OF FITNESS FOR A PARTICULAR
** PURPOSE OR OF NON- INFRINGEMENT. See the License for the specific
** language governing rights and limitations under the License.
** 
** The Original Software is GLX version 1.2 source code, released February,
** 1999. The developer of the Original Software is Silicon Graphics, Inc.
** Those portions of the Subject Software created by Silicon Graphics, Inc.
** are Copyright (c) 1991-9 Silicon Graphics, Inc. All Rights Reserved.
*/

/* glext.h
** 
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2002 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: This software was created using the
** OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
** not been independently verified as being compliant with the OpenGL(R)
** version 1.2.1 Specification.
*/

/* wglext.h
**
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2002 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: This software was created using the
** OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
** not been independently verified as being compliant with the OpenGL(R)
** version 1.2.1 Specification.
*/

/* glu.h
**
** Copyright (c) 1985-95, Microsoft Corporation
*/

/*
** Copyright 1991-1993, Silicon Graphics, Inc.
** All Rights Reserved.
** 
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
** 
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
*/

/* C# Header and Wrapper by Lars Middendorf, lmid@gmx.de */

/* Modification, corrections and additions by Alberto Bencivenni, devDept.com */

//=============================================================
// C# OpenGL Framework
// Copyright (c) 2005-2006 devDept.com
// All rights reserved.
//
// For more information on this program, please visit: 
// http://www.csharpopenglframework.com
//
// For licensing information, please visit: 
// http://www.csharpopenglframework.com/licensing.html
//=============================================================

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.Drawing;
using System.IO;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Collections;
using OpenGL.Delegates;
using System.Security;
using System.Diagnostics;

namespace OpenGL
{

	[StructLayout(LayoutKind.Sequential)]
	public class PixelFormatDescriptor
	{
		public short Size        = 40;
		public short Version     = 1;
		public int   dwFlags     = (int) (pixelFormat.DRAW_TO_WINDOW | pixelFormat.SUPPORT_OPENGL | pixelFormat.DOUBLEBUFFER);
		public byte  PixelType   = (int) pixelTypes.TYPE_RGBA;
		public byte  ColorBits   = 32;
		public byte  RedBits;
		public byte  RedShift;
		public byte  GreenBits;
		public byte  GreenShift;
		public byte  BlueBits;
		public byte  BlueShift;
		public byte  AlphaBits   = 8;
		public byte  AlphaShift;
        public byte  AccumBits; 
		public byte  AccumRedBits;
		public byte  AccumGreenBits;
		public byte  AccumBlueBits;
		public byte  AccumAlphaBits;
		public byte  DepthBits   = 24;
		public byte  StencilBits = 8;
		public byte  AuxBuffers;
		public byte  LayerType;
		public byte  Reserved;
		public int   dwLayerMask;
		public int   dwVisibleMask;
		public int   dwDamageMask;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct CharSize
	{
		public long cx;
		public long cy;
	}

	public class Windows
	{
		[DllImport("User32")]
		public static extern int GetDC(int hwnd);
		[DllImport("User32")]
		public static extern int ReleaseDC(int hwnd, int hDC);
		[DllImport("GDI32")]
		public static extern int ChoosePixelFormat(int dc, [In, MarshalAs(UnmanagedType.LPStruct)]PixelFormatDescriptor pfd);
        [DllImport("GDI32")]
        public static extern int DescribePixelFormat(int dc, int iPixelFormat, short nBytes, [In, MarshalAs(UnmanagedType.LPStruct)]PixelFormatDescriptor pfd);
		[DllImport("GDI32")]
		public static extern bool SetPixelFormat(int dc, int format, [In, MarshalAs(UnmanagedType.LPStruct)]PixelFormatDescriptor pfd);
		[DllImport("GDI32")]
		public static extern void SwapBuffers(int dc);
		[DllImport("Kernel32")]
		public static extern int GetProcAddress(int handle, String funcname);
		[DllImport("Kernel32")]
		public static extern int LoadLibrary(String funcname);
		[DllImport("Kernel32.dll")]
		public static extern int FreeLibrary(int handle);
		[DllImport("GDI32")]
		public static extern IntPtr SelectObject(int dc, IntPtr obj);
		[DllImport("GDI32")]
		public static extern bool DeleteObject(IntPtr objectHandle);
		[DllImport("GDI32")]
		public static extern IntPtr GetStockObject(int obj);
		[DllImport("GDI32")]
		public static extern IntPtr CreateFont(int height, int width, int esc, int orientation, int fnwidth, int italic, int underline, int strikeout, int charset, int precision, int clipprecision, int quality, int pitch, string face);
		[DllImport("GDI32")]
		public static extern bool GetTextExtentPoint32(int dc, string text, int length, out CharSize result);
		[DllImport("Kernel32")]
		public static extern int GetLastError();
	}

	namespace Delegates
	{
		public delegate void glActiveTexture(int texid);
		public delegate void glClientActiveTexture(int texid);
		public delegate void glMultiTexCoord1d(int texid, double d);
		public delegate void glMultiTexCoord1dv(int texid, double[] dparams);
		public delegate void glMultiTexCoord1f(int texid, float f);
		public delegate void glMultiTexCoord1fv(int texid, float[] fparams);
		public delegate void glMultiTexCoord1i(int texid, int i);
		public delegate void glMultiTexCoord1iv(int texid, int[] iparams);
		public delegate void glMultiTexCoord1s(int texid, short s);
		public delegate void glMultiTexCoord1sv(int texid, short[] sparams);
		public delegate void glMultiTexCoord2d(int texid, double d1, double d2);
		public delegate void glMultiTexCoord2dv(int texid, double[] dparams);
		public delegate void glMultiTexCoord2f(int texid, float f1, float f2);
		public delegate void glMultiTexCoord2fv(int texid, float[] fparams);
		public delegate void glMultiTexCoord2i(int texid, int i1, int i2);
		public delegate void glMultiTexCoord2iv(int texid, int[] iparams);
		public delegate void glMultiTexCoord2s(int texid, short s1, short s2);
		public delegate void glMultiTexCoord2sv(int texid, short[] sparams);
		public delegate void glMultiTexCoord3d(int texid, double d1, double d2, double d3);
		public delegate void glMultiTexCoord3dv(int texid, double[] dparams);
		public delegate void glMultiTexCoord3f(int texid, float f1, float f2, float f3);
		public delegate void glMultiTexCoord3fv(int texid, float[] fparams);
		public delegate void glMultiTexCoord3i(int texid, int i1, int i2, int i3);
		public delegate void glMultiTexCoord3iv(int texid, int[] iparams);
		public delegate void glMultiTexCoord3s(int texid, short s1, short s2, short s3);
		public delegate void glMultiTexCoord3sv(int texid, short[] sparams);
		public delegate void glMultiTexCoord4d(int texid, double d1, double d2, double d3, double d4);
		public delegate void glMultiTexCoord4dv(int texid, double[] dparams);
		public delegate void glMultiTexCoord4f(int texid, float f1, float f2, float f3, float f4);
		public delegate void glMultiTexCoord4fv(int texid, float[] fparams);
		public delegate void glMultiTexCoord4i(int texid, int i1, int i2, int i3, int i4);
		public delegate void glMultiTexCoord4iv(int texid, int[] iparams);
		public delegate void glMultiTexCoord4s(int texid, short s1, short s2, short s3, short s4);
		public delegate void glMultiTexCoord4sv(int texid, short[] sparams);

		public delegate void glBindBufferARB(int target, int buffer);
		public delegate void glDeleteBuffersARB(int n, int[] buffers);
		public delegate void glGenBuffersARB(int n, int[] buffers);
		public delegate bool glIsBufferARB(int buffer);
		public delegate void glBufferData(int target, int size, IntPtr data, int usage);
		public delegate void glBufferSubData(int target, int offset, int size, IntPtr data);
		public delegate void glGetBufferSubDataARB(int target, int offset, int size, IntPtr data);
		public delegate IntPtr glMapBufferARB(int target, int offset, int size, IntPtr data);
		public delegate bool glUnmapBufferARB(int target);
		public delegate void glGetBufferParameterivARB(int target, int pname, int[] iparams);
		public delegate void glGetBufferPointervARB(int target, int pname, IntPtr[] iparams);

		public delegate void glGenQueriesARB(int n, int[] queries);
		public delegate void glDeleteQueriesARB(int n, int[] queries);
		public delegate bool glIsQueryARB(int target, int id);
		public delegate void glBeginQueryARB(int target, int id);
		public delegate void glEndQueryARB(int target);
		public delegate void glGetQueryivARB(int target, int pname, int[] iparams);
		public delegate void glGetQueryObjectivARB(int target, int pname, int[] iparams);
		public delegate void glGetQueryObjectuivARB(int target, int pname, uint[] iparams);

		public delegate void glVertexAttribPointerARB(int index, int size, int type, bool normalized, int stride, IntPtr pointer);
		public delegate void glEnableVertexAttribArrayARB(int index);
		public delegate void glDisableVertexAttribArrayARB(int index);
		public delegate void glProgramStringARB(int target, int format, int size, string source);
		public delegate void glBindProgramARB(int target, int program);
		public delegate void glDeleteProgramsARB(int size, int[] programs);
		public delegate void glGenProgramsARB(int size, int[] programs);
		public delegate void glProgramLocalParameter4fARB(int target, int index, float x, float y, float z, float w);
		public delegate void glProgramEnvParameter4fARB(int target, int index, float x, float y, float z, float w);
		public delegate void glGetProgramivARB(int target, int pname, int[] iparams);
		public delegate void glGetProgramEnvParameterfvARB(int target, int index, float[] fparams);
		public delegate void glGetProgramLocalParameterfvARB(int target, int index, float[] fparams);
		public delegate void glVertexAttrib2fARB(int index, float x, float y);
		public delegate void glVertexAttrib3fARB(int index, float x, float y, float z);
		public delegate void glVertexAttrib4fARB(int index, float x, float y, float z, float w);

		public delegate void glStencilOpSeparateATI(int face, int sfail, int dpfail, int dppass);
		public delegate void glStencilFuncSeparateATI(int frontfunc, int backfunc, int refval, uint mask);

		public delegate void glGetCompressedTexImageARB(int target, int lod, IntPtr img);
		public delegate void glCompressedTexImage2DARB(int target, int level, int internalformat, int width, int height, int border, int imagesize, IntPtr data);

		public delegate void glTexImage3D(int target, int level, int internalformat, int width, int height, int depth, int border, int format, int type, IntPtr pixels);

		public delegate void glBlendEquation(int mode);
		public delegate void glWindowPos2f(float x, float y);

		public delegate int glCreateShaderObjectARB(int shaderType);
		public delegate void glShaderSourceARB(int shaderobj, int nstrings, string[] source, int[] lengths);
		public delegate bool glCompileShaderARB(int shaderobj);
		public delegate void glDeleteObjectARB(int obj);
		public delegate int glGetHandleARB(int pname);
		public delegate void glDetachObjectARB(int container, int attached);
		public delegate int glCreateProgramObjectARB();
		public delegate void glAttachObjectARB(int programObj, int shaderObj);
		public delegate void glLinkProgramARB(int programObj);
		public delegate void glUseProgramObjectARB(int programObj);
		public delegate void glValidateProgramARB(int programObj);
		public delegate void glGetObjectParameterfvARB(int obj, int pname, float[] fparams);
		public delegate void glGetObjectParameterivARB(int obj, int pname, int[] iparams);
		public delegate void glGetActiveAttribARB(int programObj, int index, int maxLength, ref int length, ref int size, ref int type, string name);
		public delegate void glGetActiveUniformARB(int programObj, int index, int maxLength, ref int length, ref int size, ref int type, string name);
		public delegate void glGetAttachedObjectsARB(int programObj, int maxCount, ref int count, int[] objects);
		public delegate int glGetAttribLocationARB(int programObj, string name);
		public delegate void glGetShaderSourceARB(int shader, int maxLength, ref int length, ref string source);
		public delegate void glGetUniformfvARB(int programObj, int location, float[] fparams);
		public delegate void glGetUniformivARB(int programObj, int location, float[] iparams);
		public delegate int glGetUniformLocationARB(int programObj, string name);
		public delegate void glGetInfoLogARB(int shader, int maxLength, ref int length, StringBuilder infoLog);
		public delegate void glBindAttribLocationARB(int programObj, int index, string name);
		public delegate void glUniform1fARB(int location, float x);
		public delegate void glUniform2fARB(int location, float x, float y);
		public delegate void glUniform3fARB(int location, float x, float y, float z);
		public delegate void glUniform4fARB(int location, float x, float y, float z, float w);
		public delegate void glUniform1iARB(int location, int x);
		public delegate void glUniform2iARB(int location, int x, int y);
		public delegate void glUniform3iARB(int location, int x, int y, int z);
		public delegate void glUniform4iARB(int location, int x, int y, int z, int w);
		public delegate void glUniform1fvARB(int location, float[] value);
		public delegate void glUniform2fvARB(int location, float[] value);
		public delegate void glUniform3fvARB(int location, float[] value);
		public delegate void glUniform4fvARB(int location, float[] value);
		public delegate void glUniform1ivARB(int location, int[] value);
		public delegate void glUniform2ivARB(int location, int[] value);
		public delegate void glUniform3ivARB(int location, int[] value);
		public delegate void glUniform4ivARB(int location, int[] value);
		public delegate void glUniformMatrix2fvARB(int location, int count, bool transpose, float[] matrix);
		public delegate void glUniformMatrix3fvARB(int location, int count, bool transpose, float[] matrix);
		public delegate void glUniformMatrix4fvARB(int location, int count, bool transpose, float[] matrix);
        
		public delegate bool glIsRenderbufferEXT(int renderbuffer);
		public delegate void glBindRenderbufferEXT(int target, int renderbuffer);
		public delegate void glDeleteRenderbuffersEXT(int n, int[] renderbuffers);
		public delegate void glGenRenderbuffersEXT(int n, int[] renderbuffers);
		public delegate void glRenderbufferStorageEXT(int target, int internalformat,int width,int height);
		public delegate void glGetRenderbufferParameterivEXT(int target,int pname, int[] paramlist);
		public delegate bool glIsFramebufferEXT(int framebuffer);
		public delegate void glBindFramebufferEXT(int target, int framebuffer);
		public delegate void glDeleteFramebuffersEXT(int n,int[] framebuffers);
		public delegate void glGenFramebuffersEXT(int n, int[] framebuffers);
		public delegate int glCheckFramebufferStatusEXT(int target);
		public delegate void glFramebufferTexture1DEXT(int target,int attachment,int textarget, int texture,int level);
		public delegate void glFramebufferTexture2DEXT(int target,int attachment,int textarget, int texture,int level);
		public delegate void glFramebufferTexture3DEXT(int target,int attachment,int textarget, int texture,int level, int zoffset);
		public delegate void glFramebufferRenderbufferEXT(int target,int attachment,int renderbuffertarget, int renderbuffer);
		public delegate void glGetFramebufferAttachmentParameterivEXT(int target, int attachment,int pname, int[] paramlist);
		public delegate void glGenerateMipmapEXT(int target);
	}

    public enum pixelFormat
    {

        DOUBLEBUFFER         = 0x00000001,
		STEREO               = 0x00000002,
		DRAW_TO_WINDOW       = 0x00000004,
		DRAW_TO_BITMAP       = 0x00000008,
		SUPPORT_GDI          = 0x00000010,
		SUPPORT_OPENGL       = 0x00000020,
		GENERIC_FORMAT       = 0x00000040,
		NEED_PALETTE         = 0x00000080,
		NEED_SYSTEM_PALETTE  = 0x00000100,
		SWAP_EXCHANGE        = 0x00000200,
		SWAP_COPY            = 0x00000400,
		SWAP_LAYER_BUFFERS   = 0x00000800,
		GENERIC_ACCELERATED  = 0x00001000,
		SUPPORT_DIRECTDRAW   = 0x00002000

    }

    public enum pixelTypes
    {
        TYPE_RGBA       = 0,
        TYPE_COLORINDEX = 1
    }

	public class gl
	{
		public const string DLLName="OPENGL32.DLL";
		public const int VERSION_1_1 = 1;
		public const int VERSION_1_2 = 1;
		public const int ARB_imaging = 1;
		public const int ARB_multitexture = 1;
		public const int ACCUM = 0x0100;
		public const int LOAD = 0x0101;
		public const int RETURN = 0x0102;
		public const int MULT = 0x0103;
		public const int ADD = 0x0104;
		public const int NEVER = 0x0200;
		public const int LESS = 0x0201;
		public const int EQUAL = 0x0202;
		public const int LEQUAL = 0x0203;
		public const int GREATER = 0x0204;
		public const int NOTEQUAL = 0x0205;
		public const int GEQUAL = 0x0206;
		public const int ALWAYS = 0x0207;
		public const int CURRENT_BIT = 0x00000001;
		public const int POINT_BIT = 0x00000002;
		public const int LINE_BIT = 0x00000004;
		public const int POLYGON_BIT = 0x00000008;
		public const int POLYGON_STIPPLE_BIT = 0x00000010;
		public const int PIXEL_MODE_BIT = 0x00000020;
		public const int LIGHTING_BIT = 0x00000040;
		public const int FOG_BIT = 0x00000080;
		public const int DEPTH_BUFFER_BIT = 0x00000100;
		public const int ACCUM_BUFFER_BIT = 0x00000200;
		public const int STENCIL_BUFFER_BIT = 0x00000400;
		public const int VIEWPORT_BIT = 0x00000800;
		public const int TRANSFORM_BIT = 0x00001000;
		public const int ENABLE_BIT = 0x00002000;
		public const int COLOR_BUFFER_BIT = 0x00004000;
		public const int HINT_BIT = 0x00008000;
		public const int EVAL_BIT = 0x00010000;
		public const int LIST_BIT = 0x00020000;
		public const int TEXTURE_BIT = 0x00040000;
		public const int SCISSOR_BIT = 0x00080000;
		public const int ALL_ATTRIB_BITS = 0x000fffff;
		public const int POINTS = 0x0000;
		public const int LINES = 0x0001;
		public const int LINE_LOOP = 0x0002;
		public const int LINE_STRIP = 0x0003;
		public const int TRIANGLES = 0x0004;
		public const int TRIANGLE_STRIP = 0x0005;
		public const int TRIANGLE_FAN = 0x0006;
		public const int QUADS = 0x0007;
		public const int QUAD_STRIP = 0x0008;
		public const int POLYGON = 0x0009;
		public const int ZERO = 0;
		public const int ONE = 1;
		public const int SRC_COLOR = 0x0300;
		public const int ONE_MINUS_SRC_COLOR = 0x0301;
		public const int SRC_ALPHA = 0x0302;
		public const int ONE_MINUS_SRC_ALPHA = 0x0303;
		public const int DST_ALPHA = 0x0304;
		public const int ONE_MINUS_DST_ALPHA = 0x0305;
		public const int DST_COLOR = 0x0306;
		public const int ONE_MINUS_DST_COLOR = 0x0307;
		public const int SRC_ALPHA_SATURATE = 0x0308;
		public const int TRUE = 1;
		public const int FALSE = 0;
		public const int CLIP_PLANE0 = 0x3000;
		public const int CLIP_PLANE1 = 0x3001;
		public const int CLIP_PLANE2 = 0x3002;
		public const int CLIP_PLANE3 = 0x3003;
		public const int CLIP_PLANE4 = 0x3004;
		public const int CLIP_PLANE5 = 0x3005;
		public const int BYTE = 0x1400;
		public const int UNSIGNED_BYTE = 0x1401;
		public const int SHORT = 0x1402;
		public const int UNSIGNED_SHORT = 0x1403;
		public const int INT = 0x1404;
		public const int UNSIGNED_INT = 0x1405;
		public const int FLOAT = 0x1406;
		public const int _2_BYTES = 0x1407;
		public const int _3_BYTES = 0x1408;
		public const int _4_BYTES = 0x1409;
		public const int DOUBLE = 0x140A;
		public const int NONE = 0;
		public const int FRONT_LEFT = 0x0400;
		public const int FRONT_RIGHT = 0x0401;
		public const int BACK_LEFT = 0x0402;
		public const int BACK_RIGHT = 0x0403;
		public const int FRONT = 0x0404;
		public const int BACK = 0x0405;
		public const int LEFT = 0x0406;
		public const int RIGHT = 0x0407;
		public const int FRONT_AND_BACK = 0x0408;
		public const int AUX0 = 0x0409;
		public const int AUX1 = 0x040A;
		public const int AUX2 = 0x040B;
		public const int AUX3 = 0x040C;
		public const int NO_ERROR = 0;
		public const int INVALID_ENUM = 0x0500;
		public const int INVALID_VALUE = 0x0501;
		public const int INVALID_OPERATION = 0x0502;
		public const int STACK_OVERFLOW = 0x0503;
		public const int STACK_UNDERFLOW = 0x0504;
		public const int OUT_OF_MEMORY = 0x0505;
		public const int _2D = 0x0600;
		public const int _3D = 0x0601;
		public const int _3D_COLOR = 0x0602;
		public const int _3D_COLOR_TEXTURE = 0x0603;
		public const int _4D_COLOR_TEXTURE = 0x0604;
		public const int PASS_THROUGH_TOKEN = 0x0700;
		public const int POINT_TOKEN = 0x0701;
		public const int LINE_TOKEN = 0x0702;
		public const int POLYGON_TOKEN = 0x0703;
		public const int BITMAP_TOKEN = 0x0704;
		public const int DRAW_PIXEL_TOKEN = 0x0705;
		public const int COPY_PIXEL_TOKEN = 0x0706;
		public const int LINE_RESET_TOKEN = 0x0707;
		public const int EXP = 0x0800;
		public const int EXP2 = 0x0801;
		public const int CW = 0x0900;
		public const int CCW = 0x0901;
		public const int COEFF = 0x0A00;
		public const int ORDER = 0x0A01;
		public const int DOMAIN = 0x0A02;
		public const int CURRENT_COLOR = 0x0B00;
		public const int CURRENT_INDEX = 0x0B01;
		public const int CURRENT_NORMAL = 0x0B02;
		public const int CURRENT_TEXTURE_COORDS = 0x0B03;
		public const int CURRENT_RASTER_COLOR = 0x0B04;
		public const int CURRENT_RASTER_INDEX = 0x0B05;
		public const int CURRENT_RASTER_TEXTURE_COORDS = 0x0B06;
		public const int CURRENT_RASTER_POSITION = 0x0B07;
		public const int CURRENT_RASTER_POSITION_VALID = 0x0B08;
		public const int CURRENT_RASTER_DISTANCE = 0x0B09;
		public const int POINT_SMOOTH = 0x0B10;
		public const int POINT_SIZE = 0x0B11;
		public const int POINT_SIZE_RANGE = 0x0B12;
		public const int POINT_SIZE_GRANULARITY = 0x0B13;
		public const int LINE_SMOOTH = 0x0B20;
		public const int LINE_WIDTH = 0x0B21;
		public const int LINE_WIDTH_RANGE = 0x0B22;
		public const int LINE_WIDTH_GRANULARITY = 0x0B23;
		public const int LINE_STIPPLE = 0x0B24;
		public const int LINE_STIPPLE_PATTERN = 0x0B25;
		public const int LINE_STIPPLE_REPEAT = 0x0B26;
		public const int LIST_MODE = 0x0B30;
		public const int MAX_LIST_NESTING = 0x0B31;
		public const int LIST_BASE = 0x0B32;
		public const int LIST_INDEX = 0x0B33;
		public const int POLYGON_MODE = 0x0B40;
		public const int POLYGON_SMOOTH = 0x0B41;
		public const int POLYGON_STIPPLE = 0x0B42;
		public const int EDGE_FLAG = 0x0B43;
		public const int CULL_FACE = 0x0B44;
		public const int CULL_FACE_MODE = 0x0B45;
		public const int FRONT_FACE = 0x0B46;
		public const int LIGHTING = 0x0B50;
		public const int LIGHT_MODEL_LOCAL_VIEWER = 0x0B51;
		public const int LIGHT_MODEL_TWO_SIDE = 0x0B52;
		public const int LIGHT_MODEL_AMBIENT = 0x0B53;
		public const int SHADE_MODEL = 0x0B54;
		public const int COLOR_MATERIAL_FACE = 0x0B55;
		public const int COLOR_MATERIAL_PARAMETER = 0x0B56;
		public const int COLOR_MATERIAL = 0x0B57;
		public const int FOG = 0x0B60;
		public const int FOG_INDEX = 0x0B61;
		public const int FOG_DENSITY = 0x0B62;
		public const int FOG_START = 0x0B63;
		public const int FOG_END = 0x0B64;
		public const int FOG_MODE = 0x0B65;
		public const int FOG_COLOR = 0x0B66;
		public const int DEPTH_RANGE = 0x0B70;
		public const int DEPTH_TEST = 0x0B71;
		public const int DEPTH_WRITEMASK = 0x0B72;
		public const int DEPTH_CLEAR_VALUE = 0x0B73;
		public const int DEPTH_FUNC = 0x0B74;
		public const int ACCUM_CLEAR_VALUE = 0x0B80;
		public const int STENCIL_TEST = 0x0B90;
		public const int STENCIL_CLEAR_VALUE = 0x0B91;
		public const int STENCIL_FUNC = 0x0B92;
		public const int STENCIL_VALUE_MASK = 0x0B93;
		public const int STENCIL_FAIL = 0x0B94;
		public const int STENCIL_PASS_DEPTH_FAIL = 0x0B95;
		public const int STENCIL_PASS_DEPTH_PASS = 0x0B96;
		public const int STENCIL_REF = 0x0B97;
		public const int STENCIL_WRITEMASK = 0x0B98;
		public const int MATRIX_MODE = 0x0BA0;
		public const int NORMALIZE = 0x0BA1;
		public const int VIEWPORT = 0x0BA2;
		public const int MODELVIEW_STACK_DEPTH = 0x0BA3;
		public const int PROJECTION_STACK_DEPTH = 0x0BA4;
		public const int TEXTURE_STACK_DEPTH = 0x0BA5;
		public const int MODELVIEW_MATRIX = 0x0BA6;
		public const int PROJECTION_MATRIX = 0x0BA7;
		public const int TEXTURE_MATRIX = 0x0BA8;
		public const int ATTRIB_STACK_DEPTH = 0x0BB0;
		public const int CLIENT_ATTRIB_STACK_DEPTH = 0x0BB1;
		public const int ALPHA_TEST = 0x0BC0;
		public const int ALPHA_TEST_FUNC = 0x0BC1;
		public const int ALPHA_TEST_REF = 0x0BC2;
		public const int DITHER = 0x0BD0;
		public const int BLEND_DST = 0x0BE0;
		public const int BLEND_SRC = 0x0BE1;
		public const int BLEND = 0x0BE2;
		public const int LOGIC_OP_MODE = 0x0BF0;
		public const int INDEX_LOGIC_OP = 0x0BF1;
		public const int COLOR_LOGIC_OP = 0x0BF2;
		public const int AUX_BUFFERS = 0x0C00;
		public const int DRAW_BUFFER = 0x0C01;
		public const int READ_BUFFER = 0x0C02;
		public const int SCISSOR_BOX = 0x0C10;
		public const int SCISSOR_TEST = 0x0C11;
		public const int INDEX_CLEAR_VALUE = 0x0C20;
		public const int INDEX_WRITEMASK = 0x0C21;
		public const int COLOR_CLEAR_VALUE = 0x0C22;
		public const int COLOR_WRITEMASK = 0x0C23;
		public const int INDEX_MODE = 0x0C30;
		public const int RGBA_MODE = 0x0C31;
		public const int DOUBLEBUFFER = 0x0C32;
		public const int STEREO = 0x0C33;
		public const int RENDER_MODE = 0x0C40;
		public const int PERSPECTIVE_CORRECTION_HINT = 0x0C50;
		public const int POINT_SMOOTH_HINT = 0x0C51;
		public const int LINE_SMOOTH_HINT = 0x0C52;
		public const int POLYGON_SMOOTH_HINT = 0x0C53;
		public const int FOG_HINT = 0x0C54;
		public const int TEXTURE_GEN_S = 0x0C60;
		public const int TEXTURE_GEN_T = 0x0C61;
		public const int TEXTURE_GEN_R = 0x0C62;
		public const int TEXTURE_GEN_Q = 0x0C63;
		public const int PIXEL_MAP_I_TO_I = 0x0C70;
		public const int PIXEL_MAP_S_TO_S = 0x0C71;
		public const int PIXEL_MAP_I_TO_R = 0x0C72;
		public const int PIXEL_MAP_I_TO_G = 0x0C73;
		public const int PIXEL_MAP_I_TO_B = 0x0C74;
		public const int PIXEL_MAP_I_TO_A = 0x0C75;
		public const int PIXEL_MAP_R_TO_R = 0x0C76;
		public const int PIXEL_MAP_G_TO_G = 0x0C77;
		public const int PIXEL_MAP_B_TO_B = 0x0C78;
		public const int PIXEL_MAP_A_TO_A = 0x0C79;
		public const int PIXEL_MAP_I_TO_I_SIZE = 0x0CB0;
		public const int PIXEL_MAP_S_TO_S_SIZE = 0x0CB1;
		public const int PIXEL_MAP_I_TO_R_SIZE = 0x0CB2;
		public const int PIXEL_MAP_I_TO_G_SIZE = 0x0CB3;
		public const int PIXEL_MAP_I_TO_B_SIZE = 0x0CB4;
		public const int PIXEL_MAP_I_TO_A_SIZE = 0x0CB5;
		public const int PIXEL_MAP_R_TO_R_SIZE = 0x0CB6;
		public const int PIXEL_MAP_G_TO_G_SIZE = 0x0CB7;
		public const int PIXEL_MAP_B_TO_B_SIZE = 0x0CB8;
		public const int PIXEL_MAP_A_TO_A_SIZE = 0x0CB9;
		public const int UNPACK_SWAP_BYTES = 0x0CF0;
		public const int UNPACK_LSB_FIRST = 0x0CF1;
		public const int UNPACK_ROW_LENGTH = 0x0CF2;
		public const int UNPACK_SKIP_ROWS = 0x0CF3;
		public const int UNPACK_SKIP_PIXELS = 0x0CF4;
		public const int UNPACK_ALIGNMENT = 0x0CF5;
		public const int PACK_SWAP_BYTES = 0x0D00;
		public const int PACK_LSB_FIRST = 0x0D01;
		public const int PACK_ROW_LENGTH = 0x0D02;
		public const int PACK_SKIP_ROWS = 0x0D03;
		public const int PACK_SKIP_PIXELS = 0x0D04;
		public const int PACK_ALIGNMENT = 0x0D05;
		public const int MAP_COLOR = 0x0D10;
		public const int MAP_STENCIL = 0x0D11;
		public const int INDEX_SHIFT = 0x0D12;
		public const int INDEX_OFFSET = 0x0D13;
		public const int RED_SCALE = 0x0D14;
		public const int RED_BIAS = 0x0D15;
		public const int ZOOM_X = 0x0D16;
		public const int ZOOM_Y = 0x0D17;
		public const int GREEN_SCALE = 0x0D18;
		public const int GREEN_BIAS = 0x0D19;
		public const int BLUE_SCALE = 0x0D1A;
		public const int BLUE_BIAS = 0x0D1B;
		public const int ALPHA_SCALE = 0x0D1C;
		public const int ALPHA_BIAS = 0x0D1D;
		public const int DEPTH_SCALE = 0x0D1E;
		public const int DEPTH_BIAS = 0x0D1F;
		public const int MAX_EVAL_ORDER = 0x0D30;
		public const int MAX_LIGHTS = 0x0D31;
		public const int MAX_CLIP_PLANES = 0x0D32;
		public const int MAX_TEXTURE_SIZE = 0x0D33;
		public const int MAX_PIXEL_MAP_TABLE = 0x0D34;
		public const int MAX_ATTRIB_STACK_DEPTH = 0x0D35;
		public const int MAX_MODELVIEW_STACK_DEPTH = 0x0D36;
		public const int MAX_NAME_STACK_DEPTH = 0x0D37;
		public const int MAX_PROJECTION_STACK_DEPTH = 0x0D38;
		public const int MAX_TEXTURE_STACK_DEPTH = 0x0D39;
		public const int MAX_VIEWPORT_DIMS = 0x0D3A;
		public const int MAX_CLIENT_ATTRIB_STACK_DEPTH = 0x0D3B;
		public const int SUBPIXEL_BITS = 0x0D50;
		public const int INDEX_BITS = 0x0D51;
		public const int RED_BITS = 0x0D52;
		public const int GREEN_BITS = 0x0D53;
		public const int BLUE_BITS = 0x0D54;
		public const int ALPHA_BITS = 0x0D55;
		public const int DEPTH_BITS = 0x0D56;
		public const int STENCIL_BITS = 0x0D57;
		public const int ACCUM_RED_BITS = 0x0D58;
		public const int ACCUM_GREEN_BITS = 0x0D59;
		public const int ACCUM_BLUE_BITS = 0x0D5A;
		public const int ACCUM_ALPHA_BITS = 0x0D5B;
		public const int NAME_STACK_DEPTH = 0x0D70;
		public const int AUTO_NORMAL = 0x0D80;
		public const int MAP1_COLOR_4 = 0x0D90;
		public const int MAP1_INDEX = 0x0D91;
		public const int MAP1_NORMAL = 0x0D92;
		public const int MAP1_TEXTURE_COORD_1 = 0x0D93;
		public const int MAP1_TEXTURE_COORD_2 = 0x0D94;
		public const int MAP1_TEXTURE_COORD_3 = 0x0D95;
		public const int MAP1_TEXTURE_COORD_4 = 0x0D96;
		public const int MAP1_VERTEX_3 = 0x0D97;
		public const int MAP1_VERTEX_4 = 0x0D98;
		public const int MAP2_COLOR_4 = 0x0DB0;
		public const int MAP2_INDEX = 0x0DB1;
		public const int MAP2_NORMAL = 0x0DB2;
		public const int MAP2_TEXTURE_COORD_1 = 0x0DB3;
		public const int MAP2_TEXTURE_COORD_2 = 0x0DB4;
		public const int MAP2_TEXTURE_COORD_3 = 0x0DB5;
		public const int MAP2_TEXTURE_COORD_4 = 0x0DB6;
		public const int MAP2_VERTEX_3 = 0x0DB7;
		public const int MAP2_VERTEX_4 = 0x0DB8;
		public const int MAP1_GRID_DOMAIN = 0x0DD0;
		public const int MAP1_GRID_SEGMENTS = 0x0DD1;
		public const int MAP2_GRID_DOMAIN = 0x0DD2;
		public const int MAP2_GRID_SEGMENTS = 0x0DD3;
		public const int TEXTURE_1D = 0x0DE0;
		public const int TEXTURE_2D = 0x0DE1;
		public const int FEEDBACK_BUFFER_POINTER = 0x0DF0;
		public const int FEEDBACK_BUFFER_SIZE = 0x0DF1;
		public const int FEEDBACK_BUFFER_TYPE = 0x0DF2;
		public const int SELECTION_BUFFER_POINTER = 0x0DF3;
		public const int SELECTION_BUFFER_SIZE = 0x0DF4;
		public const int TEXTURE_WIDTH = 0x1000;
		public const int TEXTURE_HEIGHT = 0x1001;
		public const int TEXTURE_INTERNAL_FORMAT = 0x1003;
		public const int TEXTURE_BORDER_COLOR = 0x1004;
		public const int TEXTURE_BORDER = 0x1005;
		public const int DONT_CARE = 0x1100;
		public const int FASTEST = 0x1101;
		public const int NICEST = 0x1102;
		public const int LIGHT0 = 0x4000;
		public const int LIGHT1 = 0x4001;
		public const int LIGHT2 = 0x4002;
		public const int LIGHT3 = 0x4003;
		public const int LIGHT4 = 0x4004;
		public const int LIGHT5 = 0x4005;
		public const int LIGHT6 = 0x4006;
		public const int LIGHT7 = 0x4007;
		public const int AMBIENT = 0x1200;
		public const int DIFFUSE = 0x1201;
		public const int SPECULAR = 0x1202;
		public const int POSITION = 0x1203;
		public const int SPOT_DIRECTION = 0x1204;
		public const int SPOT_EXPONENT = 0x1205;
		public const int SPOT_CUTOFF = 0x1206;
		public const int CONSTANT_ATTENUATION = 0x1207;
		public const int LINEAR_ATTENUATION = 0x1208;
		public const int QUADRATIC_ATTENUATION = 0x1209;
		public const int COMPILE = 0x1300;
		public const int COMPILE_AND_EXECUTE = 0x1301;
		public const int CLEAR = 0x1500;
		public const int AND = 0x1501;
		public const int AND_REVERSE = 0x1502;
		public const int COPY = 0x1503;
		public const int AND_INVERTED = 0x1504;
		public const int NOOP = 0x1505;
		public const int XOR = 0x1506;
		public const int OR = 0x1507;
		public const int NOR = 0x1508;
		public const int EQUIV = 0x1509;
		public const int INVERT = 0x150A;
		public const int OR_REVERSE = 0x150B;
		public const int COPY_INVERTED = 0x150C;
		public const int OR_INVERTED = 0x150D;
		public const int NAND = 0x150E;
		public const int SET = 0x150F;
		public const int EMISSION = 0x1600;
		public const int SHININESS = 0x1601;
		public const int AMBIENT_AND_DIFFUSE = 0x1602;
		public const int COLOR_INDEXES = 0x1603;
		public const int MODELVIEW = 0x1700;
		public const int PROJECTION = 0x1701;
		public const int TEXTURE = 0x1702;
		public const int COLOR = 0x1800;
		public const int DEPTH = 0x1801;
		public const int STENCIL = 0x1802;
		public const int COLOR_INDEX = 0x1900;
		public const int STENCIL_INDEX = 0x1901;
		public const int DEPTH_COMPONENT = 0x1902;
		public const int RED = 0x1903;
		public const int GREEN = 0x1904;
		public const int BLUE = 0x1905;
		public const int ALPHA = 0x1906;
		public const int RGB = 0x1907;
		public const int RGBA = 0x1908;
		public const int LUMINANCE = 0x1909;
		public const int LUMINANCE_ALPHA = 0x190A;
		public const int BITMAP = 0x1A00;
		public const int POINT = 0x1B00;
		public const int LINE = 0x1B01;
		public const int FILL = 0x1B02;
		public const int RENDER = 0x1C00;
		public const int FEEDBACK = 0x1C01;
		public const int SELECT = 0x1C02;
		public const int FLAT = 0x1D00;
		public const int SMOOTH = 0x1D01;
		public const int KEEP = 0x1E00;
		public const int REPLACE = 0x1E01;
		public const int INCR = 0x1E02;
		public const int DECR = 0x1E03;
		public const int VENDOR = 0x1F00;
		public const int RENDERER = 0x1F01;
		public const int VERSION = 0x1F02;
		public const int EXTENSIONS = 0x1F03;
		public const int S = 0x2000;
		public const int T = 0x2001;
		public const int R = 0x2002;
		public const int Q = 0x2003;
		public const int MODULATE = 0x2100;
		public const int DECAL = 0x2101;
		public const int TEXTURE_ENV_MODE = 0x2200;
		public const int TEXTURE_ENV_COLOR = 0x2201;
		public const int TEXTURE_ENV = 0x2300;
		public const int EYE_LINEAR = 0x2400;
		public const int OBJECT_LINEAR = 0x2401;
		public const int SPHERE_MAP = 0x2402;
		public const int TEXTURE_GEN_MODE = 0x2500;
		public const int OBJECT_PLANE = 0x2501;
		public const int EYE_PLANE = 0x2502;
		public const int NEAREST = 0x2600;
		public const int LINEAR = 0x2601;
		public const int NEAREST_MIPMAP_NEAREST = 0x2700;
		public const int LINEAR_MIPMAP_NEAREST = 0x2701;
		public const int NEAREST_MIPMAP_LINEAR = 0x2702;
		public const int LINEAR_MIPMAP_LINEAR = 0x2703;
		public const int TEXTURE_MAG_FILTER = 0x2800;
		public const int TEXTURE_MIN_FILTER = 0x2801;
		public const int TEXTURE_WRAP_S = 0x2802;
		public const int TEXTURE_WRAP_T = 0x2803;
		public const int CLAMP = 0x2900;
		public const int REPEAT = 0x2901;
		public const int CLIENT_PIXEL_STORE_BIT = 0x00000001;
		public const int CLIENT_VERTEX_ARRAY_BIT = 0x00000002;
		public const uint CLIENT_ALL_ATTRIB_BITS = 0xffffffff;
		public const int POLYGON_OFFSET_FACTOR = 0x8038;
		public const int POLYGON_OFFSET_UNITS = 0x2A00;
		public const int POLYGON_OFFSET_POINT = 0x2A01;
		public const int POLYGON_OFFSET_LINE = 0x2A02;
		public const int POLYGON_OFFSET_FILL = 0x8037;
		public const int ALPHA4 = 0x803B;
		public const int ALPHA8 = 0x803C;
		public const int ALPHA12 = 0x803D;
		public const int ALPHA16 = 0x803E;
		public const int LUMINANCE4 = 0x803F;
		public const int LUMINANCE8 = 0x8040;
		public const int LUMINANCE12 = 0x8041;
		public const int LUMINANCE16 = 0x8042;
		public const int LUMINANCE4_ALPHA4 = 0x8043;
		public const int LUMINANCE6_ALPHA2 = 0x8044;
		public const int LUMINANCE8_ALPHA8 = 0x8045;
		public const int LUMINANCE12_ALPHA4 = 0x8046;
		public const int LUMINANCE12_ALPHA12 = 0x8047;
		public const int LUMINANCE16_ALPHA16 = 0x8048;
		public const int INTENSITY = 0x8049;
		public const int INTENSITY4 = 0x804A;
		public const int INTENSITY8 = 0x804B;
		public const int INTENSITY12 = 0x804C;
		public const int INTENSITY16 = 0x804D;
		public const int R3_G3_B2 = 0x2A10;
		public const int RGB4 = 0x804F;
		public const int RGB5 = 0x8050;
		public const int RGB8 = 0x8051;
		public const int RGB10 = 0x8052;
		public const int RGB12 = 0x8053;
		public const int RGB16 = 0x8054;
		public const int RGBA2 = 0x8055;
		public const int RGBA4 = 0x8056;
		public const int RGB5_A1 = 0x8057;
		public const int RGBA8 = 0x8058;
		public const int RGB10_A2 = 0x8059;
		public const int RGBA12 = 0x805A;
		public const int RGBA16 = 0x805B;
		public const int TEXTURE_RED_SIZE = 0x805C;
		public const int TEXTURE_GREEN_SIZE = 0x805D;
		public const int TEXTURE_BLUE_SIZE = 0x805E;
		public const int TEXTURE_ALPHA_SIZE = 0x805F;
		public const int TEXTURE_LUMINANCE_SIZE = 0x8060;
		public const int TEXTURE_INTENSITY_SIZE = 0x8061;
		public const int PROXY_TEXTURE_1D = 0x8063;
		public const int PROXY_TEXTURE_2D = 0x8064;
		public const int TEXTURE_PRIORITY = 0x8066;
		public const int TEXTURE_RESIDENT = 0x8067;
		public const int TEXTURE_BINDING_1D = 0x8068;
		public const int TEXTURE_BINDING_2D = 0x8069;
		public const int TEXTURE_BINDING_3D = 0x806A;
		public const int VERTEX_ARRAY = 0x8074;
		public const int NORMAL_ARRAY = 0x8075;
		public const int COLOR_ARRAY = 0x8076;
		public const int INDEX_ARRAY = 0x8077;
		public const int TEXTURE_COORD_ARRAY = 0x8078;
		public const int EDGE_FLAG_ARRAY = 0x8079;
		public const int VERTEX_ARRAY_SIZE = 0x807A;
		public const int VERTEX_ARRAY_TYPE = 0x807B;
		public const int VERTEX_ARRAY_STRIDE = 0x807C;
		public const int NORMAL_ARRAY_TYPE = 0x807E;
		public const int NORMAL_ARRAY_STRIDE = 0x807F;
		public const int COLOR_ARRAY_SIZE = 0x8081;
		public const int COLOR_ARRAY_TYPE = 0x8082;
		public const int COLOR_ARRAY_STRIDE = 0x8083;
		public const int INDEX_ARRAY_TYPE = 0x8085;
		public const int INDEX_ARRAY_STRIDE = 0x8086;
		public const int TEXTURE_COORD_ARRAY_SIZE = 0x8088;
		public const int TEXTURE_COORD_ARRAY_TYPE = 0x8089;
		public const int TEXTURE_COORD_ARRAY_STRIDE = 0x808A;
		public const int EDGE_FLAG_ARRAY_STRIDE = 0x808C;
		public const int VERTEX_ARRAY_POINTER = 0x808E;
		public const int NORMAL_ARRAY_POINTER = 0x808F;
		public const int COLOR_ARRAY_POINTER = 0x8090;
		public const int INDEX_ARRAY_POINTER = 0x8091;
		public const int TEXTURE_COORD_ARRAY_POINTER = 0x8092;
		public const int EDGE_FLAG_ARRAY_POINTER = 0x8093;
		public const int V2F = 0x2A20;
		public const int V3F = 0x2A21;
		public const int C4UB_V2F = 0x2A22;
		public const int C4UB_V3F = 0x2A23;
		public const int C3F_V3F = 0x2A24;
		public const int N3F_V3F = 0x2A25;
		public const int C4F_N3F_V3F = 0x2A26;
		public const int T2F_V3F = 0x2A27;
		public const int T4F_V4F = 0x2A28;
		public const int T2F_C4UB_V3F = 0x2A29;
		public const int T2F_C3F_V3F = 0x2A2A;
		public const int T2F_N3F_V3F = 0x2A2B;
		public const int T2F_C4F_N3F_V3F = 0x2A2C;
		public const int T4F_C4F_N3F_V4F = 0x2A2D;
		public const int BGR = 0x80E0;
		public const int BGRA = 0x80E1;
		public const int CONSTANT_COLOR = 0x8001;
		public const int ONE_MINUS_CONSTANT_COLOR = 0x8002;
		public const int CONSTANT_ALPHA = 0x8003;
		public const int ONE_MINUS_CONSTANT_ALPHA = 0x8004;
		public const int BLEND_COLOR = 0x8005;
		public const int FUNC_ADD = 0x8006;
		public const int MIN = 0x8007;
		public const int MAX = 0x8008;
		public const int BLEND_EQUATION = 0x8009;
		public const int FUNC_SUBTRACT = 0x800A;
		public const int FUNC_REVERSE_SUBTRACT = 0x800B;
		public const int COLOR_MATRIX = 0x80B1;
		public const int COLOR_MATRIX_STACK_DEPTH = 0x80B2;
		public const int MAX_COLOR_MATRIX_STACK_DEPTH = 0x80B3;
		public const int POST_COLOR_MATRIX_RED_SCALE = 0x80B4;
		public const int POST_COLOR_MATRIX_GREEN_SCALE = 0x80B5;
		public const int POST_COLOR_MATRIX_BLUE_SCALE = 0x80B6;
		public const int POST_COLOR_MATRIX_ALPHA_SCALE = 0x80B7;
		public const int POST_COLOR_MATRIX_RED_BIAS = 0x80B8;
		public const int POST_COLOR_MATRIX_GREEN_BIAS = 0x80B9;
		public const int POST_COLOR_MATRIX_BLUE_BIAS = 0x80BA;
		public const int POST_COLOR_MATRIX_ALPHA_BIAS = 0x80BB;
		public const int COLOR_TABLE = 0x80D0;
		public const int POST_CONVOLUTION_COLOR_TABLE = 0x80D1;
		public const int POST_COLOR_MATRIX_COLOR_TABLE = 0x80D2;
		public const int PROXY_COLOR_TABLE = 0x80D3;
		public const int PROXY_POST_CONVOLUTION_COLOR_TABLE = 0x80D4;
		public const int PROXY_POST_COLOR_MATRIX_COLOR_TABLE = 0x80D5;
		public const int COLOR_TABLE_SCALE = 0x80D6;
		public const int COLOR_TABLE_BIAS = 0x80D7;
		public const int COLOR_TABLE_FORMAT = 0x80D8;
		public const int COLOR_TABLE_WIDTH = 0x80D9;
		public const int COLOR_TABLE_RED_SIZE = 0x80DA;
		public const int COLOR_TABLE_GREEN_SIZE = 0x80DB;
		public const int COLOR_TABLE_BLUE_SIZE = 0x80DC;
		public const int COLOR_TABLE_ALPHA_SIZE = 0x80DD;
		public const int COLOR_TABLE_LUMINANCE_SIZE = 0x80DE;
		public const int COLOR_TABLE_INTENSITY_SIZE = 0x80DF;
		public const int CONVOLUTION_1D = 0x8010;
		public const int CONVOLUTION_2D = 0x8011;
		public const int SEPARABLE_2D = 0x8012;
		public const int CONVOLUTION_BORDER_MODE = 0x8013;
		public const int CONVOLUTION_FILTER_SCALE = 0x8014;
		public const int CONVOLUTION_FILTER_BIAS = 0x8015;
		public const int REDUCE = 0x8016;
		public const int CONVOLUTION_FORMAT = 0x8017;
		public const int CONVOLUTION_WIDTH = 0x8018;
		public const int CONVOLUTION_HEIGHT = 0x8019;
		public const int MAX_CONVOLUTION_WIDTH = 0x801A;
		public const int MAX_CONVOLUTION_HEIGHT = 0x801B;
		public const int POST_CONVOLUTION_RED_SCALE = 0x801C;
		public const int POST_CONVOLUTION_GREEN_SCALE = 0x801D;
		public const int POST_CONVOLUTION_BLUE_SCALE = 0x801E;
		public const int POST_CONVOLUTION_ALPHA_SCALE = 0x801F;
		public const int POST_CONVOLUTION_RED_BIAS = 0x8020;
		public const int POST_CONVOLUTION_GREEN_BIAS = 0x8021;
		public const int POST_CONVOLUTION_BLUE_BIAS = 0x8022;
		public const int POST_CONVOLUTION_ALPHA_BIAS = 0x8023;
		public const int CONSTANT_BORDER = 0x8151;
		public const int REPLICATE_BORDER = 0x8153;
		public const int CONVOLUTION_BORDER_COLOR = 0x8154;
		public const int MAX_ELEMENTS_VERTICES = 0x80E8;
		public const int MAX_ELEMENTS_INDICES = 0x80E9;
		public const int HISTOGRAM = 0x8024;
		public const int PROXY_HISTOGRAM = 0x8025;
		public const int HISTOGRAM_WIDTH = 0x8026;
		public const int HISTOGRAM_FORMAT = 0x8027;
		public const int HISTOGRAM_RED_SIZE = 0x8028;
		public const int HISTOGRAM_GREEN_SIZE = 0x8029;
		public const int HISTOGRAM_BLUE_SIZE = 0x802A;
		public const int HISTOGRAM_ALPHA_SIZE = 0x802B;
		public const int HISTOGRAM_LUMINANCE_SIZE = 0x802C;
		public const int HISTOGRAM_SINK = 0x802D;
		public const int MINMAX = 0x802E;
		public const int MINMAX_FORMAT = 0x802F;
		public const int MINMAX_SINK = 0x8030;
		public const int TABLE_TOO_LARGE = 0x8031;
		public const int UNSIGNED_BYTE_3_3_2 = 0x8032;
		public const int UNSIGNED_SHORT_4_4_4_4 = 0x8033;
		public const int UNSIGNED_SHORT_5_5_5_1 = 0x8034;
		public const int UNSIGNED_INT_8_8_8_8 = 0x8035;
		public const int UNSIGNED_INT_10_10_10_2 = 0x8036;
		public const int UNSIGNED_BYTE_2_3_3_REV = 0x8362;
		public const int UNSIGNED_SHORT_5_6_5 = 0x8363;
		public const int UNSIGNED_SHORT_5_6_5_REV = 0x8364;
		public const int UNSIGNED_SHORT_4_4_4_4_REV = 0x8365;
		public const int UNSIGNED_SHORT_1_5_5_5_REV = 0x8366;
		public const int UNSIGNED_INT_8_8_8_8_REV = 0x8367;
		public const int UNSIGNED_INT_2_10_10_10_REV = 0x8368;
		public const int RESCALE_NORMAL = 0x803A;
		public const int LIGHT_MODEL_COLOR_CONTROL = 0x81F8;
		public const int SINGLE_COLOR = 0x81F9;
		public const int SEPARATE_SPECULAR_COLOR = 0x81FA;
		public const int PACK_SKIP_IMAGES = 0x806B;
		public const int PACK_IMAGE_HEIGHT = 0x806C;
		public const int UNPACK_SKIP_IMAGES = 0x806D;
		public const int UNPACK_IMAGE_HEIGHT = 0x806E;
		public const int TEXTURE_3D = 0x806F;
		public const int PROXY_TEXTURE_3D = 0x8070;
		public const int TEXTURE_DEPTH = 0x8071;
		public const int TEXTURE_WRAP_R = 0x8072;
		public const int MAX_3D_TEXTURE_SIZE = 0x8073;
		public const int CLAMP_TO_EDGE = 0x812F;
		public const int MIRRORED_REPEAT = 0x8370;
		public const int TEXTURE_MIN_LOD = 0x813A;
		public const int TEXTURE_MAX_LOD = 0x813B;
		public const int TEXTURE_BASE_LEVEL = 0x813C;
		public const int TEXTURE_MAX_LEVEL = 0x813D;
		public const int SMOOTH_POINT_SIZE_RANGE = 0x0B12;
		public const int SMOOTH_POINT_SIZE_GRANULARITY = 0x0B13;
		public const int SMOOTH_LINE_WIDTH_RANGE = 0x0B22;
		public const int SMOOTH_LINE_WIDTH_GRANULARITY = 0x0B23;
		public const int ALIASED_POINT_SIZE_RANGE = 0x846D;
		public const int ALIASED_LINE_WIDTH_RANGE = 0x846E;
		public const int TEXTURE0_ARB = 0x84C0;
		public const int TEXTURE1_ARB = 0x84C1;
		public const int TEXTURE2_ARB = 0x84C2;
		public const int TEXTURE3_ARB = 0x84C3;
		public const int TEXTURE4_ARB = 0x84C4;
		public const int TEXTURE5_ARB = 0x84C5;
		public const int TEXTURE6_ARB = 0x84C6;
		public const int TEXTURE7_ARB = 0x84C7;
		public const int TEXTURE8_ARB = 0x84C8;
		public const int TEXTURE9_ARB = 0x84C9;
		public const int TEXTURE10_ARB = 0x84CA;
		public const int TEXTURE11_ARB = 0x84CB;
		public const int TEXTURE12_ARB = 0x84CC;
		public const int TEXTURE13_ARB = 0x84CD;
		public const int TEXTURE14_ARB = 0x84CE;
		public const int TEXTURE15_ARB = 0x84CF;
		public const int TEXTURE16_ARB = 0x84D0;
		public const int TEXTURE17_ARB = 0x84D1;
		public const int TEXTURE18_ARB = 0x84D2;
		public const int TEXTURE19_ARB = 0x84D3;
		public const int TEXTURE20_ARB = 0x84D4;
		public const int TEXTURE21_ARB = 0x84D5;
		public const int TEXTURE22_ARB = 0x84D6;
		public const int TEXTURE23_ARB = 0x84D7;
		public const int TEXTURE24_ARB = 0x84D8;
		public const int TEXTURE25_ARB = 0x84D9;
		public const int TEXTURE26_ARB = 0x84DA;
		public const int TEXTURE27_ARB = 0x84DB;
		public const int TEXTURE28_ARB = 0x84DC;
		public const int TEXTURE29_ARB = 0x84DD;
		public const int TEXTURE30_ARB = 0x84DE;
		public const int TEXTURE31_ARB = 0x84DF;
		public const int ACTIVE_TEXTURE_ARB = 0x84E0;
		public const int CLIENT_ACTIVE_TEXTURE_ARB = 0x84E1;
		public const int MAX_TEXTURE_UNITS_ARB = 0x84E2;
		public const int ABGR_EXT = 0x8000;
		public const int CONSTANT_COLOR_EXT = 0x8001;
		public const int ONE_MINUS_CONSTANT_COLOR_EXT = 0x8002;
		public const int CONSTANT_ALPHA_EXT = 0x8003;
		public const int ONE_MINUS_CONSTANT_ALPHA_EXT = 0x8004;
		public const int BLEND_COLOR_EXT = 0x8005;
		public const int FUNC_ADD_EXT = 0x8006;
		public const int MIN_EXT = 0x8007;
		public const int MAX_EXT = 0x8008;
		public const int BLEND_EQUATION_EXT = 0x8009;
		public const int FUNC_SUBTRACT_EXT = 0x800A;
		public const int FUNC_REVERSE_SUBTRACT_EXT = 0x800B;
		public const int COMBINE_EXT = 0x8570;
		public const int COMBINE_RGB_EXT = 0x8571;
		public const int COMBINE_ALPHA_EXT = 0x8572;
		public const int RGB_SCALE_EXT = 0x8573;
		public const int ADD_SIGNED_EXT = 0x8574;
		public const int INTERPOLATE_EXT = 0x8575;
		public const int CONSTANT_EXT = 0x8576;
		public const int PRIMARY_COLOR_EXT = 0x8577;
		public const int PREVIOUS_EXT = 0x8578;
		public const int SOURCE0_RGB_EXT = 0x8580;
		public const int SOURCE1_RGB_EXT = 0x8581;
		public const int SOURCE2_RGB_EXT = 0x8582;
		public const int SOURCE0_ALPHA_EXT = 0x8588;
		public const int SOURCE1_ALPHA_EXT = 0x8589;
		public const int SOURCE2_ALPHA_EXT = 0x858A;
		public const int OPERAND0_RGB_EXT = 0x8590;
		public const int OPERAND1_RGB_EXT = 0x8591;
		public const int OPERAND2_RGB_EXT = 0x8592;
		public const int OPERAND0_ALPHA_EXT = 0x8598;
		public const int OPERAND1_ALPHA_EXT = 0x8599;
		public const int OPERAND2_ALPHA_EXT = 0x859A;
		public const int LOGIC_OP = INDEX_LOGIC_OP;
		public const int TEXTURE_COMPONENTS = TEXTURE_INTERNAL_FORMAT;
		public const int ARRAY_BUFFER_ARB = 0x8892;
		public const int ELEMENT_ARRAY_BUFFER_ARB = 0x8893;
		public const int ARRAY_BUFFER_BINDING_ARB = 0x8894;
		public const int ELEMENT_ARRAY_BUFFER_BINDING_ARB = 0x8895;
		public const int VERTEX_ARRAY_BUFFER_BINDING_ARB = 0x8896;
		public const int NORMAL_ARRAY_BUFFER_BINDING_ARB = 0x8897;
		public const int COLOR_ARRAY_BUFFER_BINDING_ARB = 0x8898;
		public const int INDEX_ARRAY_BUFFER_BINDING_ARB = 0x8899;
		public const int TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB = 0x889A;
		public const int EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB = 0x889B;
		public const int SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB = 0x889C;
		public const int FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB = 0x889D;
		public const int WEIGHT_ARRAY_BUFFER_BINDING_ARB = 0x889E;
		public const int VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB = 0x889F;
		public const int STREAM_DRAW_ARB = 0x88E0;
		public const int STREAM_READ_ARB = 0x88E1;
		public const int STREAM_COPY_ARB = 0x88E2;
		public const int STATIC_DRAW_ARB = 0x88E4;
		public const int STATIC_READ_ARB = 0x88E5;
		public const int STATIC_COPY_ARB = 0x88E6;
		public const int DYNAMIC_DRAW_ARB = 0x88E8;
		public const int DYNAMIC_READ_ARB = 0x88E9;
		public const int DYNAMIC_COPY_ARB = 0x88EA;
		public const int READ_ONLY_ARB = 0x88B8;
		public const int WRITE_ONLY_ARB = 0x88B9;
		public const int READ_WRITE_ARB = 0x88BA;
		public const int BUFFER_SIZE_ARB = 0x8764;
		public const int BUFFER_USAGE_ARB = 0x8765;
		public const int BUFFER_ACCESS_ARB = 0x88BB;
		public const int BUFFER_MAPPED_ARB = 0x88BC;
		public const int BUFFER_MAP_POINTER_ARB = 0x88BD;
		public const int SAMPLES_PASSED_ARB = 0x8914;
		public const int QUERY_COUNTER_BITS_ARB = 0x8864;
		public const int CURRENT_QUERY_ARB = 0x8865;
		public const int QUERY_RESULT_ARB = 0x8866;
		public const int QUERY_RESULT_AVAILABLE_ARB = 0x8867;
		public const int FRAGMENT_PROGRAM_ARB = 0x8804;
		public const int VERTEX_PROGRAM_ARB = 0x8620;
		public const int PROGRAM_ERROR_POSITION_ARB = 0x864B;
		public const int PROGRAM_ERROR_STRING_ARB = 0x8874;
		public const int VERTEX_PROGRAM_POINT_SIZE_ARB = 0x8642;
		public const int VERTEX_PROGRAM_TWO_SIDE_ARB = 0x8643;
		public const int COLOR_SUM_ARB = 0x8458;
		public const int PROGRAM_FORMAT_ASCII_ARB = 0x8875;
		public const int PROGRAM_LENGTH_ARB = 0x8627;
		public const int PROGRAM_FORMAT_ARB = 0x8876;
		public const int PROGRAM_BINDING_ARB = 0x8677;
		public const int PROGRAM_INSTRUCTIONS_ARB = 0x88A0;
		public const int MAX_PROGRAM_INSTRUCTIONS_ARB = 0x88A1;
		public const int PROGRAM_NATIVE_INSTRUCTIONS_ARB = 0x88A2;
		public const int MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB = 0x88A3;
		public const int PROGRAM_TEMPORARIES_ARB = 0x88A4;
		public const int MAX_PROGRAM_TEMPORARIES_ARB = 0x88A5;
		public const int PROGRAM_NATIVE_TEMPORARIES_ARB = 0x88A6;
		public const int MAX_PROGRAM_NATIVE_TEMPORARIES_ARB = 0x88A7;
		public const int PROGRAM_PARAMETERS_ARB = 0x88A8;
		public const int MAX_PROGRAM_PARAMETERS_ARB = 0x88A9;
		public const int PROGRAM_NATIVE_PARAMETERS_ARB = 0x88AA;
		public const int MAX_PROGRAM_NATIVE_PARAMETERS_ARB = 0x88AB;
		public const int PROGRAM_ATTRIBS_ARB = 0x88AC;
		public const int MAX_PROGRAM_ATTRIBS_ARB = 0x88AD;
		public const int PROGRAM_NATIVE_ATTRIBS_ARB = 0x88AE;
		public const int MAX_PROGRAM_NATIVE_ATTRIBS_ARB = 0x88AF;
		public const int PROGRAM_ADDRESS_REGISTERS_ARB = 0x88B0;
		public const int MAX_PROGRAM_ADDRESS_REGISTERS_ARB = 0x88B1;
		public const int PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB = 0x88B2;
		public const int MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB = 0x88B3;
		public const int MAX_PROGRAM_LOCAL_PARAMETERS_ARB = 0x88B4;
		public const int MAX_PROGRAM_ENV_PARAMETERS_ARB = 0x88B5;
		public const int PROGRAM_UNDER_NATIVE_LIMITS_ARB = 0x88B6;
		public const int VERTEX_ATTRIB_ARRAY_ENABLED_ARB = 0x8622;
		public const int VERTEX_ATTRIB_ARRAY_SIZE_ARB = 0x8623;
		public const int VERTEX_ATTRIB_ARRAY_STRIDE_ARB = 0x8624;
		public const int VERTEX_ATTRIB_ARRAY_TYPE_ARB = 0x8625;
		public const int VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB = 0x886A;
		public const int CURRENT_VERTEX_ATTRIB_ARB = 0x8626;
		public const int VERTEX_ATTRIB_ARRAY_POINTER_ARB = 0x8645;
		public const int PROGRAM_STRING_ARB = 0x8628;
		public const int CURRENT_MATRIX_ARB = 0x8641;
		public const int TRANSPOSE_CURRENT_MATRIX_ARB = 0x88B7;
		public const int CURRENT_MATRIX_STACK_DEPTH_ARB = 0x8640;
		public const int MAX_VERTEX_ATTRIBS_ARB = 0x8869;
		public const int MAX_PROGRAM_MATRICES_ARB = 0x862F;
		public const int MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB = 0x862E;
		public const int MATRIX0_ARB = 0x88C0;
		public const int MATRIX1_ARB = 0x88C1;
		public const int MATRIX2_ARB = 0x88C2;
		public const int MATRIX3_ARB = 0x88C3;
		public const int MATRIX4_ARB = 0x88C4;
		public const int MATRIX5_ARB = 0x88C5;
		public const int MATRIX6_ARB = 0x88C6;
		public const int MATRIX7_ARB = 0x88C7;
		public const int MATRIX8_ARB = 0x88C8;
		public const int MATRIX9_ARB = 0x88C9;
		public const int MATRIX10_ARB = 0x88CA;
		public const int MATRIX11_ARB = 0x88CB;
		public const int MATRIX12_ARB = 0x88CC;
		public const int MATRIX13_ARB = 0x88CD;
		public const int MATRIX14_ARB = 0x88CE;
		public const int MATRIX15_ARB = 0x88CF;
		public const int MATRIX16_ARB = 0x88D0;
		public const int MATRIX17_ARB = 0x88D1;
		public const int MATRIX18_ARB = 0x88D2;
		public const int MATRIX19_ARB = 0x88D3;
		public const int MATRIX20_ARB = 0x88D4;
		public const int MATRIX21_ARB = 0x88D5;
		public const int MATRIX22_ARB = 0x88D6;
		public const int MATRIX23_ARB = 0x88D7;
		public const int MATRIX24_ARB = 0x88D8;
		public const int MATRIX25_ARB = 0x88D9;
		public const int MATRIX26_ARB = 0x88DA;
		public const int MATRIX27_ARB = 0x88DB;
		public const int MATRIX28_ARB = 0x88DC;
		public const int MATRIX29_ARB = 0x88DD;
		public const int MATRIX30_ARB = 0x88DE;
		public const int MATRIX31_ARB = 0x88DF;
		public const int INCR_WRAP_EXT = 0x8507;
		public const int GL_DECR_WRAP_EXT = 8508;
		public const int COMPRESSED_ALPHA_ARB = 0x84E9;
		public const int COMPRESSED_LUMINANCE_ARB = 0x84EA;
		public const int COMPRESSED_LUMINANCE_ALPHA_ARB = 0x84EB;
		public const int COMPRESSED_INTENSITY_ARB = 0x84EC;
		public const int COMPRESSED_RGB_ARB = 0x84ED;
		public const int COMPRESSED_RGBA_ARB = 0x84EE;
		public const int TEXTURE_COMPRESSION_HINT_ARB = 0x84EF;
		public const int TEXTURE_COMPRESSED_IMAGE_SIZE_ARB = 0x86A0;
		public const int TEXTURE_COMPRESSED_ARB = 0x86A1;
		public const int NUM_COMPRESSED_TEXTURE_FORMATS_ARB = 0x86A2;
		public const int COMPRESSED_TEXTURE_FORMATS_ARB = 0x86A3;
		public const int TEXTURE_FILTER_CONTROL_EXT = 0x8500;
		public const int TEXTURE_LOD_BIAS_EXT = 0x8501;
		public const int MAX_TEXTURE_LOD_BIAS_EXT = 0x84FD;
		public const int TEXTURE_MIN_LOD_SGIS = 0x813A;
		public const int EXTURE_MAX_LOD_SGIS = 0x813B;
		public const int TEXTURE_BASE_LEVEL_SGIS = 0x813C;
		public const int TEXTURE_MAX_LEVEL_SGIS = 0x813D;
		public const int GENERATE_MIPMAP_SGIS = 0x8191;
		public const int GL_MIRRORED_REPEAT_ARB = 0x8370;
		public const int DOT3_RGB_ARB = 0x86AE;
		public const int DOT3_RGBA_ARB = 0x86AF;
		public const int TEXTURE_3D_ARB = 0x806F;
		public const int TEXTURE_WRAP_R_ARB = 0x8072;
		public const int PROGRAM_OBJECT_ARB = 0x8B40;
		public const int OBJECT_TYPE_ARB = 0x8B4E;
		public const int OBJECT_SUBTYPE_ARB = 0x8B4F;
		public const int SHADER_OBJECT_ARB = 0x8B48;
		public const int FLOAT_VEC2_ARB = 0x8B50;
		public const int FLOAT_VEC3_ARB = 0x8B51;
		public const int FLOAT_VEC4_ARB = 0x8B52;
		public const int INT_VEC2_ARB = 0x8B53;
		public const int INT_VEC3_ARB = 0x8B54;
		public const int INT_VEC4_ARB = 0x8B55;
		public const int BOOL_ARB = 0x8B56;
		public const int BOOL_VEC2_ARB = 0x8B57;
		public const int BOOL_VEC3_ARB = 0x8B58;
		public const int BOOL_VEC4_ARB = 0x8B59;
		public const int FLOAT_MAT2_ARB = 0x8B5A;
		public const int FLOAT_MAT3_ARB = 0x8B5B;
		public const int FLOAT_MAT4_ARB = 0x8B5C;
		public const int VERTEX_SHADER_ARB = 0x8B31;
		public const int MAX_VERTEX_UNIFORM_COMPONENTS_ARB = 0x8B4A;
		public const int MAX_TEXTURE_IMAGE_UNITS_ARB = 0x8872;
		public const int MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB = 0x8B4C;
		public const int MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB = 0x8B4D;
		public const int MAX_TEXTURE_COORDS_ARB = 0x8871;
		public const int FRAGMENT_SHADER_ARB = 0x8B30;
		public const int MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB = 0x8B49;
		public const int MAX_VARYING_FLOATS_ARB = 0x8B4B;
		public const int OBJECT_DELETE_STATUS_ARB = 0x8B80;
		public const int OBJECT_COMPILE_STATUS_ARB = 0x8B81;
		public const int OBJECT_LINK_STATUS_ARB = 0x8B82;
		public const int OBJECT_VALIDATE_STATUS_ARB = 0x8B83;
		public const int OBJECT_INFO_LOG_LENGTH_ARB = 0x8B84;
		public const int OBJECT_ATTACHED_OBJECTS_ARB = 0x8B85;
		public const int OBJECT_ACTIVE_UNIFORMS_ARB = 0x8B86;
		public const int OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB = 0x8B87;
		public const int OBJECT_SHADER_SOURCE_LENGTH_ARB = 0x8B88;
		public const int OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB = 0x8B8A;
		public const int OBJECT_ACTIVE_ATTRIBUTES_ARB = 0x8B89;
		public const int NORMAL_MAP_ARB = 0x8511;
		public const int REFLECTION_MAP_ARB = 0x8512;
		public const int TEXTURE_CUBE_MAP_ARB = 0x8513;
		public const int TEXTURE_BINDING_CUBE_MAP_ARB = 0x8514;
		public const int TEXTURE_CUBE_MAP_POSITIVE_X_ARB = 0x8515;
		public const int TEXTURE_CUBE_MAP_NEGATIVE_X_ARB = 0x8516;
		public const int TEXTURE_CUBE_MAP_POSITIVE_Y_ARB = 0x8517;
		public const int TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB = 0x8518;
		public const int TEXTURE_CUBE_MAP_POSITIVE_Z_ARB = 0x8519;
		public const int TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB = 0x851A;
		public const int PROXY_TEXTURE_CUBE_MAP_ARB = 0x851B;
		public const int MAX_CUBE_MAP_TEXTURE_SIZE_ARB = 0x851C;
		public const int FRAMEBUFFER_EXT = 0x8D40;
		public const int RENDERBUFFER_EXT = 0x8D41;
		public const int STENCIL_INDEX_EXT = 0x8D45;
		public const int STENCIL_INDEX1_EXT = 0x8D46;
		public const int STENCIL_INDEX4_EXT = 0x8D47;
		public const int STENCIL_INDEX8_EXT = 0x8D48;
		public const int STENCIL_INDEX16_EXT = 0x8D49;
		public const int RENDERBUFFER_WIDTH_EXT = 0x8D42;
		public const int RENDERBUFFER_HEIGHT_EXT = 0x8D43;
		public const int RENDERBUFFER_INTERNAL_FORMAT_EXT = 0x8D44;
		public const int FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT = 0x8CD0;
		public const int FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT = 0x8CD1;
		public const int FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT = 0x8CD2;
		public const int FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT = 0x8CD3;
		public const int FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT = 0x8CD4;
		public const int COLOR_ATTACHMENT0_EXT = 0x8CE0;
		public const int COLOR_ATTACHMENT1_EXT = 0x8CE1;
		public const int COLOR_ATTACHMENT2_EXT = 0x8CE2;
		public const int COLOR_ATTACHMENT3_EXT = 0x8CE3;
		public const int COLOR_ATTACHMENT4_EXT = 0x8CE4;
		public const int COLOR_ATTACHMENT5_EXT = 0x8CE5;
		public const int COLOR_ATTACHMENT6_EXT = 0x8CE6;
		public const int COLOR_ATTACHMENT7_EXT = 0x8CE7;
		public const int COLOR_ATTACHMENT8_EXT = 0x8CE8;
		public const int COLOR_ATTACHMENT9_EXT = 0x8CE9;
		public const int COLOR_ATTACHMENT10_EXT = 0x8CEA;
		public const int COLOR_ATTACHMENT11_EXT = 0x8CEB;
		public const int COLOR_ATTACHMENT12_EXT = 0x8CEC;
		public const int COLOR_ATTACHMENT13_EXT = 0x8CED;
		public const int COLOR_ATTACHMENT14_EXT = 0x8CEE;
		public const int COLOR_ATTACHMENT15_EXT = 0x8CEF;
		public const int DEPTH_ATTACHMENT_EXT = 0x8D00;
		public const int STENCIL_ATTACHMENT_EXT = 0x8D20;
		public const int FRAMEBUFFER_COMPLETE_EXT = 0x8CD5;
		public const int FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT = 0x8CD6;
		public const int FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT = 0x8CD7;
		public const int FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT = 0x8CD8;
		public const int FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT = 0x8CD9;
		public const int FRAMEBUFFER_INCOMPLETE_FORMATS_EXT = 0x8CDA;
		public const int FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT = 0x8CDB;
		public const int FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT = 0x8CDC;
		public const int FRAMEBUFFER_UNSUPPORTED_EXT = 0x8CDD;
		public const int FRAMEBUFFER_STATUS_ERROR_EXT = 0x8CDE;
		public const int FRAMEBUFFER_BINDING_EXT = 0x8CA6;
		public const int RENDERBUFFER_BINDING_EXT = 0x8CA7;
		public const int MAX_COLOR_ATTACHMENTS_EXT = 0x8CDF;
		public const int MAX_RENDERBUFFER_SIZE_EXT = 0x84E8;
		public const int INVALID_FRAMEBUFFER_OPERATION_EXT = 0x0506;

        public const int BGR_EXT  = 0x80E0;
        public const int BGRA_EXT = 0x80E1;

		[DllImport(DLLName, EntryPoint = "glAccum")]
		public static extern void Accum(int op, float value);
        
		[DllImport(DLLName, EntryPoint = "glAlphaFunc")]
		public static extern void AlphaFunc(int func, float refval);

		[DllImport(DLLName, EntryPoint = "glAreTexturesResident")]
		public static extern bool AreTexturesResident(int n, uint[] textures, bool[] residences);

		[DllImport(DLLName, EntryPoint = "glArrayElement")]
		public static extern void ArrayElement(int i);

		[DllImport(DLLName, EntryPoint = "glBegin")]
		public static extern void Begin(int mode);

		[DllImport(DLLName, EntryPoint = "glBindTexture")]
//		public static extern void BindTexture(int target, int texture);
        public static extern void BindTexture(int target, uint texture);

		[DllImport(DLLName, EntryPoint = "glBitmap")]
		public static extern void Bitmap(int width, int height, float xorig, float yorig, float xmove, float ymove, byte[] bitmap);

		[DllImport(DLLName, EntryPoint = "glBlendFunc")]
		public static extern void BlendFunc(int sfactor, int dfactor);

		[DllImport(DLLName, EntryPoint = "glCallList")]
		public static extern void CallList(uint list);

		[DllImport(DLLName, EntryPoint = "glCallLists")]
		public static extern void CallLists(int n, int type, int[] lists);

		[DllImport(DLLName, EntryPoint = "glCallLists")]
		public static extern void CallLists(int n, int type, string lists);

		[DllImport(DLLName, EntryPoint = "glClear")]
		public static extern void Clear(int mask);

		[DllImport(DLLName, EntryPoint = "glClearAccum")]
		public static extern void ClearAccum(float red, float green, float blue, float alpha);

		[DllImport(DLLName, EntryPoint = "glClearColor")]
		public static extern void ClearColor(float red, float green, float blue, float alpha);

		[DllImport(DLLName, EntryPoint = "glClearDepth")]
		public static extern void ClearDepth(double depth);

		[DllImport(DLLName, EntryPoint = "glClearIndex")]
		public static extern void ClearIndex(float c);

		[DllImport(DLLName, EntryPoint = "glClearStencil")]
		public static extern void ClearStencil(int s);

		[DllImport(DLLName, EntryPoint = "glClipPlane")]
		public static extern void ClipPlane(int plane, double[] equation);

		[DllImport(DLLName, EntryPoint = "glColor3b")]
		public static extern void Color3b(sbyte red, sbyte green, sbyte blue);

		[DllImport(DLLName, EntryPoint = "glColor3bv")]
		public static extern void Color3bv(ref sbyte v);

		[DllImport(DLLName, EntryPoint = "glColor3d")]
		public static extern void Color3d(double red, double green, double blue);

		[DllImport(DLLName, EntryPoint = "glColor3dv")]
		public static extern void Color3dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glColor3f")]
		public static extern void Color3f(float red, float green, float blue);

		[DllImport(DLLName, EntryPoint = "glColor3fv")]
		public static extern void Color3fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glColor3i")]
		public static extern void Color3i(int red, int green, int blue);

		[DllImport(DLLName, EntryPoint = "glColor3iv")]
		public static extern void Color3iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glColor3s")]
		public static extern void Color3s(short red, short green, short blue);

		[DllImport(DLLName, EntryPoint = "glColor3sv")]
		public static extern void Color3sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glColor3ub")]
		public static extern void Color3ub(byte red, byte green, byte blue);

		[DllImport(DLLName, EntryPoint = "glColor3ubv")]
		public static extern void Color3ubv(byte[] v);

		[DllImport(DLLName, EntryPoint = "glColor3ui")]
		public static extern void Color3ui(uint red, uint green, uint blue);

		[DllImport(DLLName, EntryPoint = "glColor3uiv")]
		public static extern void Color3uiv(uint[] v);

		[DllImport(DLLName, EntryPoint = "glColor3us")]
		public static extern void Color3us(short red, ushort green, ushort blue);

		[DllImport(DLLName, EntryPoint = "glColor3usv")]
		public static extern void Color3usv(short[] v);

		[DllImport(DLLName, EntryPoint = "glColor4b")]
		public static extern void Color4b(sbyte red, sbyte green, sbyte blue, sbyte alpha);

		[DllImport(DLLName, EntryPoint = "glColor4bv")]
		public static extern void Color4bv(sbyte[] v);

		[DllImport(DLLName, EntryPoint = "glColor4d")]
		public static extern void Color4d(double red, double green, double blue, double alpha);

		[DllImport(DLLName, EntryPoint = "glColor4dv")]
		public static extern void Color4dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glColor4f")]
		public static extern void Color4f(float red, float green, float blue, float alpha);

		[DllImport(DLLName, EntryPoint = "glColor4fv")]
		public static extern void Color4fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glColor4i")]
		public static extern void Color4i(int red, int green, int blue, int alpha);

		[DllImport(DLLName, EntryPoint = "glColor4iv")]
		public static extern void Color4iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glColor4s")]
		public static extern void Color4s(short red, short green, short blue, short alpha);

		[DllImport(DLLName, EntryPoint = "glColor4sv")]
		public static extern void Color4sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glColor4ub")]
		public static extern void Color4ub(byte red, byte green, byte blue, byte alpha);

		[DllImport(DLLName, EntryPoint = "glColor4ubv")]
		public static extern void Color4ubv(byte[] v);

		[DllImport(DLLName, EntryPoint = "glColor4ui")]
		public static extern void Color4ui(uint red, uint green, uint blue, uint alpha);

		[DllImport(DLLName, EntryPoint = "glColor4uiv")]
		public static extern void Color4uiv(uint[] v);

		[DllImport(DLLName, EntryPoint = "glColor4us")]
		public static extern void Color4us(ushort red, ushort green, ushort blue, ushort alpha);

		[DllImport(DLLName, EntryPoint = "glColor4usv")]
		public static extern void Color4usv(ushort[] v);

		[DllImport(DLLName, EntryPoint = "glColorMask")]
		public static extern void ColorMask(bool red, bool green, bool blue, bool alpha);

		[DllImport(DLLName, EntryPoint = "glColorMaterial")]
		public static extern void ColorMaterial(int face, int mode);

		[DllImport(DLLName, EntryPoint = "glColorPointer")]
		public static extern void ColorPointer(int size, int type, int stride, IntPtr pointer);

		[DllImport(DLLName, EntryPoint = "glColorPointer")]
		public static extern void ColorPointer(int size, int type, int stride, int offset);

		[DllImport(DLLName, EntryPoint = "glCopyPixels")]
		public static extern void CopyPixels(int x, int y, int width, int height, int type);

		[DllImport(DLLName, EntryPoint = "glCopyTexImage1D")]
		public static extern void CopyTexImage1D(int target, int level, int internalformat, int x, int y, int width, int border);

		[DllImport(DLLName, EntryPoint = "glCopyTexImage2D")]
		public static extern void CopyTexImage2D(int target, int level, int internalformat, int x, int y, int width, int height, int border);

		[DllImport(DLLName, EntryPoint = "glCopyTexSubImage1D")]
		public static extern void CopyTexSubImage1D(int target, int level, int xoffset, int x, int y, int width);

		[DllImport(DLLName, EntryPoint = "glCopyTexSubImage2D")]
		public static extern void CopyTexSubImage2D(int target, int level, int xoffset, int yoffset, int x, int y, int width, int height);

		[DllImport(DLLName, EntryPoint = "glCullFace")]
		public static extern void CullFace(int mode);

		[DllImport(DLLName, EntryPoint = "glDeleteLists")]
		public static extern void DeleteLists(uint list, int range);

		[DllImport(DLLName, EntryPoint = "glDeleteTextures")]
		public static extern void DeleteTextures(int n, int[] textures);

		[DllImport(DLLName, EntryPoint = "glDeleteTextures")]
		public static extern void DeleteTextures(int n, ref int texture);

		[DllImport(DLLName, EntryPoint = "glDepthFunc")]
		public static extern void DepthFunc(int func);

		[DllImport(DLLName, EntryPoint = "glDepthMask")]
		public static extern void DepthMask(bool flag);

		[DllImport(DLLName, EntryPoint = "glDepthRange")]
		public static extern void DepthRange(double zNear, double zFar);

		[DllImport(DLLName, EntryPoint = "glDisable")]
		public static extern void Disable(int cap);

		[DllImport(DLLName, EntryPoint = "glDisableClientState")]
		public static extern void DisableClientState(int array);

		[DllImport(DLLName, EntryPoint = "glDrawArrays")]
		public static extern void DrawArrays(int mode, int first, int count);

		[DllImport(DLLName, EntryPoint = "glDrawBuffer")]
		public static extern void DrawBuffer(int mode);

		[DllImport(DLLName, EntryPoint = "glDrawElements")]
		public static extern void DrawElements(int mode, int count, int type, int[] indices);

		[DllImport(DLLName, EntryPoint = "glDrawElements")]
		public static extern void DrawElements(int mode, int count, int type, IntPtr indices);

		[DllImport(DLLName, EntryPoint = "glDrawElements")]
		public static extern void DrawElements(int mode, int count, int type, int offset);

		[DllImport(DLLName, EntryPoint = "glDrawPixels")]
//		public static extern void DrawPixels(int width, int height, int format, int type, IntPtr pixels);
        public static extern void DrawPixels(int width, int height, int format, int type, byte[] pixels);
		[DllImport(DLLName, EntryPoint = "glEdgeFlag")]
		public static extern void EdgeFlag(bool flag);

		[DllImport(DLLName, EntryPoint = "glEdgeFlagPointer")]
		public static extern void EdgeFlagPointer(int stride, IntPtr pointer);

		[DllImport(DLLName, EntryPoint = "glEdgeFlagPointer")]
		public static extern void EdgeFlagPointer(int stride, int offset);

		[DllImport(DLLName, EntryPoint = "glEdgeFlagv")]
		public static extern void EdgeFlagv(bool[] flag);

		[DllImport(DLLName, EntryPoint = "glEnable")]
		public static extern void Enable(int cap);

		[DllImport(DLLName, EntryPoint = "glEnableClientState")]
		public static extern void EnableClientState(int array);

		[DllImport(DLLName, EntryPoint = "glEnd")]
		public static extern void End();

		[DllImport(DLLName, EntryPoint = "glEndList")]
		public static extern void EndList();

		[DllImport(DLLName, EntryPoint = "glEvalCoord1d")]
		public static extern void EvalCoord1d(double u);

		[DllImport(DLLName, EntryPoint = "glEvalCoord1dv")]
		public static extern void EvalCoord1dv(double[] u);

		[DllImport(DLLName, EntryPoint = "glEvalCoord1f")]
		public static extern void EvalCoord1f(float u);

		[DllImport(DLLName, EntryPoint = "glEvalCoord1fv")]
		public static extern void EvalCoord1fv(float[] u);

		[DllImport(DLLName, EntryPoint = "glEvalCoord2d")]
		public static extern void EvalCoord2d(double u, double v);

		[DllImport(DLLName, EntryPoint = "glEvalCoord2dv")]
		public static extern void EvalCoord2dv(double[] u);

		[DllImport(DLLName, EntryPoint = "glEvalCoord2f")]
		public static extern void EvalCoord2f(float u, float v);

		[DllImport(DLLName, EntryPoint = "glEvalCoord2fv")]
		public static extern void EvalCoord2fv(ref float u);

		[DllImport(DLLName, EntryPoint = "glEvalMesh1")]
		public static extern void EvalMesh1(int mode, int i1, int i2);

		[DllImport(DLLName, EntryPoint = "glEvalMesh2")]
		public static extern void EvalMesh2(int mode, int i1, int i2, int j1, int j2);

		[DllImport(DLLName, EntryPoint = "glEvalPoint1")]
		public static extern void EvalPoint1(int i);

		[DllImport(DLLName, EntryPoint = "glEvalPoint2")]
		public static extern void EvalPoint2(int i, int j);

		[DllImport(DLLName, EntryPoint = "glFeedbackBuffer")]
		public static extern void FeedbackBuffer(int size, int type, float[] buffer);

		[DllImport(DLLName, EntryPoint = "glFinish")]
		public static extern void Finish();

		[DllImport(DLLName, EntryPoint = "glFlush")]
		public static extern void Flush();

		[DllImport(DLLName, EntryPoint = "glFogf")]
		public static extern void Fogf(int pname, float param);

		[DllImport(DLLName, EntryPoint = "glFogfv")]
		public static extern void Fogfv(int pname, float[] fogparams);

		[DllImport(DLLName, EntryPoint = "glFogi")]
		public static extern void Fogi(int pname, int param);

		[DllImport(DLLName, EntryPoint = "glFogiv")]
		public static extern void Fogiv(int pname, int[] fogparams);

		[DllImport(DLLName, EntryPoint = "glFrontFace")]
		public static extern void FrontFace(int mode);

		[DllImport(DLLName, EntryPoint = "glFrustum")]
		public static extern void Frustum(double left, double right, double bottom, double top, double zNear, double zFar);

		[DllImport(DLLName, EntryPoint = "glGenLists")]
		public static extern uint GenLists(int range);

		[DllImport(DLLName, EntryPoint = "glGenTextures")]
		public static extern void GenTextures(int n, uint[] textures);

		[DllImport(DLLName, EntryPoint = "glGenTextures")]
		public static extern void GenTextures(int n, ref uint texture);

		[DllImport(DLLName, EntryPoint = "glGetBooleanv")]
		public static extern void GetBooleanv(int pname, bool[] bparams);

		[DllImport(DLLName, EntryPoint = "glGetClipPlane")]
		public static extern void GetClipPlane(int plane, double[] equation);

		[DllImport(DLLName, EntryPoint = "glGetDoublev")]
		public static extern void GetDoublev(int pname, double[] dparams);

		[DllImport(DLLName, EntryPoint = "glGetError")]
		public static extern int GetError();

		[DllImport(DLLName, EntryPoint = "glGetFloatv")]
		public static extern void GetFloatv(int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glGetIntegerv")]
		public static extern void GetIntegerv(int pname, int[] fparams);

		[DllImport(DLLName, EntryPoint = "glGetLightfv")]
		public static extern void GetLightfv(int light, int pname, float[] lparams);

		[DllImport(DLLName, EntryPoint = "glGetLightiv")]
		public static extern void GetLightiv(int light, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glGetMapdv")]
		public static extern void GetMapdv(int target, int query, double[] v);

		[DllImport(DLLName, EntryPoint = "glGetMapfv")]
		public static extern void GetMapfv(int target, int query, float[] v);

		[DllImport(DLLName, EntryPoint = "glGetMapiv")]
		public static extern void GetMapiv(int target, int query, int[] v);

		[DllImport(DLLName, EntryPoint = "glGetMaterialfv")]
		public static extern void GetMaterialfv(int face, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glGetMaterialiv")]
		public static extern void GetMaterialiv(int face, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glGetPixelMapfv")]
		public static extern void GetPixelMapfv(int map, float[] values);

		[DllImport(DLLName, EntryPoint = "glGetPixelMapuiv")]
		public static extern void GetPixelMapuiv(int map, uint[] values);

		[DllImport(DLLName, EntryPoint = "glGetPixelMapusv")]
		public static extern void GetPixelMapusv(int map, ushort[] values);

		[DllImport(DLLName, EntryPoint = "glGetPointerv")]
		public static extern void GetPointerv(int pname, ref int param);

		[DllImport(DLLName, EntryPoint = "glGetPolygonStipple")]
		public static extern void GetPolygonStipple(byte[] mask);

		[DllImport(DLLName, EntryPoint = "glGetString")]
		private static extern IntPtr GetString_(int name);

		[DllImport(DLLName, EntryPoint = "glGetTexEnvfv")]
		public static extern void GetTexEnvfv(int target, int pname, float[] envparams);

		[DllImport(DLLName, EntryPoint = "glGetTexEnviv")]
		public static extern void GetTexEnviv(int target, int pname, int[] envparams);

		[DllImport(DLLName, EntryPoint = "glGetTexGendv")]
		public static extern void GetTexGendv(int coord, int pname, double[] dparams);

		[DllImport(DLLName, EntryPoint = "glGetTexGenfv")]
		public static extern void GetTexGenfv(int coord, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glGetTexGeniv")]
		public static extern void GetTexGeniv(int coord, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glGetTexImage")]
		public static extern void GetTexImage(int target, int level, int format, int type, IntPtr pixels);

		[DllImport(DLLName, EntryPoint = "glGetTexLevelParameterfv")]
		public static extern void GetTexLevelParameterfv(int target, int level, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glGetTexLevelParameteriv")]
		public static extern void GetTexLevelParameteriv(int target, int level, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glGetTexParameterfv")]
		public static extern void GetTexParameterfv(int target, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glGetTexParameteriv")]
		public static extern void GetTexParameteriv(int target, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glHint")]
		public static extern void Hint(int target, int mode);

		[DllImport(DLLName, EntryPoint = "glIndexMask")]
		public static extern void IndexMask(uint mask);

		[DllImport(DLLName, EntryPoint = "glIndexPointer")]
		public static extern void IndexPointer(int type, int stride, int[] pointer);

		[DllImport(DLLName, EntryPoint = "glIndexd")]
		public static extern void Indexd(double c);

		[DllImport(DLLName, EntryPoint = "glIndexdv")]
		public static extern void Indexdv(double[] c);

		[DllImport(DLLName, EntryPoint = "glIndexf")]
		public static extern void Indexf(float c);

		[DllImport(DLLName, EntryPoint = "glIndexfv")]
		public static extern void Indexfv(float[] c);

		[DllImport(DLLName, EntryPoint = "glIndexi")]
		public static extern void Indexi(int c);

		[DllImport(DLLName, EntryPoint = "glIndexiv")]
		public static extern void Indexiv(int[] c);

		[DllImport(DLLName, EntryPoint = "glIndexs")]
		public static extern void Indexs(short c);

		[DllImport(DLLName, EntryPoint = "glIndexsv")]
		public static extern void Indexsv(short[] c);

		[DllImport(DLLName, EntryPoint = "glIndexub")]
		public static extern void Indexub(byte c);

		[DllImport(DLLName, EntryPoint = "glIndexubv")]
		public static extern void Indexubv(byte[] c);

		[DllImport(DLLName, EntryPoint = "glInitNames")]
		public static extern void InitNames();

		[DllImport(DLLName, EntryPoint = "glInterleavedArrays")]
		public static extern void interleavedArrays(int format, int stride, IntPtr pointer);

		[DllImport(DLLName, EntryPoint = "glInterleavedArrays")]
		public static extern void interleavedArrays(int format, int stride, int offset);

		[DllImport(DLLName, EntryPoint = "glIsEnabled")]
		public static extern bool IsEnabled(int cap);

		[DllImport(DLLName, EntryPoint = "glIsList")]
		public static extern bool IsList(uint list);

		[DllImport(DLLName, EntryPoint = "glIsTexture")]
		public static extern bool IsTexture(uint texture);

		[DllImport(DLLName, EntryPoint = "glLightModelf")]
		public static extern void LightModelf(int pname, float param);

		[DllImport(DLLName, EntryPoint = "glLightModelfv")]
		public static extern void LightModelfv(int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glLightModeli")]
		public static extern void LightModeli(int pname, int param);

		[DllImport(DLLName, EntryPoint = "glLightModeliv")]
		public static extern void LightModeliv(int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glLightf")]
		public static extern void Lightf(int light, int pname, float param);

		[DllImport(DLLName, EntryPoint = "glLightfv")]
		public static extern void Lightfv(int light, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glLighti")]
		public static extern void Lighti(int light, int pname, int param);

		[DllImport(DLLName, EntryPoint = "glLightiv")]
		public static extern void Lightiv(int light, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glLineStipple")]
		public static extern void LineStipple(int factor, ushort pattern);

		[DllImport(DLLName, EntryPoint = "glLineWidth")]
		public static extern void LineWidth(float width);

		[DllImport(DLLName, EntryPoint = "glListBase")]
		public static extern void ListBase(int basevalue);

		[DllImport(DLLName, EntryPoint = "glLoadIdentity")]
		public static extern void LoadIdentity();

		[DllImport(DLLName, EntryPoint = "glLoadMatrixd")]
		public static extern void LoadMatrixd(double[] m);

		[DllImport(DLLName, EntryPoint = "glLoadMatrixf")]
		public static extern void LoadMatrixf(float[] m);

		[DllImport(DLLName, EntryPoint = "glLoadName")]
		public static extern void LoadName(uint name);

		[DllImport(DLLName, EntryPoint = "glLogicOp")]
		public static extern void LogicOp(int opcode);

		[DllImport(DLLName, EntryPoint = "glMap1d")]
		public static extern void Map1d(int target, double u1, double u2, int stride, int order, double[] points);

		[DllImport(DLLName, EntryPoint = "glMap1f")]
		public static extern void Map1f(int target, float u1, float u2, int stride, int order, float[] points);

		[DllImport(DLLName, EntryPoint = "glMap2d")]
		public static extern void Map2d(int target, double u1, double u2, int ustride, int uorder, double v1, double v2, int vstride, int vorder, double[] points);

		[DllImport(DLLName, EntryPoint = "glMap2f")]
		public static extern void Map2f(int target, float u1, float u2, int ustride, int uorder, float v1, float v2, int vstride, int vorder, float[] points);

		[DllImport(DLLName, EntryPoint = "glMapGrid1d")]
		public static extern void MapGrid1d(int un, double u1, double u2);

		[DllImport(DLLName, EntryPoint = "glMapGrid1f")]
		public static extern void MapGrid1f(int un, float u1, float u2);

		[DllImport(DLLName, EntryPoint = "glMapGrid2d")]
		public static extern void MapGrid2d(int un, double u1, double u2, int vn, double v1, double v2);

		[DllImport(DLLName, EntryPoint = "glMapGrid2f")]
		public static extern void MapGrid2f(int un, float u1, float u2, int vn, float v1, float v2);

		[DllImport(DLLName, EntryPoint = "glMaterialf")]
		public static extern void Materialf(int face, int pname, float param);

		[DllImport(DLLName, EntryPoint = "glMaterialfv")]
		public static extern void Materialfv(int face, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glMateriali")]
		public static extern void Materiali(int face, int pname, int param);

		[DllImport(DLLName, EntryPoint = "glMaterialiv")]
		public static extern void Materialiv(int face, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glMatrixMode")]
		public static extern void MatrixMode(int mode);

		[DllImport(DLLName, EntryPoint = "glMultMatrixd")]
		public static extern void MultMatrixd(double[] m);

		[DllImport(DLLName, EntryPoint = "glMultMatrixf")]
		public static extern void MultMatrixf(float[] m);

		[DllImport(DLLName, EntryPoint = "glNewList")]
		public static extern void NewList(uint list, int mode);

		[DllImport(DLLName, EntryPoint = "glNormal3b")]
		public static extern void Normal3b(sbyte nx, sbyte ny, sbyte nz);

		[DllImport(DLLName, EntryPoint = "glNormal3bv")]
		public static extern void Normal3bv(sbyte[] v);

		[DllImport(DLLName, EntryPoint = "glNormal3d")]
		public static extern void Normal3d(double nx, double ny, double nz);

		[DllImport(DLLName, EntryPoint = "glNormal3dv")]
		public static extern void Normal3dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glNormal3f")]
		public static extern void Normal3f(float nx, float ny, float nz);

		[DllImport(DLLName, EntryPoint = "glNormal3fv")]
		public static extern void Normal3fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glNormal3i")]
		public static extern void Normal3i(int nx, int ny, int nz);

		[DllImport(DLLName, EntryPoint = "glNormal3iv")]
		public static extern void Normal3iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glNormal3s")]
		public static extern void Normal3s(short nx, short ny, short nz);

		[DllImport(DLLName, EntryPoint = "glNormal3sv")]
		public static extern void Normal3sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glNormalPointer")]
		public static extern void NormalPointer(int type, int stride, IntPtr pointer);

		[DllImport(DLLName, EntryPoint = "glNormalPointer")]
		public static extern void NormalPointer(int type, int stride, int offset);

		[DllImport(DLLName, EntryPoint = "glOrtho")]
		public static extern void Ortho(double left, double right, double bottom, double top, double zNear, double zFar);

		[DllImport(DLLName, EntryPoint = "glPassThrough")]
		public static extern void PassThrough(float token);

		[DllImport(DLLName, EntryPoint = "glPixelMapfv")]
		public static extern void PixelMapfv(int map, int mapsize, float[] values);

		[DllImport(DLLName, EntryPoint = "glPixelMapuiv")]
		public static extern void PixelMapuiv(int map, int mapsize, uint[] values);

		[DllImport(DLLName, EntryPoint = "glPixelMapusv")]
		public static extern void PixelMapusv(int map, int mapsize, ushort[] values);

		[DllImport(DLLName, EntryPoint = "glPixelStoref")]
		public static extern void PixelStoref(int pname, float param);

		[DllImport(DLLName, EntryPoint = "glPixelStorei")]
		public static extern void PixelStorei(int pname, int param);

		[DllImport(DLLName, EntryPoint = "glPixelTransferf")]
		public static extern void PixelTransferf(int pname, float param);

		[DllImport(DLLName, EntryPoint = "glPixelTransferi")]
		public static extern void PixelTransferi(int pname, int param);

		[DllImport(DLLName, EntryPoint = "glPixelZoom")]
		public static extern void PixelZoom(float xfactor, float yfactor);

		[DllImport(DLLName, EntryPoint = "glPointSize")]
		public static extern void PointSize(float size);

		[DllImport(DLLName, EntryPoint = "glPolygonMode")]
		public static extern void PolygonMode(int face, int mode);

		[DllImport(DLLName, EntryPoint = "glPolygonOffset")]
		public static extern void PolygonOffset(float factor, float units);

		[DllImport(DLLName, EntryPoint = "glPolygonStipple")]
		public static extern void PolygonStipple(byte[] mask);

		[DllImport(DLLName, EntryPoint = "glPopAttrib")]
		public static extern void PopAttrib();

		[DllImport(DLLName, EntryPoint = "glPopClientAttrib")]
		public static extern void PopClientAttrib();

		[DllImport(DLLName, EntryPoint = "glPopMatrix")]
		public static extern void PopMatrix();

		[DllImport(DLLName, EntryPoint = "glPopName")]
		public static extern void PopName();

		[DllImport(DLLName, EntryPoint = "glPrioritizeTextures")]
		public static extern void PrioritizeTextures(int n, uint[] textures, float[] priorities);

		[DllImport(DLLName, EntryPoint = "glPushAttrib")]
		public static extern void PushAttrib(int mask);

		[DllImport(DLLName, EntryPoint = "glPushClientAttrib")]
		public static extern void PushClientAttrib(int mask);

		[DllImport(DLLName, EntryPoint = "glPushMatrix")]
		public static extern void PushMatrix();

		[DllImport(DLLName, EntryPoint = "glPushName")]
		public static extern void PushName(uint name);

		[DllImport(DLLName, EntryPoint = "glRasterPos2d")]
		public static extern void RasterPos2d(double x, double y);

		[DllImport(DLLName, EntryPoint = "glRasterPos2dv")]
		public static extern void RasterPos2dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos2f")]
		public static extern void RasterPos2f(float x, float y);

		[DllImport(DLLName, EntryPoint = "glRasterPos2fv")]
		public static extern void RasterPos2fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos2i")]
		public static extern void RasterPos2i(int x, int y);

		[DllImport(DLLName, EntryPoint = "glRasterPos2iv")]
		public static extern void RasterPos2iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos2s")]
		public static extern void RasterPos2s(short x, short y);

		[DllImport(DLLName, EntryPoint = "glRasterPos2sv")]
		public static extern void RasterPos2sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos3d")]
		public static extern void RasterPos3d(double x, double y, double z);

		[DllImport(DLLName, EntryPoint = "glRasterPos3dv")]
		public static extern void RasterPos3dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos3f")]
		public static extern void RasterPos3f(float x, float y, float z);

		[DllImport(DLLName, EntryPoint = "glRasterPos3fv")]
		public static extern void RasterPos3fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos3i")]
		public static extern void RasterPos3i(int x, int y, int z);

		[DllImport(DLLName, EntryPoint = "glRasterPos3iv")]
		public static extern void RasterPos3iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos3s")]
		public static extern void RasterPos3s(short x, short y, short z);

		[DllImport(DLLName, EntryPoint = "glRasterPos3sv")]
		public static extern void RasterPos3sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos4d")]
		public static extern void RasterPos4d(double x, double y, double z, double w);

		[DllImport(DLLName, EntryPoint = "glRasterPos4dv")]
		public static extern void RasterPos4dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos4f")]
		public static extern void RasterPos4f(float x, float y, float z, float w);

		[DllImport(DLLName, EntryPoint = "glRasterPos4fv")]
		public static extern void RasterPos4fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos4i")]
		public static extern void RasterPos4i(int x, int y, int z, int w);

		[DllImport(DLLName, EntryPoint = "glRasterPos4iv")]
		public static extern void RasterPos4iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glRasterPos4s")]
		public static extern void RasterPos4s(short x, short y, short z, short w);

		[DllImport(DLLName, EntryPoint = "glRasterPos4sv")]
		public static extern void RasterPos4sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glReadBuffer")]
		public static extern void ReadBuffer(int mode);

		[DllImport(DLLName, EntryPoint = "glReadPixels")]
//		public static extern void ReadPixels(int x, int y, int width, int height, int format, int type, IntPtr pixels);
        public static extern void ReadPixels(int x, int y, int width, int height, int format, int type, byte[] pixels);

		[DllImport(DLLName, EntryPoint = "glRectd")]
		public static extern void Rectd(double x1, double y1, double x2, double y2);

		[DllImport(DLLName, EntryPoint = "glRectdv")]
		public static extern void Rectdv(double[] v1, double[] v2);

		[DllImport(DLLName, EntryPoint = "glRectf")]
		public static extern void Rectf(float x1, float y1, float x2, float y2);

		[DllImport(DLLName, EntryPoint = "glRectfv")]
		public static extern void Rectfv(float[] v1, float[] v2);

		[DllImport(DLLName, EntryPoint = "glRecti")]
		public static extern void Recti(int x1, int y1, int x2, int y2);

		[DllImport(DLLName, EntryPoint = "glRectiv")]
		public static extern void Rectiv(int[] v1, int[] v2);

		[DllImport(DLLName, EntryPoint = "glRects")]
		public static extern void Rects(short x1, short y1, short x2, short y2);

		[DllImport(DLLName, EntryPoint = "glRectsv")]
		public static extern void Rectsv(short[] v1, short[] v2);

		[DllImport(DLLName, EntryPoint = "glRenderMode")]
		public static extern int RenderMode(int mode);

		[DllImport(DLLName, EntryPoint = "glRotated")]
		public static extern void Rotated(double angle, double x, double y, double z);

		[DllImport(DLLName, EntryPoint = "glRotatef")]
		public static extern void Rotatef(float angle, float x, float y, float z);

		[DllImport(DLLName, EntryPoint = "glScaled")]
		public static extern void Scaled(double x, double y, double z);

		[DllImport(DLLName, EntryPoint = "glScalef")]
		public static extern void Scalef(float x, float y, float z);

		[DllImport(DLLName, EntryPoint = "glScissor")]
		public static extern void Scissor(int x, int y, int width, int height);

		[DllImport(DLLName, EntryPoint = "glSelectBuffer")]
		public static extern void SelectBuffer(int size, int[] buffer);

		[DllImport(DLLName, EntryPoint = "glSelectBuffer")]
		public static extern void SelectBuffer(int size, IntPtr buffer);

		[DllImport(DLLName, EntryPoint = "glShadeModel")]
		public static extern void ShadeModel(int mode);

		[DllImport(DLLName, EntryPoint = "glStencilFunc")]
		public static extern void StencilFunc(int func, int refvalue, uint mask);

		[DllImport(DLLName, EntryPoint = "glStencilMask")]
		public static extern void StencilMask(uint mask);

		[DllImport(DLLName, EntryPoint = "glStencilOp")]
		public static extern void StencilOp(int fail, int zfail, int zpass);

		[DllImport(DLLName, EntryPoint = "glTexCoord1d")]
		public static extern void TexCoord1d(double s);

		[DllImport(DLLName, EntryPoint = "glTexCoord1dv")]
		public static extern void TexCoord1dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord1f")]
		public static extern void TexCoord1f(float s);

		[DllImport(DLLName, EntryPoint = "glTexCoord1fv")]
		public static extern void TexCoord1fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord1i")]
		public static extern void TexCoord1i(int s);

		[DllImport(DLLName, EntryPoint = "glTexCoord1iv")]
		public static extern void TexCoord1iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord1s")]
		public static extern void TexCoord1s(short s);

		[DllImport(DLLName, EntryPoint = "glTexCoord1sv")]
		public static extern void TexCoord1sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord2d")]
		public static extern void TexCoord2d(double s, double t);

		[DllImport(DLLName, EntryPoint = "glTexCoord2dv")]
		public static extern void TexCoord2dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord2f")]
		public static extern void TexCoord2f(float s, float t);

		[DllImport(DLLName, EntryPoint = "glTexCoord2fv")]
		public static extern void TexCoord2fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord2i")]
		public static extern void TexCoord2i(int s, int t);

		[DllImport(DLLName, EntryPoint = "glTexCoord2iv")]
		public static extern void TexCoord2iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord2s")]
		public static extern void TexCoord2s(short s, short t);

		[DllImport(DLLName, EntryPoint = "glTexCoord2sv")]
		public static extern void TexCoord2sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord3d")]
		public static extern void TexCoord3d(double s, double t, double r);

		[DllImport(DLLName, EntryPoint = "glTexCoord3dv")]
		public static extern void TexCoord3dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord3f")]
		public static extern void TexCoord3f(float s, float t, float r);

		[DllImport(DLLName, EntryPoint = "glTexCoord3fv")]
		public static extern void TexCoord3fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord3i")]
		public static extern void TexCoord3i(int s, int t, int r);

		[DllImport(DLLName, EntryPoint = "glTexCoord3iv")]
		public static extern void TexCoord3iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord3s")]
		public static extern void TexCoord3s(short s, short t, short r);

		[DllImport(DLLName, EntryPoint = "glTexCoord3sv")]
		public static extern void TexCoord3sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord4d")]
		public static extern void TexCoord4d(double s, double t, double r, double q);

		[DllImport(DLLName, EntryPoint = "glTexCoord4dv")]
		public static extern void TexCoord4dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord4f")]
		public static extern void TexCoord4f(float s, float t, float r, float q);

		[DllImport(DLLName, EntryPoint = "glTexCoord4fv")]
		public static extern void TexCoord4fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord4i")]
		public static extern void TexCoord4i(int s, int t, int r, int q);

		[DllImport(DLLName, EntryPoint = "glTexCoord4iv")]
		public static extern void TexCoord4iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoord4s")]
		public static extern void TexCoord4s(short s, short t, short r, short q);

		[DllImport(DLLName, EntryPoint = "glTexCoord4sv")]
		public static extern void TexCoord4sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glTexCoordPointer")]
		public static extern void TexCoordPointer(int size, int type, int stride, IntPtr pointer);

		[DllImport(DLLName, EntryPoint = "glTexCoordPointer")]
		public static extern void TexCoordPointer(int size, int type, int stride, int offset);

		[DllImport(DLLName, EntryPoint = "glTexCoordPointer")]
		public static extern void TexCoordPointer(int size, int type, int stride, float[] pointer);

		[DllImport(DLLName, EntryPoint = "glTexEnvf")]
		public static extern void TexEnvf(int target, int pname, float param);

		[DllImport(DLLName, EntryPoint = "glTexEnvfv")]
		public static extern void TexEnvfv(int target, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glTexEnvi")]
		public static extern void TexEnvi(int target, int pname, int param);

		[DllImport(DLLName, EntryPoint = "glTexEnviv")]
		public static extern void TexEnviv(int target, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glTexGend")]
		public static extern void TexGend(int coord, int pname, double param);

		[DllImport(DLLName, EntryPoint = "glTexGendv")]
		public static extern void TexGendv(int coord, int pname, double[] dparams);

		[DllImport(DLLName, EntryPoint = "glTexGenf")]
		public static extern void TexGenf(int coord, int pname, float param);

		[DllImport(DLLName, EntryPoint = "glTexGenfv")]
		public static extern void TexGenfv(int coord, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glTexGeni")]
		public static extern void TexGeni(int coord, int pname, int param);

		[DllImport(DLLName, EntryPoint = "glTexGeniv")]
		public static extern void TexGeniv(int coord, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glTexImage1D")]
		public static extern void TexImage1D(int target, int level, int internalformat, int width, int border, int format, int type, IntPtr pixels);

		[DllImport(DLLName, EntryPoint = "glTexImage1D")]
		public static extern void TexImage1D(int target, int level, int internalformat, int width, int border, int format, int type, byte[] pixels);

		[DllImport(DLLName, EntryPoint = "glTexImage2D")]
		public static extern void TexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, IntPtr pixels);

		[DllImport(DLLName, EntryPoint = "glTexImage2D")]
		public static extern void TexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, byte[] pixels);

		[DllImport(DLLName, EntryPoint = "glTexParameterf")]
		public static extern void TexParameterf(int target, int pname, float param);

		[DllImport(DLLName, EntryPoint = "glTexParameterfv")]
		public static extern void TexParameterfv(int target, int pname, float[] fparams);

		[DllImport(DLLName, EntryPoint = "glTexParameteri")]
		public static extern void TexParameteri(int target, int pname, int param);

		[DllImport(DLLName, EntryPoint = "glTexParameteriv")]
		public static extern void TexParameteriv(int target, int pname, int[] iparams);

		[DllImport(DLLName, EntryPoint = "glTexSubImage1D")]
		public static extern void TexSubImage1D(int target, int level, int xoffset, int width, int format, int type, byte[] pixels);

		[DllImport(DLLName, EntryPoint = "glTexSubImage2D")]
		public static extern void TexSubImage2D(int target, int level, int xoffset, int yoffset, int width, int height, int format, int type, byte[] pixels);

		[DllImport(DLLName, EntryPoint = "glTranslated")]
		public static extern void Translated(double x, double y, double z);

		[DllImport(DLLName, EntryPoint = "glTranslatef")]
		public static extern void Translatef(float x, float y, float z);

		[DllImport(DLLName, EntryPoint = "glVertex2d")]
		public static extern void Vertex2d(double x, double y);

		[DllImport(DLLName, EntryPoint = "glVertex2dv")]
		public static extern void Vertex2dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glVertex2f")]
		public static extern void Vertex2f(float x, float y);

		[DllImport(DLLName, EntryPoint = "glVertex2fv")]
		public static extern void Vertex2fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glVertex2i")]
		public static extern void Vertex2i(int x, int y);

		[DllImport(DLLName, EntryPoint = "glVertex2iv")]
		public static extern void Vertex2iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glVertex2s")]
		public static extern void Vertex2s(short x, short y);

		[DllImport(DLLName, EntryPoint = "glVertex2sv")]
		public static extern void Vertex2sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glVertex3d")]
		public static extern void Vertex3d(double x, double y, double z);

		[DllImport(DLLName, EntryPoint = "glVertex3dv")]
		public static extern void Vertex3dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glVertex3f")]
		public static extern void Vertex3f(float x, float y, float z);

		[DllImport(DLLName, EntryPoint = "glVertex3fv")]
		public static extern void Vertex3fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glVertex3i")]
		public static extern void Vertex3i(int x, int y, int z);

		[DllImport(DLLName, EntryPoint = "glVertex3iv")]
		public static extern void Vertex3iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glVertex3s")]
		public static extern void Vertex3s(short x, short y, short z);

		[DllImport(DLLName, EntryPoint = "glVertex3sv")]
		public static extern void Vertex3sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glVertex4d")]
		public static extern void Vertex4d(double x, double y, double z, double w);

		[DllImport(DLLName, EntryPoint = "glVertex4dv")]
		public static extern void Vertex4dv(double[] v);

		[DllImport(DLLName, EntryPoint = "glVertex4f")]
		public static extern void Vertex4f(float x, float y, float z, float w);

		[DllImport(DLLName, EntryPoint = "glVertex4fv")]
		public static extern void Vertex4fv(float[] v);

		[DllImport(DLLName, EntryPoint = "glVertex4i")]
		public static extern void Vertex4i(int x, int y, int z, int w);

		[DllImport(DLLName, EntryPoint = "glVertex4iv")]
		public static extern void Vertex4iv(int[] v);

		[DllImport(DLLName, EntryPoint = "glVertex4s")]
		public static extern void Vertex4s(short x, short y, short z, short w);

		[DllImport(DLLName, EntryPoint = "glVertex4sv")]
		public static extern void Vertex4sv(short[] v);

		[DllImport(DLLName, EntryPoint = "glVertexPointer")]
		public static extern void VertexPointer(int size, int type, int stride, IntPtr pointer);

		[DllImport(DLLName, EntryPoint = "glVertexPointer")]
		public static extern void VertexPointer(int size, int type, int stride, float[] pointer);

		[DllImport(DLLName, EntryPoint = "glVertexPointer")]
		public static extern void VertexPointer(int size, int type, int stride, int offset);

		[DllImport(DLLName, EntryPoint = "glViewport")]
		public static extern void Viewport(int x, int y, int width, int height);

		public static int Handle;

		public static glActiveTexture ActiveTexture;
		public static glClientActiveTexture ClientActiveTexture;
		public static glMultiTexCoord1d MultiTexCoord1d;
		public static glMultiTexCoord1dv MultiTexCoord1dv;
		public static glMultiTexCoord1f MultiTexCoord1f;
		public static glMultiTexCoord1fv MultiTexCoord1fv;
		public static glMultiTexCoord1i MultiTexCoord1i;
		public static glMultiTexCoord1iv MultiTexCoord1iv;
		public static glMultiTexCoord1s MultiTexCoord1s;
		public static glMultiTexCoord1sv MultiTexCoord1sv;
		public static glMultiTexCoord2d MultiTexCoord2d;
		public static glMultiTexCoord2dv MultiTexCoord2dv;
		public static glMultiTexCoord2f MultiTexCoord2f;
		public static glMultiTexCoord2fv MultiTexCoord2fv;
		public static glMultiTexCoord2i MultiTexCoord2i;
		public static glMultiTexCoord2iv MultiTexCoord2iv;
		public static glMultiTexCoord2s MultiTexCoord2s;
		public static glMultiTexCoord2sv MultiTexCoord2sv;
		public static glMultiTexCoord3d MultiTexCoord3d;
		public static glMultiTexCoord3dv MultiTexCoord3dv;
		public static glMultiTexCoord3f MultiTexCoord3f;
		public static glMultiTexCoord3fv MultiTexCoord3fv;
		public static glMultiTexCoord3i MultiTexCoord3i;
		public static glMultiTexCoord3iv MultiTexCoord3iv;
		public static glMultiTexCoord3s MultiTexCoord3s;
		public static glMultiTexCoord3sv MultiTexCoord3sv;
		public static glMultiTexCoord4d MultiTexCoord4d;
		public static glMultiTexCoord4dv MultiTexCoord4dv;
		public static glMultiTexCoord4f MultiTexCoord4f;
		public static glMultiTexCoord4fv MultiTexCoord4fv;
		public static glMultiTexCoord4i MultiTexCoord4i;
		public static glMultiTexCoord4iv MultiTexCoord4iv;
		public static glMultiTexCoord4s MultiTexCoord4s;
		public static glMultiTexCoord4sv MultiTexCoord4sv;

		public static glBindBufferARB BindBufferARB; 
		public static glDeleteBuffersARB DeleteBuffersARB_;
		public static glGenBuffersARB GenBuffersARB_;
		public static glIsBufferARB IsBufferARB;
		private static glBufferData BufferData_;
		private static glBufferSubData BufferSubData_;
		public static glGetBufferSubDataARB GetBufferSubDataARB;
		public static glMapBufferARB MapBufferARB;
		public static glUnmapBufferARB UnmapBufferARB;
		public static glGetBufferParameterivARB GetBufferParameterivARB;
		public static glGetBufferPointervARB GetBufferPointervARB;

		public static glGenQueriesARB GenQueriesARB_;
		public static glDeleteQueriesARB DeleteQueriesARB_;
		public static glIsQueryARB IsQueryARB;
		public static glBeginQueryARB BeginQueryARB;
		public static glEndQueryARB EndQueryARB;
		public static glGetQueryivARB GetQueryivARB;
		public static glGetQueryObjectivARB GetQueryObjectivARB;
		public static glGetQueryObjectuivARB GetQueryObjectuivARB;

		public static glVertexAttribPointerARB VertexAttribPointerARB;
		public static glEnableVertexAttribArrayARB EnableVertexAttribArrayARB;
		public static glDisableVertexAttribArrayARB DisableVertexAttribArrayARB;
		private static glProgramStringARB ProgramStringARB_;
		public static glBindProgramARB BindProgramARB;
		public static glDeleteProgramsARB DeleteProgramsARB_;
		public static glGenProgramsARB GenProgramsARB_;
		public static glProgramLocalParameter4fARB ProgramLocalParameter4fARB;
		public static glProgramEnvParameter4fARB ProgramEnvParameter4fARB;
		public static glGetProgramivARB _GetProgramivARB;
		public static glGetProgramEnvParameterfvARB GetProgramEnvParameterfvARB;
		public static glGetProgramLocalParameterfvARB GetProgramLocalParameterfvARB;
		public static glVertexAttrib2fARB VertexAttrib2fARB;
		public static glVertexAttrib3fARB VertexAttrib3fARB;
		public static glVertexAttrib4fARB VertexAttrib4fARB;

		public static glStencilOpSeparateATI StencilOpSeparateATI;
		public static glStencilFuncSeparateATI StencilFuncSeparateATI;

		public static glGetCompressedTexImageARB GetCompressedTexImageARB;
		public static glCompressedTexImage2DARB CompressedTexImage2DARB;

		public static glTexImage3D _TexImage3D;
		public static glBlendEquation BlendEquation;
		public static glWindowPos2f WindowPos2f;

		public static glCreateShaderObjectARB CreateShaderObjectARB;
		private static glShaderSourceARB ShaderSourceARB_;
		public static glCompileShaderARB CompileShaderARB;
		public static glDeleteObjectARB DeleteObjectARB;
		public static glGetHandleARB GetHandleARB;
		public static glDetachObjectARB DetachObjectARB;
		public static glCreateProgramObjectARB CreateProgramObjectARB;
		public static glAttachObjectARB AttachObjectARB;
		public static glLinkProgramARB LinkProgramARB;
		public static glUseProgramObjectARB UseProgramObjectARB;
		public static glValidateProgramARB ValidateProgramARB;
		public static glGetObjectParameterfvARB GetObjectParameterfvARB;
		public static glGetObjectParameterivARB GetObjectParameterivARB;
		public static glGetActiveAttribARB GetActiveAttribARB;
		public static glGetActiveUniformARB GetActiveUniformARB;
		public static glGetAttachedObjectsARB GetAttachedObjectsARB;
		public static glGetAttribLocationARB GetAttribLocationARB;
		public static glGetShaderSourceARB GetShaderSourceARB;
		public static glGetUniformfvARB GetUniformfvARB;  
		public static glGetUniformivARB GetUniformivARB;
		public static glGetUniformLocationARB GetUniformLocationARB;  
		private static glGetInfoLogARB GetInfoLogARB_;
		public static glBindAttribLocationARB BindAttribLocationARB;
		public static glUniform1fARB Uniform1fARB;
		public static glUniform2fARB Uniform2fARB;
		public static glUniform3fARB Uniform3fARB;
		public static glUniform4fARB Uniform4fARB;
		public static glUniform1iARB Uniform1iARB;
		public static glUniform2iARB Uniform2iARB;
		public static glUniform3iARB Uniform3iARB;
		public static glUniform4iARB Uniform4iARB;
		public static glUniform1fvARB Uniform1fvARB;
		public static glUniform2fvARB Uniform2fvARB;
		public static glUniform3fvARB Uniform3fvARB;
		public static glUniform4fvARB Uniform4fvARB;
		public static glUniform1ivARB Uniform1ivARB;
		public static glUniform2ivARB Uniform2ivARB;
		public static glUniform3ivARB Uniform3ivARB;
		public static glUniform4ivARB Uniform4ivARB;
		public static glUniformMatrix2fvARB UniformMatrix2fvARB;
		public static glUniformMatrix3fvARB UniformMatrix3fvARB;
		public static glUniformMatrix4fvARB UniformMatrix4fvARB;

		public static glIsRenderbufferEXT IsRenderbufferEXT;
		public static glBindRenderbufferEXT BindRenderbufferEXT;
		public static glDeleteRenderbuffersEXT DeleteRenderbuffersEXT;
		private static glGenRenderbuffersEXT _GenRenderbuffersEXT;
		public static glRenderbufferStorageEXT RenderbufferStorageEXT;
		public static glGetRenderbufferParameterivEXT GetRenderbufferParameterivEXT;
		public static glIsFramebufferEXT IsFramebufferEXT;
		public static glBindFramebufferEXT BindFramebufferEXT;
		public static glDeleteFramebuffersEXT DeleteFramebuffersEXT;
		private static glGenFramebuffersEXT _GenFramebuffersEXT;
		public static glCheckFramebufferStatusEXT CheckFramebufferStatusEXT;
		public static glFramebufferTexture1DEXT FramebufferTexture1DEXT;
		public static glFramebufferTexture2DEXT FramebufferTexture2DEXT;
		public static glFramebufferTexture3DEXT FramebufferTexture3DEXT;
		public static glFramebufferRenderbufferEXT FramebufferRenderbufferEXT;
		public static glGetFramebufferAttachmentParameterivEXT GetFramebufferAttachmentParameterivEXT;
		public static glGenerateMipmapEXT GenerateMipmapEXT;


		public static void TexImage2D(int target, int level, int internalformat, int border, Bitmap bmp)
		{
			BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Size.Width, bmp.Size.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
			TexImage2D(target, level, internalformat, bmp.Size.Width, bmp.Size.Height, border, gl.BGRA, gl.UNSIGNED_BYTE, data.Scan0);
			bmp.UnlockBits(data);
		}

		public static void TexImage2D(int target, int level, int internalformat, int border, string filename)
		{
			using (Bitmap bmp = new Bitmap(filename))
			{
				gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA8, 0, bmp);
			}
		}

		public static void TexImage2D(int target, int level, int internalformat, int border, Stream st)
		{
			using (Bitmap bmp = new Bitmap(st))
			{
				gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA8, 0, bmp);
			}
		}

		public static void TexImage3D(int target, int level, int internalformat, int width, int height, int depth, int border, int format, int type, IntPtr pixels)
		{
			_TexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
		}

		public static void TexImage3D(int target, int level, int internalformat, int width, int height, int depth, int border, int format, int type, Array data)
		{
			GCHandle h = GCHandle.Alloc(data, GCHandleType.Pinned);
			try
			{
				_TexImage3D(target, level, internalformat, width, height, depth, border, format, type, h.AddrOfPinnedObject());
			}
			finally
			{
				h.Free();
			}
		}
        
		public static void ColorPointer(int size, int type, int stride, Array data)
		{
			GCHandle h = GCHandle.Alloc(data, GCHandleType.Pinned);
			try
			{
				ColorPointer(size, type, stride, h.AddrOfPinnedObject());
			}
			finally
			{
				h.Free();
			}
		}

		public static void NormalPointer(int type, int stride, Array data)
		{
			GCHandle h = GCHandle.Alloc(data, GCHandleType.Pinned);
			try
			{
				NormalPointer(type, stride, h.AddrOfPinnedObject());
			}
			finally
			{
				h.Free();
			}
		}

		public static void TexCoordPointer(int size, int type, int stride, Array data)
		{
			GCHandle h = GCHandle.Alloc(data, GCHandleType.Pinned);
			try
			{
				TexCoordPointer(size, type, stride, h.AddrOfPinnedObject());
			}
			finally
			{
				h.Free();
			}
		}

		public static void VertexPointer(int size, int type, int stride, Array data)
		{
			GCHandle h = GCHandle.Alloc(data, GCHandleType.Pinned);
			try
			{
				VertexPointer(size, type, stride, h.AddrOfPinnedObject());
			}
			finally
			{
				h.Free();
			}
		}

		public static void BufferData(int target, int size, IntPtr data, int usage)
		{
			BufferData_(target, size, data, usage);
		}

		public static void BufferData(int target, int size, Array data, int usage)
		{
			GCHandle h = GCHandle.Alloc(data,GCHandleType.Pinned);
			try
			{
				BufferData(target, size, h.AddrOfPinnedObject(),usage);
			}
			finally
			{
				h.Free();
			}
		}

		public static void BufferSubData(int target, int offset, int size, IntPtr data)
		{
			BufferSubData_(target, offset, size, data);
		}

		public static void BufferSubData(int target, int offset, int size, Array data)
		{
			GCHandle h = GCHandle.Alloc(data,GCHandleType.Pinned);
			try
			{
				BufferSubData(target, offset,size, h.AddrOfPinnedObject());
			}
			finally
			{
				h.Free();
			}
		}

		public static string GetInfoLog(int obj)
		{
			StringBuilder info = new StringBuilder(1024);
			int length = 1024;
			gl.GetInfoLogARB_(obj, 1024, ref length, info);
			return info.ToString();
		}

		public static void ShaderSource(int shader, string source)
		{
			string[] s = new string[] { source };
			int[] l = new int[1] { source.Length };
			gl.ShaderSourceARB_(shader, 1, s, l);
		}

		public static string GetString(int name)
		{
			IntPtr i = GetString_(name);
			return Marshal.PtrToStringAnsi(i);
		}

        //public static uint GenTexture()
        //{
        //    uint i = 0;

        //    GenTextures(1, ref i);

        //    return i;
        //}

        //public static uint[] GenTextures(int n)
        //{
        //    uint[] res = new uint[n];

        //    GenTextures(n, res);

        //    return res;
        //}

		public static void ProgramStringARB(int target, string source)
		{
			ProgramStringARB_(target, PROGRAM_FORMAT_ASCII_ARB, source.Length, source);
		}

		public static int[] GetIntegerv(int name, int size)
		{
			int[] res = new int[size];
			GetIntegerv(name, res);
			return res;
		}

		public static int GetIntegerv(int name)
		{
			return GetIntegerv(name, 1)[0];
		}

		public static float[] GetFloatv(int name, int size)
		{
			float[] res = new float[size];
			GetFloatv(name, res);
			return res;
		}

		public static double[] GetDoublev(int name, int size)
		{
			double[] res = new double[size];
			GetDoublev(name, res);
			return res;
		}

		public static void Viewport(Rectangle r)
		{
			Viewport(r.Left, r.Top, r.Width, r.Height);
		}

		public static void GenBuffersARB(int n, int[] buffers)
		{
			GenBuffersARB_(n, buffers);
		}

		public static int[] GenBuffersARB(int n)
		{
			int[] result = new int[n];
			GenBuffersARB_(n, result);
			return result;
		}

		public static int GenBufferARB()
		{
			int[] result = new int[1];
			GenBuffersARB_(1, result);
			return result[0];
		}

		public static void DeleteBuffersARB(int n, int[] buffers)
		{
			DeleteBuffersARB_(n, buffers);
		}

		public static void DeleteBuffersARB(int[] buffers)
		{
			DeleteBuffersARB_(buffers.Length,buffers);
		}

		public static void GenQueriesARB(int n, int[] queries)
		{
			GenQueriesARB_(n, queries);
		}

		public static int[] GenQueriesARB(int n)
		{
			int[] result = new int[n];
			GenQueriesARB_(n, result);
			return result;
		}

		public static int GenQueryARB()
		{
			int[] result = new int[1];
			GenQueriesARB_(1, result);
			return result[0];
		}

		public static void DeleteQueriesARB(int n, int[] queries)
		{
			DeleteQueriesARB_(n, queries);
		}

		public static void DeleteQueriesARB(int[] queries)
		{
			DeleteQueriesARB_(queries.Length, queries);
		}

		public static void GenProgramsARB(int n, int[] progs)
		{
			GenProgramsARB_(n, progs);
		}

		public static int[] GenProgramsARB(int n)
		{
			int[] result = new int[n];
			GenProgramsARB_(n, result);
			return result;
		}

		public static int GenProgramARB()
		{
			int[] result = new int[1];
			GenProgramsARB_(1, result);
			return result[0];
		}

		public static int GenProgramARB(int target, string source)
		{
			int r = GenProgramARB();
			BindProgramARB(target, r);
			ProgramStringARB(target, source);
			return r;
		}

		public static void DeleteProgramsARB(int n, int[] progs)
		{
			DeleteProgramsARB_(n, progs);
		}

		public static void DeleteProgramsARB(int[] progs)
		{
			DeleteProgramsARB_(progs.Length,progs);
		}

		public static void DeleteProgramARB(int prog)
		{
			int[] progs = new int[] { prog };
			DeleteProgramsARB_(1, progs);
		}

		public static void GetProgramivARB(int target,int pname,int[] iparams)
		{
			_GetProgramivARB(target, pname, iparams);
		}
        
		public static int GetProgramivARB(int target,int pname)
		{
			int[] result=new int[1];
			GetProgramivARB(target,pname,result);
			return result[0];
		}

		public static int[] GenRenderbuffersEXT(int n)
		{
			int[] result = new int[n];
			_GenRenderbuffersEXT(n, result);
			return result;
		}

		public static int GenRenderbuffersEXT()
		{
			int[] result = new int[1];
			_GenRenderbuffersEXT(1, result);
			return result[0];
		}

		public static void GenRenderbuffersEXT(int n, int[] renderbuffers)
		{
			_GenRenderbuffersEXT(n, renderbuffers);
		}

		public static int[] GenFramebuffersExt(int n)
		{
			int[] result = new int[n];
			_GenFramebuffersEXT(n, result);
			return result;
		}

		public static int GenFramebuffersExt()
		{
			int[] result = new int[1];
			_GenFramebuffersEXT(1, result);
			return result[0];
		}

		public static void GenFramebuffersEXT(int n, int[] framebuffers)
		{
			_GenFramebuffersEXT(n, framebuffers);
		}

		private static Delegate GetAddress(string name,Type t)
		{
			int addr=wgl.GetProcAddress(name);
			if (addr == 0) return null; else return Marshal.GetDelegateForFunctionPointer(new IntPtr(addr), t);
		}

		public static void LoadExtensions()
		{
			ActiveTexture = (glActiveTexture)GetAddress("glActiveTexture", typeof(glActiveTexture));
			ClientActiveTexture = (glClientActiveTexture)GetAddress("glClientActiveTexture", typeof(glClientActiveTexture));
			MultiTexCoord1d = (glMultiTexCoord1d)GetAddress("glMultiTexCoord1d", typeof(glMultiTexCoord1d));
			MultiTexCoord1dv = (glMultiTexCoord1dv)GetAddress("glMultiTexCoord1dv", typeof(glMultiTexCoord1dv));
			MultiTexCoord1f = (glMultiTexCoord1f)GetAddress("glMultiTexCoord1f", typeof(glMultiTexCoord1f));
			MultiTexCoord1fv = (glMultiTexCoord1fv)GetAddress("glMultiTexCoord1fv", typeof(glMultiTexCoord1fv));
			MultiTexCoord1i = (glMultiTexCoord1i)GetAddress("glMultiTexCoord1i", typeof(glMultiTexCoord1i));
			MultiTexCoord1iv = (glMultiTexCoord1iv)GetAddress("glMultiTexCoord1iv", typeof(glMultiTexCoord1iv));
			MultiTexCoord1s = (glMultiTexCoord1s)GetAddress("glMultiTexCoord1s", typeof(glMultiTexCoord1s));
			MultiTexCoord1sv = (glMultiTexCoord1sv)GetAddress("glMultiTexCoord1sv", typeof(glMultiTexCoord1sv));
			MultiTexCoord2d = (glMultiTexCoord2d)GetAddress("glMultiTexCoord2d", typeof(glMultiTexCoord2d));
			MultiTexCoord2dv = (glMultiTexCoord2dv)GetAddress("glMultiTexCoord2dv", typeof(glMultiTexCoord2dv));
			MultiTexCoord2f = (glMultiTexCoord2f)GetAddress("glMultiTexCoord2f", typeof(glMultiTexCoord2f));
			MultiTexCoord2fv = (glMultiTexCoord2fv)GetAddress("glMultiTexCoord2fv", typeof(glMultiTexCoord2fv));
			MultiTexCoord2i = (glMultiTexCoord2i)GetAddress("glMultiTexCoord2i", typeof(glMultiTexCoord2i));
			MultiTexCoord2iv = (glMultiTexCoord2iv)GetAddress("glMultiTexCoord2iv", typeof(glMultiTexCoord2iv));
			MultiTexCoord2s = (glMultiTexCoord2s)GetAddress("glMultiTexCoord2s", typeof(glMultiTexCoord2s));
			MultiTexCoord2sv = (glMultiTexCoord2sv)GetAddress("glMultiTexCoord2sv", typeof(glMultiTexCoord2sv));
			MultiTexCoord3d = (glMultiTexCoord3d)GetAddress("glMultiTexCoord3d", typeof(glMultiTexCoord3d));
			MultiTexCoord3dv = (glMultiTexCoord3dv)GetAddress("glMultiTexCoord3dv", typeof(glMultiTexCoord3dv));
			MultiTexCoord3f = (glMultiTexCoord3f)GetAddress("glMultiTexCoord3f", typeof(glMultiTexCoord3f));
			MultiTexCoord3fv = (glMultiTexCoord3fv)GetAddress("glMultiTexCoord3fv", typeof(glMultiTexCoord3fv));
			MultiTexCoord3i = (glMultiTexCoord3i)GetAddress("glMultiTexCoord3i", typeof(glMultiTexCoord3i));
			MultiTexCoord3iv = (glMultiTexCoord3iv)GetAddress("glMultiTexCoord3iv", typeof(glMultiTexCoord3iv));
			MultiTexCoord3s = (glMultiTexCoord3s)GetAddress("glMultiTexCoord3s", typeof(glMultiTexCoord3s));
			MultiTexCoord3sv = (glMultiTexCoord3sv)GetAddress("glMultiTexCoord3sv", typeof(glMultiTexCoord3sv));
			MultiTexCoord4d = (glMultiTexCoord4d)GetAddress("glMultiTexCoord4d", typeof(glMultiTexCoord4d));
			MultiTexCoord4dv = (glMultiTexCoord4dv)GetAddress("glMultiTexCoord4dv", typeof(glMultiTexCoord4dv));
			MultiTexCoord4f = (glMultiTexCoord4f)GetAddress("glMultiTexCoord4f", typeof(glMultiTexCoord4f));
			MultiTexCoord4fv = (glMultiTexCoord4fv)GetAddress("glMultiTexCoord4fv", typeof(glMultiTexCoord4fv));
			MultiTexCoord4i = (glMultiTexCoord4i)GetAddress("glMultiTexCoord4i", typeof(glMultiTexCoord4i));
			MultiTexCoord4iv = (glMultiTexCoord4iv)GetAddress("glMultiTexCoord4iv", typeof(glMultiTexCoord4iv));
			MultiTexCoord4s = (glMultiTexCoord4s)GetAddress("glMultiTexCoord4s", typeof(glMultiTexCoord4s));
			MultiTexCoord4sv = (glMultiTexCoord4sv)GetAddress("glMultiTexCoord4sv", typeof(glMultiTexCoord4sv));
			BindBufferARB = (glBindBufferARB)GetAddress("glBindBufferARB", typeof(glBindBufferARB));
			DeleteBuffersARB_ = (glDeleteBuffersARB)GetAddress("glDeleteBuffersARB", typeof(glDeleteBuffersARB));
			GenBuffersARB_ = (glGenBuffersARB)GetAddress("glGenBuffersARB", typeof(glGenBuffersARB));
			IsBufferARB = (glIsBufferARB)GetAddress("glIsBufferARB", typeof(glIsBufferARB));
			BufferData_ = (glBufferData)GetAddress("glBufferData", typeof(glBufferData));
			BufferSubData_ = (glBufferSubData)GetAddress("glBufferSubData", typeof(glBufferSubData));
			GetBufferSubDataARB = (glGetBufferSubDataARB)GetAddress("glGetBufferSubDataARB", typeof(glGetBufferSubDataARB));
			MapBufferARB = (glMapBufferARB)GetAddress("glMapBufferARB", typeof(glMapBufferARB));
			UnmapBufferARB = (glUnmapBufferARB)GetAddress("glUnmapBufferARB", typeof(glUnmapBufferARB));
			GetBufferParameterivARB = (glGetBufferParameterivARB)GetAddress("glGetBufferParameterivARB", typeof(glGetBufferParameterivARB));
			GetBufferPointervARB = (glGetBufferPointervARB)GetAddress("glGetBufferPointervARB", typeof(glGetBufferPointervARB));
			GenQueriesARB_ = (glGenQueriesARB)GetAddress("glGenQueriesARB", typeof(glGenQueriesARB));
			DeleteQueriesARB_ = (glDeleteQueriesARB)GetAddress("glDeleteQueriesARB", typeof(glDeleteQueriesARB));
			IsQueryARB = (glIsQueryARB)GetAddress("glIsQueryARB", typeof(glIsQueryARB));
			BeginQueryARB = (glBeginQueryARB)GetAddress("glBeginQueryARB", typeof(glBeginQueryARB));
			EndQueryARB = (glEndQueryARB)GetAddress("glEndQueryARB", typeof(glEndQueryARB));
			GetQueryivARB = (glGetQueryivARB)GetAddress("glGetQueryivARB", typeof(glGetQueryivARB));
			GetQueryObjectivARB = (glGetQueryObjectivARB)GetAddress("glGetQueryObjectivARB", typeof(glGetQueryObjectivARB));
			GetQueryObjectuivARB = (glGetQueryObjectuivARB)GetAddress("glGetQueryObjectuivARB", typeof(glGetQueryObjectuivARB));
			VertexAttribPointerARB = (glVertexAttribPointerARB)GetAddress("glVertexAttribPointerARB", typeof(glVertexAttribPointerARB));
			EnableVertexAttribArrayARB = (glEnableVertexAttribArrayARB)GetAddress("glEnableVertexAttribArrayARB", typeof(glEnableVertexAttribArrayARB));
			DisableVertexAttribArrayARB = (glDisableVertexAttribArrayARB)GetAddress("glDisableVertexAttribArrayARB", typeof(glDisableVertexAttribArrayARB));
			ProgramStringARB_ = (glProgramStringARB)GetAddress("glProgramStringARB", typeof(glProgramStringARB));
			BindProgramARB = (glBindProgramARB)GetAddress("glBindProgramARB", typeof(glBindProgramARB));
			DeleteProgramsARB_ = (glDeleteProgramsARB)GetAddress("glDeleteProgramsARB", typeof(glDeleteProgramsARB));
			GenProgramsARB_ = (glGenProgramsARB)GetAddress("glGenProgramsARB", typeof(glGenProgramsARB));
			ProgramLocalParameter4fARB = (glProgramLocalParameter4fARB)GetAddress("glProgramLocalParameter4fARB", typeof(glProgramLocalParameter4fARB));
			ProgramEnvParameter4fARB = (glProgramEnvParameter4fARB)GetAddress("glProgramEnvParameter4fARB", typeof(glProgramEnvParameter4fARB));
			_GetProgramivARB = (glGetProgramivARB)GetAddress("glGetProgramivARB", typeof(glGetProgramivARB));
			GetProgramEnvParameterfvARB = (glGetProgramEnvParameterfvARB)GetAddress("glGetProgramEnvParameterfvARB", typeof(glGetProgramEnvParameterfvARB));
			GetProgramLocalParameterfvARB = (glGetProgramLocalParameterfvARB)GetAddress("glGetProgramLocalParameterfvARB", typeof(glGetProgramLocalParameterfvARB));
			VertexAttrib2fARB = (glVertexAttrib2fARB)GetAddress("glVertexAttrib2fARB", typeof(glVertexAttrib2fARB));
			VertexAttrib3fARB = (glVertexAttrib3fARB)GetAddress("glVertexAttrib3fARB", typeof(glVertexAttrib3fARB));
			VertexAttrib4fARB = (glVertexAttrib4fARB)GetAddress("glVertexAttrib4fARB", typeof(glVertexAttrib4fARB));
            
            /* uncomment if you have an ATI card
			StencilOpSeparateATI = (glStencilOpSeparateATI)GetAddress("glStencilOpSeparateATI", typeof(glStencilOpSeparateATI));
			StencilFuncSeparateATI = (glStencilFuncSeparateATI)GetAddress("glStencilFuncSeparateATI", typeof(glStencilFuncSeparateATI));
            */
			
            GetCompressedTexImageARB = (glGetCompressedTexImageARB)GetAddress("glGetCompressedTexImageARB", typeof(glGetCompressedTexImageARB));
			CompressedTexImage2DARB = (glCompressedTexImage2DARB)GetAddress("glCompressedTexImage2DARB", typeof(glCompressedTexImage2DARB));
			_TexImage3D = (glTexImage3D)GetAddress("glTexImage3D", typeof(glTexImage3D));
			BlendEquation = (glBlendEquation)GetAddress("glBlendEquation", typeof(glBlendEquation));
			WindowPos2f = (glWindowPos2f)GetAddress("glWindowPos2f", typeof(glWindowPos2f));
			CreateShaderObjectARB = (glCreateShaderObjectARB)GetAddress("glCreateShaderObjectARB", typeof(glCreateShaderObjectARB));
			ShaderSourceARB_ = (glShaderSourceARB)GetAddress("glShaderSourceARB", typeof(glShaderSourceARB));
			CompileShaderARB = (glCompileShaderARB)GetAddress("glCompileShaderARB", typeof(glCompileShaderARB));
			DeleteObjectARB = (glDeleteObjectARB)GetAddress("glDeleteObjectARB", typeof(glDeleteObjectARB));
			GetHandleARB = (glGetHandleARB)GetAddress("glGetHandleARB", typeof(glGetHandleARB));
			DetachObjectARB = (glDetachObjectARB)GetAddress("glDetachObjectARB", typeof(glDetachObjectARB));
			CreateProgramObjectARB = (glCreateProgramObjectARB)GetAddress("glCreateProgramObjectARB", typeof(glCreateProgramObjectARB));
			AttachObjectARB = (glAttachObjectARB)GetAddress("glAttachObjectARB", typeof(glAttachObjectARB));
			LinkProgramARB = (glLinkProgramARB)GetAddress("glLinkProgramARB", typeof(glLinkProgramARB));
			UseProgramObjectARB = (glUseProgramObjectARB)GetAddress("glUseProgramObjectARB", typeof(glUseProgramObjectARB));
			ValidateProgramARB = (glValidateProgramARB)GetAddress("glValidateProgramARB", typeof(glValidateProgramARB));
			GetObjectParameterfvARB = (glGetObjectParameterfvARB)GetAddress("glGetObjectParameterfvARB", typeof(glGetObjectParameterfvARB));
			GetObjectParameterivARB = (glGetObjectParameterivARB)GetAddress("glGetObjectParameterivARB", typeof(glGetObjectParameterivARB));
			GetActiveAttribARB = (glGetActiveAttribARB)GetAddress("glGetActiveAttribARB", typeof(glGetActiveAttribARB));
			GetActiveUniformARB = (glGetActiveUniformARB)GetAddress("glGetActiveUniformARB", typeof(glGetActiveUniformARB));
			GetAttachedObjectsARB = (glGetAttachedObjectsARB)GetAddress("glGetAttachedObjectsARB", typeof(glGetAttachedObjectsARB));
			GetAttribLocationARB = (glGetAttribLocationARB)GetAddress("glGetAttribLocationARB", typeof(glGetAttribLocationARB));
			GetShaderSourceARB = (glGetShaderSourceARB)GetAddress("glGetShaderSourceARB", typeof(glGetShaderSourceARB));
			GetUniformfvARB = (glGetUniformfvARB)GetAddress("glGetUniformfvARB", typeof(glGetUniformfvARB));
			GetUniformivARB = (glGetUniformivARB)GetAddress("glGetUniformivARB", typeof(glGetUniformivARB));
			GetUniformLocationARB = (glGetUniformLocationARB)GetAddress("glGetUniformLocationARB", typeof(glGetUniformLocationARB));
			GetInfoLogARB_ = (glGetInfoLogARB)GetAddress("glGetInfoLogARB", typeof(glGetInfoLogARB));
			BindAttribLocationARB = (glBindAttribLocationARB)GetAddress("glBindAttribLocationARB", typeof(glBindAttribLocationARB));
			Uniform1fARB = (glUniform1fARB)GetAddress("glUniform1fARB", typeof(glUniform1fARB));
			Uniform2fARB = (glUniform2fARB)GetAddress("glUniform2fARB", typeof(glUniform2fARB));
			Uniform3fARB = (glUniform3fARB)GetAddress("glUniform3fARB", typeof(glUniform3fARB));
			Uniform4fARB = (glUniform4fARB)GetAddress("glUniform4fARB", typeof(glUniform4fARB));
			Uniform1iARB = (glUniform1iARB)GetAddress("glUniform1iARB", typeof(glUniform1iARB));
			Uniform2iARB = (glUniform2iARB)GetAddress("glUniform2iARB", typeof(glUniform2iARB));
			Uniform3iARB = (glUniform3iARB)GetAddress("glUniform3iARB", typeof(glUniform3iARB));
			Uniform4iARB = (glUniform4iARB)GetAddress("glUniform4iARB", typeof(glUniform4iARB));
			Uniform1fvARB = (glUniform1fvARB)GetAddress("glUniform1fvARB", typeof(glUniform1fvARB));
			Uniform2fvARB = (glUniform2fvARB)GetAddress("glUniform2fvARB", typeof(glUniform2fvARB));
			Uniform3fvARB = (glUniform3fvARB)GetAddress("glUniform3fvARB", typeof(glUniform3fvARB));
			Uniform4fvARB = (glUniform4fvARB)GetAddress("glUniform4fvARB", typeof(glUniform4fvARB));
			Uniform1ivARB = (glUniform1ivARB)GetAddress("glUniform1ivARB", typeof(glUniform1ivARB));
			Uniform2ivARB = (glUniform2ivARB)GetAddress("glUniform2ivARB", typeof(glUniform2ivARB));
			Uniform3ivARB = (glUniform3ivARB)GetAddress("glUniform3ivARB", typeof(glUniform3ivARB));
			Uniform4ivARB = (glUniform4ivARB)GetAddress("glUniform4ivARB", typeof(glUniform4ivARB));
			UniformMatrix2fvARB = (glUniformMatrix2fvARB)GetAddress("glUniformMatrix2fvARB", typeof(glUniformMatrix2fvARB));
			UniformMatrix3fvARB = (glUniformMatrix3fvARB)GetAddress("glUniformMatrix3fvARB", typeof(glUniformMatrix3fvARB));
			UniformMatrix4fvARB = (glUniformMatrix4fvARB)GetAddress("glUniformMatrix4fvARB", typeof(glUniformMatrix4fvARB));

			IsRenderbufferEXT = (glIsRenderbufferEXT)GetAddress("glIsRenderbufferEXT", typeof(glIsRenderbufferEXT));
			BindRenderbufferEXT = (glBindRenderbufferEXT)GetAddress("glBindRenderbufferEXT", typeof(glBindRenderbufferEXT));
			DeleteRenderbuffersEXT = (glDeleteRenderbuffersEXT)GetAddress("glDeleteRenderbuffersEXT", typeof(glDeleteRenderbuffersEXT));
			_GenRenderbuffersEXT = (glGenRenderbuffersEXT)GetAddress("glGenRenderbuffersEXT", typeof(glGenRenderbuffersEXT));
			RenderbufferStorageEXT = (glRenderbufferStorageEXT)GetAddress("glRenderbufferStorageEXT", typeof(glRenderbufferStorageEXT));
			GetRenderbufferParameterivEXT = (glGetRenderbufferParameterivEXT)GetAddress("glGetRenderbufferParameterivEXT", typeof(glGetRenderbufferParameterivEXT));
			IsFramebufferEXT = (glIsFramebufferEXT)GetAddress("glIsFramebufferEXT", typeof(glIsFramebufferEXT));
			BindFramebufferEXT = (glBindFramebufferEXT)GetAddress("glBindFramebufferEXT", typeof(glBindFramebufferEXT));
			DeleteFramebuffersEXT = (glDeleteFramebuffersEXT)GetAddress("glDeleteFramebuffersEXT", typeof(glDeleteFramebuffersEXT));
			_GenFramebuffersEXT = (glGenFramebuffersEXT)GetAddress("glGenFramebuffersEXT", typeof(glGenFramebuffersEXT));
			CheckFramebufferStatusEXT = (glCheckFramebufferStatusEXT)GetAddress("glCheckFramebufferStatusEXT", typeof(glCheckFramebufferStatusEXT));
			FramebufferTexture1DEXT = (glFramebufferTexture1DEXT)GetAddress("glFramebufferTexture1DEXT", typeof(glFramebufferTexture1DEXT));
			FramebufferTexture2DEXT = (glFramebufferTexture2DEXT)GetAddress("glFramebufferTexture2DEXT", typeof(glFramebufferTexture2DEXT));
			FramebufferTexture3DEXT = (glFramebufferTexture3DEXT)GetAddress("glFramebufferTexture3DEXT", typeof(glFramebufferTexture3DEXT));
			FramebufferRenderbufferEXT = (glFramebufferRenderbufferEXT)GetAddress("glFramebufferRenderbufferEXT", typeof(glFramebufferRenderbufferEXT));
			GetFramebufferAttachmentParameterivEXT = (glGetFramebufferAttachmentParameterivEXT)GetAddress("glGetFramebufferAttachmentParameterivEXT", typeof(glGetFramebufferAttachmentParameterivEXT));
			GenerateMipmapEXT = (glGenerateMipmapEXT)GetAddress("glGenerateMipmapEXT", typeof(glGenerateMipmapEXT));

		}

	}

	public class glu
	{
		private const string GLUDLLName="GLU32.DLL"; 
        
		[DllImport(GLUDLLName, EntryPoint = "gluOrtho2D")]        
		private static extern IntPtr ErrorString_(int errCode);

		[DllImport(GLUDLLName, EntryPoint = "gluGetString")]
		private static extern IntPtr GetString_(int errCode);
                      
		[DllImport(GLUDLLName, EntryPoint = "gluOrtho2D")]
		public static extern void Ortho2D(double left, double right, double bottom,double top);
        
		[DllImport(GLUDLLName, EntryPoint = "gluPerspective")]
		public static extern void Perspective(double fovy, double aspect, double zNear,double zFar);

		[DllImport(GLUDLLName, EntryPoint = "gluPickMatrix")]
		public static extern void PickMatrix(double x, double y, double width,double height, int[] viewport);
        
		[DllImport(GLUDLLName, EntryPoint = "gluLookAt")]
		public static extern void LookAt(double eyex, double eyey, double eyez,double centerx,double centery,double centerz,double upx, double upy, double upz);
        
		[DllImport(GLUDLLName, EntryPoint = "gluProject")]
//		public static extern void Project(double objx, double objy, double objz, double[] modelMatrix, double[] projMatrix, int[] viewport, out double winx, out double winy, out double winz);
        public static extern int Project(double objx, double objy, double objz, double[] modelMatrix, double[] projMatrix, int[] viewport, out double winx, out double winy, out double winz);


		[DllImport(GLUDLLName, EntryPoint = "gluUnProject")]
		public static extern void UnProject(double winx, double winy, double winz, double[] modelMatrix, double[] projMatrix, int[] viewport, out double objx, out double objy, out double objz);

		[DllImport(GLUDLLName, EntryPoint = "ScaleImage")]
		public static extern void ScaleImage(int format, int widthin, int heightin,int typein, IntPtr datain, int widthout, int heightout, int typeout, IntPtr dataout);
        
		[DllImport(GLUDLLName, EntryPoint = "gluBuild1DMipmaps")]
		public static extern int Build1DMipmaps(int target, int components, int width, int format, int type, IntPtr data);

		[DllImport(GLUDLLName, EntryPoint = "gluBuild2DMipmaps")]
		public static extern int Build2DMipmaps(int target, int components, int width, int height, int format, int type, IntPtr data);

		public static string GetString(int name)
		{
			IntPtr i = GetString_(name);
			return Marshal.PtrToStringAnsi(i);
		}
		public static string ErrorString(int name)
		{
			IntPtr i = ErrorString_(name);
			return Marshal.PtrToStringAnsi(i);
		}


        public const int POINT              = 100010;
        public const int LINE               = 100011;
        public const int FILL               = 100012;
        public const int SILHOUETTE         = 100013;

        [DllImport(GLUDLLName, EntryPoint = "gluNewQuadric")]
        public static extern IntPtr NewQuadric();

        [DllImport(GLUDLLName, EntryPoint = "gluSphere")]
        public static extern void Sphere(IntPtr qObj, double radius, int slices, int stacks);

        [DllImport(GLUDLLName, EntryPoint = "gluCylinder")]
        public static extern void Cylinder(IntPtr qObj, double baseRadius, double topRadius, double height, int slices, int stacks);
        
        [DllImport(GLUDLLName, EntryPoint = "gluDisk")]
        public static extern void Disk(IntPtr qObj, double innerRadius, double outerRadius, int slices, int loops);

        [DllImport(GLUDLLName, EntryPoint = "gluQuadricDrawStyle")]
        public static extern void QuadricDrawStyle(IntPtr qObj, uint drawStyle);

        [DllImport(GLUDLLName, EntryPoint = "gluQuadricNormals")]
        public static extern void QuadricNormals(IntPtr qObj, uint drawStyle);

        [DllImport(GLUDLLName, EntryPoint = "gluQuadricTexture")]
        public static extern void QuadricTexture(IntPtr qObj, uint drawStyle);

}

	public class wgl
	{
		public const int WGLEXT_VERSION = 4;
		public const int FRONT_COLOR_BUFFER_BIT_ARB = 0x00000001;
		public const int BACK_COLOR_BUFFER_BIT_ARB = 0x00000002;
		public const int DEPTH_BUFFER_BIT_ARB = 0x00000004;
		public const int STENCIL_BUFFER_BIT_ARB = 0x00000008;
		public const int SAMPLE_BUFFERS_ARB = 0x2041;
		public const int SAMPLES_ARB = 0x2042;
		public const int NUMBER_PIXEL_FORMATS_ARB = 0x2000;
		public const int DRAW_TO_WINDOW_ARB = 0x2001;
		public const int DRAW_TO_BITMAP_ARB = 0x2002;
		public const int ACCELERATION_ARB = 0x2003;
		public const int NEED_PALETTE_ARB = 0x2004;
		public const int NEED_SYSTEM_PALETTE_ARB = 0x2005;
		public const int SWAP_LAYER_BUFFERS_ARB = 0x2006;
		public const int SWAP_METHOD_ARB = 0x2007;
		public const int NUMBER_OVERLAYS_ARB = 0x2008;
		public const int NUMBER_UNDERLAYS_ARB = 0x2009;
		public const int TRANSPARENT_ARB = 0x200A;
		public const int TRANSPARENT_RED_VALUE_ARB = 0x2037;
		public const int TRANSPARENT_GREEN_VALUE_ARB = 0x2038;
		public const int TRANSPARENT_BLUE_VALUE_ARB = 0x2039;
		public const int TRANSPARENT_ALPHA_VALUE_ARB = 0x203A;
		public const int TRANSPARENT_INDEX_VALUE_ARB = 0x203B;
		public const int SHARE_DEPTH_ARB = 0x200C;
		public const int SHARE_STENCIL_ARB = 0x200D;
		public const int SHARE_ACCUM_ARB = 0x200E;
		public const int SUPPORT_GDI_ARB = 0x200F;
		public const int SUPPORT_OPENGL_ARB = 0x2010;
		public const int DOUBLE_BUFFER_ARB = 0x2011;
		public const int STEREO_ARB = 0x2012;
		public const int PIXEL_TYPE_ARB = 0x2013;
		public const int COLOR_BITS_ARB = 0x2014;
		public const int RED_BITS_ARB = 0x2015;
		public const int RED_SHIFT_ARB = 0x2016;
		public const int GREEN_BITS_ARB = 0x2017;
		public const int GREEN_SHIFT_ARB = 0x2018;
		public const int BLUE_BITS_ARB = 0x2019;
		public const int BLUE_SHIFT_ARB = 0x201A;
		public const int ALPHA_BITS_ARB = 0x201B;
		public const int ALPHA_SHIFT_ARB = 0x201C;
		public const int ACCUM_BITS_ARB = 0x201D;
		public const int ACCUM_RED_BITS_ARB = 0x201E;
		public const int ACCUM_GREEN_BITS_ARB = 0x201F;
		public const int ACCUM_BLUE_BITS_ARB = 0x2020;
		public const int ACCUM_ALPHA_BITS_ARB = 0x2021;
		public const int DEPTH_BITS_ARB = 0x2022;
		public const int STENCIL_BITS_ARB = 0x2023;
		public const int AUX_BUFFERS_ARB = 0x2024;
		public const int NO_ACCELERATION_ARB = 0x2025;
		public const int GENERIC_ACCELERATION_ARB = 0x2026;
		public const int FULL_ACCELERATION_ARB = 0x2027;
		public const int SWAP_EXCHANGE_ARB = 0x2028;
		public const int SWAP_COPY_ARB = 0x2029;
		public const int SWAP_UNDEFINED_ARB = 0x202A;
		public const int TYPE_RGBA_ARB = 0x202B;
		public const int TYPE_COLORINDEX_ARB = 0x202C;
		public const int ERROR_INVALID_PIXEL_TYPE_ARB = 0x2043;
		public const int ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB = 0x2054;
		public const int DRAW_TO_PBUFFER_ARB = 0x202D;
		public const int MAX_PBUFFER_PIXELS_ARB = 0x202E;
		public const int MAX_PBUFFER_WIDTH_ARB = 0x202F;
		public const int MAX_PBUFFER_HEIGHT_ARB = 0x2030;
		public const int PBUFFER_LARGEST_ARB = 0x2033;
		public const int PBUFFER_WIDTH_ARB = 0x2034;
		public const int PBUFFER_HEIGHT_ARB = 0x2035;
		public const int PBUFFER_LOST_ARB = 0x2036;
		public const int BIND_TO_TEXTURE_RGB_ARB = 0x2070;
		public const int BIND_TO_TEXTURE_RGBA_ARB = 0x2071;
		public const int TEXTURE_FORMAT_ARB = 0x2072;
		public const int TEXTURE_TARGET_ARB = 0x2073;
		public const int MIPMAP_TEXTURE_ARB = 0x2074;
		public const int TEXTURE_RGB_ARB = 0x2075;
		public const int TEXTURE_RGBA_ARB = 0x2076;
		public const int NO_TEXTURE_ARB = 0x2077;
		public const int TEXTURE_CUBE_MAP_ARB = 0x2078;
		public const int TEXTURE_1D_ARB = 0x2079;
		public const int TEXTURE_2D_ARB = 0x207A;
		public const int MIPMAP_LEVEL_ARB = 0x207B;
		public const int CUBE_MAP_FACE_ARB = 0x207C;
		public const int TEXTURE_CUBE_MAP_POSITIVE_X_ARB = 0x207D;
		public const int TEXTURE_CUBE_MAP_NEGATIVE_X_ARB = 0x207E;
		public const int TEXTURE_CUBE_MAP_POSITIVE_Y_ARB = 0x207F;
		public const int TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB = 0x2080;
		public const int TEXTURE_CUBE_MAP_POSITIVE_Z_ARB = 0x2081;
		public const int TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB = 0x2082;
		public const int FRONT_LEFT_ARB = 0x2083;
		public const int FRONT_RIGHT_ARB = 0x2084;
		public const int BACK_LEFT_ARB = 0x2085;
		public const int BACK_RIGHT_ARB = 0x2086;
		public const int AUX0_ARB = 0x2087;
		public const int AUX1_ARB = 0x2088;
		public const int AUX2_ARB = 0x2089;
		public const int AUX3_ARB = 0x208A;
		public const int AUX4_ARB = 0x208B;
		public const int AUX5_ARB = 0x208C;
		public const int AUX6_ARB = 0x208D;
		public const int AUX7_ARB = 0x208E;
		public const int AUX8_ARB = 0x208F;
		public const int AUX9_ARB = 0x2090;
		public const int ERROR_INVALID_PIXEL_TYPE_EXT = 0x2043;
		public const int NUMBER_PIXEL_FORMATS_EXT = 0x2000;
		public const int DRAW_TO_WINDOW_EXT = 0x2001;
		public const int DRAW_TO_BITMAP_EXT = 0x2002;
		public const int ACCELERATION_EXT = 0x2003;
		public const int NEED_PALETTE_EXT = 0x2004;
		public const int NEED_SYSTEM_PALETTE_EXT = 0x2005;
		public const int SWAP_LAYER_BUFFERS_EXT = 0x2006;
		public const int SWAP_METHOD_EXT = 0x2007;
		public const int NUMBER_OVERLAYS_EXT = 0x2008;
		public const int NUMBER_UNDERLAYS_EXT = 0x2009;
		public const int TRANSPARENT_EXT = 0x200A;
		public const int TRANSPARENT_VALUE_EXT = 0x200B;
		public const int SHARE_DEPTH_EXT = 0x200C;
		public const int SHARE_STENCIL_EXT = 0x200D;
		public const int SHARE_ACCUM_EXT = 0x200E;
		public const int SUPPORT_GDI_EXT = 0x200F;
		public const int SUPPORT_OPENGL_EXT = 0x2010;
		public const int DOUBLE_BUFFER_EXT = 0x2011;
		public const int STEREO_EXT = 0x2012;
		public const int PIXEL_TYPE_EXT = 0x2013;
		public const int COLOR_BITS_EXT = 0x2014;
		public const int RED_BITS_EXT = 0x2015;
		public const int RED_SHIFT_EXT = 0x2016;
		public const int GREEN_BITS_EXT = 0x2017;
		public const int GREEN_SHIFT_EXT = 0x2018;
		public const int BLUE_BITS_EXT = 0x2019;
		public const int BLUE_SHIFT_EXT = 0x201A;
		public const int ALPHA_BITS_EXT = 0x201B;
		public const int ALPHA_SHIFT_EXT = 0x201C;
		public const int ACCUM_BITS_EXT = 0x201D;
		public const int ACCUM_RED_BITS_EXT = 0x201E;
		public const int ACCUM_GREEN_BITS_EXT = 0x201F;
		public const int ACCUM_BLUE_BITS_EXT = 0x2020;
		public const int ACCUM_ALPHA_BITS_EXT = 0x2021;
		public const int DEPTH_BITS_EXT = 0x2022;
		public const int STENCIL_BITS_EXT = 0x2023;
		public const int AUX_BUFFERS_EXT = 0x2024;
		public const int NO_ACCELERATION_EXT = 0x2025;
		public const int GENERIC_ACCELERATION_EXT = 0x2026;
		public const int FULL_ACCELERATION_EXT = 0x2027;
		public const int SWAP_EXCHANGE_EXT = 0x2028;
		public const int SWAP_COPY_EXT = 0x2029;
		public const int SWAP_UNDEFINED_EXT = 0x202A;
		public const int TYPE_RGBA_EXT = 0x202B;
		public const int TYPE_COLORINDEX_EXT = 0x202C;
		public const int DRAW_TO_PBUFFER_EXT = 0x202D;
		public const int MAX_PBUFFER_PIXELS_EXT = 0x202E;
		public const int MAX_PBUFFER_WIDTH_EXT = 0x202F;
		public const int MAX_PBUFFER_HEIGHT_EXT = 0x2030;
		public const int OPTIMAL_PBUFFER_WIDTH_EXT = 0x2031;
		public const int OPTIMAL_PBUFFER_HEIGHT_EXT = 0x2032;
		public const int PBUFFER_LARGEST_EXT = 0x2033;
		public const int PBUFFER_WIDTH_EXT = 0x2034;
		public const int PBUFFER_HEIGHT_EXT = 0x2035;
		public const int DEPTH_FLOAT_EXT = 0x2040;
		public const int SAMPLE_BUFFERS_3DFX = 0x2060;
		public const int SAMPLES_3DFX = 0x2061;
		public const int SAMPLE_BUFFERS_EXT = 0x2041;
		public const int SAMPLES_EXT = 0x2042;
		public const int DIGITAL_VIDEO_CURSOR_ALPHA_VALUE_I3D = 0x2051;
		public const int DIGITAL_VIDEO_CURSOR_INCLUDED_I3D = 0x2052;
		public const int DIGITAL_VIDEO_GAMMA_CORRECTED_I3D = 0x2053;
		public const int GAMMA_TABLE_SIZE_I3D = 0x204E;
		public const int GAMMA_EXCLUDE_DESKTOP_I3D = 0x204F;
		public const int GENLOCK_SOURCE_MULTIVIEW_I3D = 0x2044;
		public const int GENLOCK_SOURCE_EXTENAL_SYNC_I3D = 0x2045;
		public const int GENLOCK_SOURCE_EXTENAL_FIELD_I3D = 0x2046;
		public const int GENLOCK_SOURCE_EXTENAL_TTL_I3D = 0x2047;
		public const int GENLOCK_SOURCE_DIGITAL_SYNC_I3D = 0x2048;
		public const int GENLOCK_SOURCE_DIGITAL_FIELD_I3D = 0x2049;
		public const int GENLOCK_SOURCE_EDGE_FALLING_I3D = 0x204A;
		public const int GENLOCK_SOURCE_EDGE_RISING_I3D = 0x204B;
		public const int GENLOCK_SOURCE_EDGE_BOTH_I3D = 0x204C;
		public const int IMAGE_BUFFER_MIN_ACCESS_I3D = 0x00000001;
		public const int IMAGE_BUFFER_LOCK_I3D = 0x00000002;
		public const int BIND_TO_TEXTURE_DEPTH_NV = 0x20A3;
		public const int BIND_TO_TEXTURE_RECTANGLE_DEPTH_NV = 0x20A4;
		public const int DEPTH_TEXTURE_FORMAT_NV = 0x20A5;
		public const int TEXTURE_DEPTH_COMPONENT_NV = 0x20A6;
		public const int DEPTH_COMPONENT_NV = 0x20A7;
		public const int BIND_TO_TEXTURE_RECTANGLE_RGB_NV = 0x20A0;
		public const int BIND_TO_TEXTURE_RECTANGLE_RGBA_NV = 0x20A1;
		public const int TEXTURE_RECTANGLE_NV = 0x20A2;
		public const int FLOAT_COMPONENTS_NV = 0x20B0;
		public const int TEXTURE_FLOAT_R_NV = 0x20B5;
		public const int TEXTURE_FLOAT_RG_NV = 0x20B6;
		public const int TEXTURE_FLOAT_RGB_NV = 0x20B7;
		public const int TEXTURE_FLOAT_RGBA_NV = 0x20B8;

		[DllImport(gl.DLLName)]
		public static extern int GetProcAddress(int hwnd);

		[DllImport(gl.DLLName, EntryPoint = "wglCreateContext")]
		public static extern int CreateContext(int dc);

		[DllImport(gl.DLLName, EntryPoint = "wglMakeCurrent")]
		public static extern int MakeCurrent(int dc, int rc);

		[DllImport(gl.DLLName, EntryPoint = "wglDeleteContext")]
		public static extern int DeleteContext(int rc);

		[DllImport(gl.DLLName, EntryPoint = "wglGetCurrentContext")]
		public static extern int GetCurrentContext();

		[DllImport(gl.DLLName, EntryPoint = "wglGetCurrentDC")]
		public static extern int GetCurrentDC();

		[DllImport(gl.DLLName, EntryPoint = "wglShareLists")]
		public static extern int ShareLists(int r1,int r2);

		[DllImport(gl.DLLName, EntryPoint = "wglGetProcAddress")]
		public static extern int GetProcAddress(string funcname);



        [DllImport(gl.DLLName, EntryPoint = "wglUseFontBitmaps")]
        //public static extern int UseFontBitmaps(int dc, int start, int count, int listbase);
        public static extern bool UseFontBitmaps(IntPtr dc, int start, int count, int listbase);

 
	}

	public class Context : IDisposable
	{
		private int hdc;
		private int hrc;
		private int wnd;

		public int DC
		{
			get
			{
				return hdc;
			}
		}

		public int RC
		{
			get
			{
				return hrc;
			}
		}

		public void MakeCurrent()
		{
			wgl.MakeCurrent(hdc, hrc);
		}

		public void SwapBuffers()
		{
			Windows.SwapBuffers(hdc);
		}

		public void Dispose()
		{
			if (hrc != 0) wgl.DeleteContext(hrc);
			if (hdc != 0) Windows.ReleaseDC(wnd, hdc);
			hrc = 0;
			hdc = 0;
		}

		public Context(Control c, byte color, byte depth, byte stencil)
		{
			
            Boolean b = false;

			wnd = (int)c.Handle;

			if (gl.Handle == 0)
			{
				gl.Handle = Windows.LoadLibrary("OpenGL32.DLL");
				b = true;
			}

			hdc = Windows.GetDC(wnd);

			PixelFormatDescriptor pfd = new PixelFormatDescriptor();

			pfd.ColorBits   = color;
            pfd.DepthBits   = depth;
			pfd.StencilBits = stencil;

            int nPixelFormat = Windows.ChoosePixelFormat(hdc, pfd);

            if (nPixelFormat == 0)
            {

                Debug.WriteLine("ChoosePixelFormat failed.");

                return;

            }

            Debug.WriteLine("   ");
            Debug.WriteLine("Best match for ChoosePixelFormat(): " + nPixelFormat);

            PFList(hdc);

            Windows.SetPixelFormat(hdc, nPixelFormat, pfd);

			hrc = wgl.CreateContext(hdc);

			MakeCurrent();

			if (b) 

                gl.LoadExtensions();

		}



        void PFList(int hdc) {

            PixelFormatDescriptor pfd = new PixelFormatDescriptor();

            int count = Windows.DescribePixelFormat(hdc, 1, pfd.Size, pfd);

            Debug.WriteLine("  ");
            Debug.WriteLine("+----+--------+--------+--------+--------+--------+--------+");
            Debug.WriteLine("| ID |  Color | Alpha  | Accum  | Depth  | Stencil| AuxBuff|");
            Debug.WriteLine("+----+--------+--------+--------+--------+--------+--------+");
            for (int i = 1; i <= count; i++)
            {

                Windows.DescribePixelFormat(hdc, i, pfd.Size, pfd);

                Debug.WriteLine(string.Format("| {0,2} |{1,7} |{2,7} |{3,7} |{4,7} |{5,7} |{6,7} |", i, pfd.ColorBits, pfd.AlphaBits, pfd.AccumBits, pfd.DepthBits, pfd.StencilBits, pfd.AuxBuffers));

            }
            Debug.WriteLine("+----+--------+--------+--------+--------+--------+--------+");
            Debug.WriteLine("  ");

        
        }

	    }
           
}

