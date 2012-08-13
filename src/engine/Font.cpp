/**
\author		Korotkov Andrey aka DRON
\date		05.05.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Font.h"
#include "ResourceManager.h"
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

HRESULT CALLBACK CBitmapFont::GetTexture(ITexture *&prTexture)
{
	prTexture = _pTex;
	return S_OK;
}

HRESULT CALLBACK CBitmapFont::GetTextDimensions(const char *pcTxt, uint &uiWidth, uint &uiHeight)
{
	if (strlen(pcTxt) == 0) 
		return S_FALSE;

	float t_width = 0.f; 
	uiHeight = (uint)(_astChars[0].iH*_fScale);

	for (uint i = 0; i < strlen(pcTxt); ++i)
		t_width += _astChars[static_cast<uchar>(pcTxt[i])-32].iW * _fScale;

	uiWidth = (uint)t_width;

	return S_OK;
}

HRESULT CALLBACK CBitmapFont::SetScale(const float &fScale)
{
	_fScale = fScale;
	return S_OK;
}

HRESULT CALLBACK CBitmapFont::GetScale(float &fScale)
{
	fScale = _fScale;
	return S_OK;
}

HRESULT CALLBACK CBitmapFont::Draw3D(const char *pcTxt)
{
	size_t length = strlen(pcTxt);
	
	if (length == 0)
		return S_FALSE;

	HRESULT hr;

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

	for (size_t i = 0; i < length; i++)
	{
		const uchar ch = static_cast<const uchar>(pcTxt[i]) - 32;
		const int
			&curb_x = _astChars[ch].iX,
			&curb_y = _astChars[ch].iY,
			&curb_w = _astChars[ch].iW,
			&curb_h = _astChars[ch].iH;

		_pBuffer[i*12] = x + curb_w * _fScale; _pBuffer[i*12 + 1] = y;
		_pBuffer[i*12 + 2] = x + curb_w * _fScale; _pBuffer[i*12 + 3] = y + curb_h * _fScale;
		_pBuffer[i*12 + 4] = x; _pBuffer[i*12 + 5] = y;
		
		_pBuffer[i*12 + 6] = x; _pBuffer[i*12 + 7] = y;
		_pBuffer[i*12 + 8] = x; _pBuffer[i*12 + 9] = _pBuffer[i*12 + 3];
		_pBuffer[i*12 + 10] = x + curb_w * _fScale; _pBuffer[i*12 + 11] = _pBuffer[i*12 + 3];

		_pBuffer[length*12 + i*12]	 = (curb_x + curb_w) / (float)t_w;	_pBuffer[length*12 + i*12 + 1] = (curb_y + curb_h) / (float)t_h;
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

HRESULT CALLBACK CBitmapFont::Draw2DRect(const TRectF &stRect, const char *pcTxt, const TColor4 &stColor)
{
	std::string txt(pcTxt), word, out;

	uint width, height;
	GetTextDimensions(txt.c_str(), width, height);

	float y = stRect.y;

	if (width > stRect.width && txt.find(" ") != std::string::npos)
	{
		while (!txt.empty())
		{
			word.clear();
			std::string::size_type pos = txt.find_first_of(" ");

			if (pos == std::string::npos)
			{
				word = txt;
				txt.clear();
			}
			else
				word = txt.substr(0, pos + 1);

			GetTextDimensions((out + word).c_str(), width, height);

			if (width > stRect.width)
			{
				if (out.empty())
				{
					word.clear();
					break;
				}

				out.erase(out.end() - 1);

				if (y - stRect.y + height > stRect.height)
				{
					word.clear();
					break;
				}

				Draw2D(stRect.x, y, out.c_str(), stColor, 0.f, false);
				y += height;
								
				out.clear();
			}
			else
			{
				txt.erase(0, pos + 1);
				out.append(word);
			}
		}

		if (!word.empty())
			Draw2D(stRect.x, y, word.c_str(), stColor, 0.f, false);
	}
	else
		if (width < stRect.width)
			Draw2D(stRect.x, stRect.y, txt.c_str(), stColor, 0.f, false);
		else
			return S_FALSE;

	return S_OK;
}

HRESULT CALLBACK CBitmapFont::Draw2DSimple(int iX, int iY, const char *pcTxt, const TColor4 &stColor)
{
	return Draw2D((float)iX, (float)iY, pcTxt, stColor, 0.f, false);
}

HRESULT CALLBACK CBitmapFont::Draw2D(float fX, float fY, const char *pcTxt, const TColor4 &stColor, float fAngle, bool bVerticesColors)
{
	if (strlen(pcTxt) == 0)
		return S_FALSE;

	uint width, height;
	GetTextDimensions(pcTxt, width, height);
	float quad[] = {fX, fY, fX + (float)width, fY, fX + (float)width, fY + (float)height, fX, fY + (float)height};
	
	TMatrix transform;

	if (fAngle != 0.f)
	{
		TMatrix rot = MatrixIdentity();
		
		const float s = sinf(-fAngle * (float)M_PI/180.f), c = cosf(-fAngle * (float)M_PI/180.f);

		rot._2D[0][0] = +c;
		rot._2D[0][1] = -s;
		rot._2D[1][0] = +s;
		rot._2D[1][1] = +c;

		transform = MatrixTranslate(TPoint3(-(fX + width / 2.f), -(fY + height / 2.f), 0.f)) * rot * MatrixTranslate(TPoint3(fX + width / 2.f, fY + height / 2.f, 0.f));

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

	Core()->pRender()->pRender2D()->SetColorMixPush(stColor);

	for (uint i = 0; i < strlen(pcTxt); ++i)
	{
		const uchar ch = static_cast<const uchar>(pcTxt[i]) - 32;
		const int
			&curb_x = _astChars[ch].iX,
			&curb_y = _astChars[ch].iY,
			&curb_w = _astChars[ch].iW,
			&curb_h = _astChars[ch].iH;

		_pBuffer[0] = fX + xoffset; _pBuffer[1] = fY; _pBuffer[2] = (float)curb_x/(float)t_w; _pBuffer[3] = (float)curb_y/(float)t_h;
		_pBuffer[4] = fX + xoffset + (float)curb_w*_fScale; _pBuffer[5] = fY; _pBuffer[6] = (float)(curb_x + curb_w)/(float)t_w; _pBuffer[7] = (float)curb_y/(float)t_h;
		_pBuffer[8] = fX + xoffset + (float)curb_w*_fScale; _pBuffer[9] = fY + (float)curb_h*_fScale; _pBuffer[10] = (float)(curb_x+curb_w)/(float)t_w; _pBuffer[11] = (float)(curb_y+curb_h)/(float)t_h;
		_pBuffer[12] = fX + xoffset; _pBuffer[13] = fY + (float)curb_h*_fScale; _pBuffer[14] = (float)curb_x/(float)t_w; _pBuffer[15] = (float)(curb_y+curb_h)/(float)t_h;

		if (fAngle != 0.f)
		{
			float x = _pBuffer[0], y = _pBuffer[1];
			_pBuffer[0]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[1]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			 x = _pBuffer[4]; y = _pBuffer[5];
			_pBuffer[4]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[5]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			x = _pBuffer[8]; y = _pBuffer[9];
			_pBuffer[8]	= transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[9]	= transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];

			x = _pBuffer[12]; y = _pBuffer[13];
			_pBuffer[12] = transform._2D[0][0] * x + transform._2D[1][0] * y + transform._2D[3][0];
			_pBuffer[13] = transform._2D[0][1] * x + transform._2D[1][1] * y + transform._2D[3][1];
		}

		Core()->pRender()->pRender2D()->DrawTexQuadAsIs(_pTex, _pBuffer, (E_EFFECT2D_FLAGS)(EF_BLEND | (bVerticesColors ? EF_VERTICES_COLOR : EF_COLORMIX)));

		xoffset += (float)curb_w*_fScale;
	}

	Core()->pRender()->pRender2D()->ColorMixPop();

	return S_OK;
}

HRESULT CALLBACK CBitmapFont::Free()
{
	bool can_delete;
	
	Core()->pResMan()->RemoveResource(this, can_delete);
	
	if (can_delete)
	{
		delete this;
		return S_OK;
	}
	else
		return S_FALSE;
}

HRESULT CALLBACK CBitmapFont::GetType(E_ENG_OBJ_TYPE &eObjType)
{
	eObjType = EOT_BITMAP_FONT;
	return S_OK;
}

HRESULT CALLBACK CBitmapFont::GetUnknownType(uint &uiObjUnknownType)
{
	uiObjUnknownType = -1;
	return S_FALSE;
}