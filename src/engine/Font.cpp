/**
\author		Korotkov Andrey aka DRON
\date		16.09.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "Font.h"
#include "Render.h"
#include "Render2D.h"

CBitmapFont::CBitmapFont(uint uiInstIdx, ITexture *pTex, const TFontHeader &stHeader, TCharBox *pChars):
CInstancedObj(uiInstIdx), _pTex(pTex), _fScale(1.f), _stHeader(stHeader), _uiBufferSize(16)// never less than 16
{
	_pBuffer = new float[_uiBufferSize];
	memcpy(_astChars, pChars, sizeof(TCharBox)*224);
}

CBitmapFont::~CBitmapFont()
{
	delete[] _pBuffer;
	_pTex->Free();
}

DGLE_RESULT DGLE_API CBitmapFont::GetTexture(ITexture *&prTexture)
{
	prTexture = _pTex;
	return S_OK;
}

DGLE_RESULT DGLE_API CBitmapFont::GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight)
{
	if (strlen(pcTxt) == 0) 
		return S_FALSE;

	float t_width = 0.f; 
	uiHeight = (uint)(_astChars[0].h * _fScale);

	for (uint i = 0; i < strlen(pcTxt); ++i)
		t_width += _astChars[static_cast<uchar>(pcTxt[i]) - 32].w * _fScale;

	uiWidth = (uint)t_width;

	return S_OK;
}

DGLE_RESULT DGLE_API CBitmapFont::SetScale(float fScale)
{
	_fScale = fScale;
	return S_OK;
}

DGLE_RESULT DGLE_API CBitmapFont::GetScale(float &fScale)
{
	fScale = _fScale;
	return S_OK;
}

DGLE_RESULT DGLE_API CBitmapFont::Draw3D(const char *pcTxt)
{
	size_t length = strlen(pcTxt);
	
	if (length == 0)
		return S_FALSE;

	DGLE_RESULT hr;

	uint strwidth, strheight;
	if (FAILED(hr = GetTextDimensions(pcTxt, strwidth, strheight)))
		return hr;

	ICoreTexture *p_tex;
	_pTex->GetCoreTexture(p_tex);
	Core()->pCoreRenderer()->BindTexture(p_tex);

	uint t_w, t_h;
	_pTex->GetDimensions(t_w, t_h);
	
	float x = -(float)strwidth * 0.5f, y = -(float)strheight * 0.5f;

	if (_uiBufferSize < length*12*2)
	{
		_uiBufferSize = (uint)length*12*2;
		delete[] _pBuffer;
		_pBuffer = new float[_uiBufferSize];
	}

	for (size_t i = 0; i < length; ++i)
	{
		const uchar ch = static_cast<const uchar>(pcTxt[i]) - 32;
		const int
			&curb_x = _astChars[ch].x,
			&curb_y = _astChars[ch].y,
			&curb_w = _astChars[ch].w,
			&curb_h = _astChars[ch].h;

		_pBuffer[i*12] = x + curb_w * _fScale; _pBuffer[i*12 + 1] = y;
		_pBuffer[i*12 + 2] = x + curb_w * _fScale; _pBuffer[i*12 + 3] = y + curb_h * _fScale;
		_pBuffer[i*12 + 4] = x; _pBuffer[i*12 + 5] = y;
		
		_pBuffer[i*12 + 6] = x; _pBuffer[i*12 + 7] = y;
		_pBuffer[i*12 + 8] = x; _pBuffer[i*12 + 9] = _pBuffer[i*12 + 3];
		_pBuffer[i*12 + 10] = x + curb_w * _fScale; _pBuffer[i*12 + 11] = _pBuffer[i*12 + 3];

		_pBuffer[length*12 + i*12]	   = (curb_x + curb_w) / (float)t_w;_pBuffer[length*12 + i*12 + 1] = (curb_y + curb_h) / (float)t_h;
		_pBuffer[length*12 + i*12 + 2] = _pBuffer[length*12 + i*12];	_pBuffer[length*12 + i*12 + 3] = curb_y / (float)t_h;
		_pBuffer[length*12 + i*12 + 4] = curb_x / (float)t_w;			_pBuffer[length*12 + i*12 + 5] = _pBuffer[length*12 + i*12 + 1];
		
		_pBuffer[length*12 + i*12 + 6]  = _pBuffer[length*12 + i*12 + 4]; _pBuffer[length*12 + i*12 + 7]  = _pBuffer[length*12 + i*12 + 1];
		_pBuffer[length*12 + i*12 + 8]  = _pBuffer[length*12 + i*12 + 4]; _pBuffer[length*12 + i*12 + 9]  = _pBuffer[length*12 + i*12 + 3];
		_pBuffer[length*12 + i*12 + 10] = _pBuffer[length*12 + i*12 + 2]; _pBuffer[length*12 + i*12 + 11] = _pBuffer[length*12 + i*12 + 3];

		x += curb_w * _fScale;
	}

	Core()->pCoreRenderer()->Draw(TDrawDataDesc((uint8 *)_pBuffer, (uint)length*12*sizeof(float)), CRDM_TRIANGLES, (uint)length*6);

	return S_OK;
}

DGLE_RESULT DGLE_API CBitmapFont::Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor)
{
	return Draw2D((float)iX, (float)iY, pcTxt, stColor, 0.f, false);
}

DGLE_RESULT DGLE_API CBitmapFont::Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor, float fAngle, bool bVerticesColors)
{
	uint length = strlen(pcTxt);

	if (length == 0)
		return S_FALSE;

	bool b_need_update;

	Core()->pRender()->pRender2D()->NeedToUpdateBatchData(b_need_update);

	if (!b_need_update)
	{
		Core()->pRender()->pRender2D()->Draw(_pTex, TDrawDataDesc(), CRDM_TRIANGLES, length*6, TRectF(), (E_EFFECT2D_FLAGS)(EF_BLEND | (bVerticesColors ? EF_DEFAULT : EF_COLOR_MIX)));
		return S_OK;
	}

	uint width, height;
	
	GetTextDimensions(pcTxt, width, height);
	
	float quad[] = {fX, fY, fX + (float)width, fY, fX + (float)width, fY + (float)height, fX, fY + (float)height};
	
	TMatrix4 transform;

	if (fAngle != 0.f)
	{
		TMatrix4 rot = MatrixIdentity();
		
		const float s = sinf(-fAngle * (float)M_PI/180.f), c = cosf(-fAngle * (float)M_PI/180.f);

		rot._2D[0][0] = +c;
		rot._2D[0][1] = -s;
		rot._2D[1][0] = +s;
		rot._2D[1][1] = +c;

		transform = MatrixTranslate(TVector3(-(fX + width / 2.f), -(fY + height / 2.f), 0.f)) * rot * MatrixTranslate(TVector3(fX + width / 2.f, fY + height / 2.f, 0.f));

		float x = quad[0], y = quad[1];
		quad[0]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		quad[1]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

		 x = quad[2]; y = quad[3];
		quad[2]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		quad[3]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

		x = quad[4]; y = quad[5];
		quad[4]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		quad[5]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

		x = quad[6]; y = quad[7];
		quad[6]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
		quad[7]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];
	}

	if (!Core()->pRender()->pRender2D()->BBoxInScreen(quad, fAngle != 0.f))
		return S_OK;

	float xoffset = 0.f;
	uint t_w, t_h;
	
	_pTex->GetDimensions(t_w, t_h);

	TColor4 prev_color, verts_colors[4];
	
	Core()->pRender()->pRender2D()->GetColorMix(prev_color);
	Core()->pRender()->pRender2D()->SetColorMix(stColor);

	if (bVerticesColors)
		Core()->pRender()->pRender2D()->GetVerticesColors(verts_colors[0], verts_colors[1], verts_colors[2], verts_colors[3]);

	uint size = length*12*2;

	if (bVerticesColors)
		size = length*24*2;

	if (_uiBufferSize < size)
	{
		_uiBufferSize = size;
		delete[] _pBuffer;
		_pBuffer = new float[_uiBufferSize];
	}

	for (uint i = 0; i < length; ++i)
	{
		const uchar ch = static_cast<const uchar>(pcTxt[i]) - 32;
		
		const int
			&curb_x = _astChars[ch].x,
			&curb_y = _astChars[ch].y,
			&curb_w = _astChars[ch].w,
			&curb_h = _astChars[ch].h;
		
		const uint idx = i*24;

		_pBuffer[idx] = fX + xoffset; _pBuffer[idx + 1] = fY; _pBuffer[idx + 2] = (float)curb_x/(float)t_w; _pBuffer[idx + 3] = (float)curb_y/(float)t_h;
		_pBuffer[idx + 4] = fX + xoffset + (float)curb_w*_fScale; _pBuffer[idx + 5] = fY; _pBuffer[idx + 6] = (float)(curb_x + curb_w)/(float)t_w; _pBuffer[idx + 7] = (float)curb_y/(float)t_h;
		_pBuffer[idx + 8] = fX + xoffset + (float)curb_w*_fScale; _pBuffer[idx + 9] = fY + (float)curb_h*_fScale; _pBuffer[idx + 10] = (float)(curb_x+curb_w)/(float)t_w; _pBuffer[idx + 11] = (float)(curb_y+curb_h)/(float)t_h;
		_pBuffer[idx + 12] = _pBuffer[idx]; _pBuffer[idx + 13] = _pBuffer[idx + 1]; _pBuffer[idx + 14] = _pBuffer[idx + 2]; _pBuffer[idx + 15] = _pBuffer[idx + 3];
		_pBuffer[idx + 16] = _pBuffer[idx + 8]; _pBuffer[idx + 17] = _pBuffer[idx + 9]; _pBuffer[idx + 18] = _pBuffer[idx + 10]; _pBuffer[idx + 19] = _pBuffer[idx + 11];
		_pBuffer[idx + 20] = fX + xoffset; _pBuffer[idx + 21] = fY + (float)curb_h*_fScale; _pBuffer[idx + 22] = (float)curb_x/(float)t_w; _pBuffer[idx + 23] = (float)(curb_y+curb_h)/(float)t_h;

		if (fAngle != 0.f)
		{
			float x = _pBuffer[idx], y = _pBuffer[idx + 1];
			_pBuffer[idx]		= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[idx + 1]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			 x = _pBuffer[idx + 4]; y = _pBuffer[idx + 5];
			_pBuffer[idx + 4]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[idx + 5]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			x = _pBuffer[idx + 8]; y = _pBuffer[idx + 9];
			_pBuffer[idx + 8]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[idx + 9]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			x = _pBuffer[idx + 20]; y = _pBuffer[idx + 21];
			_pBuffer[idx + 20] = transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[idx + 21] = transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			_pBuffer[idx + 12] = _pBuffer[idx]; _pBuffer[idx + 13] = _pBuffer[idx + 1];
			_pBuffer[idx + 16] = _pBuffer[idx + 8]; _pBuffer[idx + 17] = _pBuffer[idx + 9];
		}

		if (bVerticesColors)
		{
			const uint idx = length*24 + i*24;
			memcpy(&_pBuffer[idx], verts_colors[0], 12*sizeof(float));
			memcpy(&_pBuffer[idx + 12], verts_colors[0], 4*sizeof(float));
			memcpy(&_pBuffer[idx + 16], verts_colors[2], 8*sizeof(float));
		}

		xoffset += (float)curb_w*_fScale;
	}

	TDrawDataDesc desc;

	desc.pData = (uint8 *)_pBuffer;
	desc.uiVertexStride = 4*sizeof(float);
	desc.bVertexCoord2 = true;
	desc.uiTexCoordOffset = 2*sizeof(float);
	desc.uiTexCoordStride = desc.uiVertexStride;

	if (bVerticesColors)
		desc.uiColorOffset = length*24*sizeof(float);

	Core()->pRender()->pRender2D()->Draw(_pTex, desc, CRDM_TRIANGLES, length*6, TRectF(), (E_EFFECT2D_FLAGS)(EF_BLEND | (bVerticesColors ? EF_DEFAULT : EF_COLOR_MIX)));

	Core()->pRender()->pRender2D()->SetColorMix(prev_color);

	return S_OK;
}