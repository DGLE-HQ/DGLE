/**
\author		Korotkov Andrey aka DRON
\date		21.11.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#include "PluginCore.h"
#include <locale>
using namespace std;

CPluginCore::CPluginCore(IEngineCore *pEngineCore):
_pEngineCore(pEngineCore)
{
	_pEngineCore->GetInstanceIndex(_uiInstIdx);

	_pEngineCore->AddProcedure(EPT_INIT, &_s_Init, (void*)this);
	_pEngineCore->AddProcedure(EPT_FREE, &_s_Free, (void*)this);
}

CPluginCore::~CPluginCore()
{
	_pEngineCore->RemoveProcedure(EPT_INIT, &_s_Init, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_FREE, &_s_Free, (void*)this);
}

void CPluginCore::_Init()
{
	_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)_pResMan);

	_pResMan->RegisterFileFormat("jpg", EOT_TEXTURE, "Joint Photographic Experts Group images.", &_s_LoadTextureJPG, (void*)this);
	_pResMan->RegisterFileFormat("png", EOT_TEXTURE, "Portable Network Graphics images.", &_s_LoadTexturePNG, (void*)this);
	_pResMan->RegisterFileFormat("dds", EOT_TEXTURE, "Direct Draw Surface images.", &_s_LoadTextureDDS, (void*)this);
}

void CPluginCore::_Free()
{
	_pResMan->UnregisterFileFormat("jpg");
	_pResMan->UnregisterFileFormat("png");
	_pResMan->UnregisterFileFormat("dds");
}

// PNG Loader //

void CPluginCore::_s_PNGError(png_structp ptr, png_const_charp msg)
{
	LogWrite(((CPluginCore*)ptr->io_ptr)->_uiInstIdx, ("PNG fatal error: " + string(msg) + ".").c_str(), LT_ERROR, __FILE__, __LINE__);
}

void PNGAPI CPluginCore::_s_PNGReadCallback(png_structp ptr, png_bytep data, png_size_t length)
{
	uint read;
	
	((IFile*)ptr->io_ptr)->Read((void*)data, length, read);

	if (read != length)
		png_error(ptr, "reading error");
}

bool CPluginCore::_LoadTexturePNG(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eParams)
{
	uint read;
	png_byte buffer[8];
	
	pFile->Read((void*)buffer, 8, read);
	
	if (read != 8)
	{
		LOG("Failed to read PNG file.", LT_ERROR);
		return false;
	}

	if (png_sig_cmp(buffer, 0, 8) != 0)
	{
		LOG("Wrong PNG file.", LT_ERROR);
		return false;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, this, (png_error_ptr)_s_PNGError, NULL);
	
	if (!png_ptr)
	{
		LOG("Internal PNG png_create_read_struct function failure.", LT_ERROR);
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
	{
		LOG("Internal PNG png_create_info_struct function failure.", LT_ERROR);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		LOG("Internal PNG jump failure.", LT_ERROR);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return false;
	}

	png_set_read_fn(png_ptr, pFile, _s_PNGReadCallback);

	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width, height;
	int bit_depth, color_type;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if (bit_depth < 8)
	{
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		else
			png_set_packing(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	uint8 **row_pointers = new png_bytep[height];
	
	if (!row_pointers)
	{
		LOG("Failed to allocate PNG row pointers.", LT_ERROR);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	int pitch = (color_type == PNG_COLOR_TYPE_RGB_ALPHA ? 4 : 3) * width;
	uint8 *data = new uint8[height * pitch];
	uint8 *offset = data;

	for (uint i = 0; i < height; ++i)
	{
		row_pointers[i] = offset;
		offset += pitch;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		LOG("Internal PNG jump failure.", LT_ERROR);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		delete[] row_pointers;
		delete[] data;
		return false;
	}

	png_read_image(png_ptr, row_pointers);

	png_read_end(png_ptr, NULL);	
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	delete[] row_pointers;

	bool result;

	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		result = _pResMan->CreateTexture(prTex, data, width, height, TDF_RGBA8, TCF_DEFAULT, eParams) == S_OK;
	else
		result = _pResMan->CreateTexture(prTex, data, width, height, TDF_RGB8, width % 4 != 0 ? TCF_PIXEL_ALIGNMENT_1 : TCF_DEFAULT, eParams) == S_OK;

	delete[] data;

	return result;
}

// JPG Loader //

struct TJpegErrorMgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setBufferJump;
	CPluginCore *pPluginCore;
};

void CPluginCore::_s_JPGInitSrc(j_decompress_ptr cinfo)
{}

boolean CPluginCore::_s_FillInputBuffer(j_decompress_ptr cinfo)
{
	return 1;
}

void CPluginCore::_s_SkipInputData(j_decompress_ptr cinfo, long count)
{
	jpeg_source_mgr * src = cinfo->src;
	
	if (count > 0)
	{
		src->bytes_in_buffer -= count;
		src->next_input_byte += count;
	}
}

void CPluginCore::_s_TermSource(j_decompress_ptr cinfo)
{}

void CPluginCore::_s_ErrorExit(j_common_ptr cinfo)
{
	(*cinfo->err->output_message) (cinfo);
	TJpegErrorMgr *myerr = (TJpegErrorMgr*) cinfo->err;
	longjmp(myerr->setBufferJump, 1);
}

void CPluginCore::_s_OutputMessage(j_common_ptr cinfo)
{
	char temp[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo, temp);
	LogWrite(((TJpegErrorMgr*)cinfo->err)->pPluginCore->_uiInstIdx, ("JPG error: \"" + string(temp) + "\".").c_str(), LT_ERROR, GetFileName(__FILE__).c_str(), __LINE__);
}

bool CPluginCore::_LoadTextureJPG(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eParams)
{
	uint32 size;
	pFile->GetSize(size);
	
	uint8 *input = new uint8[size];
	
	uint read;
	pFile->Read((void*)input, size, read);

	if (read != size)
	{
		LOG("Failed to read JPG image raw file data.", LT_ERROR);
		return false;
	}

	jpeg_decompress_struct cinfo;
	TJpegErrorMgr jerr;
	
	jerr.pPluginCore = this;

	cinfo.err = jpeg_std_error(&jerr.pub);
	cinfo.err->error_exit = _s_ErrorExit;
	cinfo.err->output_message = _s_OutputMessage;

	if (setjmp(jerr.setBufferJump))
	{
		LOG("Internal JPG jump failure.", LT_ERROR);
		delete[] input;
		return false;
	}

	jpeg_create_decompress(&cinfo);

	jpeg_source_mgr jsrc;

	jsrc.init_source = _s_JPGInitSrc;
	jsrc.fill_input_buffer = _s_FillInputBuffer;
	jsrc.skip_input_data = _s_SkipInputData;
	jsrc.resync_to_restart = jpeg_resync_to_restart;
	jsrc.term_source = _s_TermSource;
	jsrc.bytes_in_buffer = size;
	jsrc.next_input_byte = (JOCTET*)input;

	cinfo.src = &jsrc;

	jpeg_read_header(&cinfo, TRUE);

	cinfo.out_color_space = JCS_RGB;
	cinfo.out_color_components = 3;
	cinfo.do_fancy_upsampling = FALSE;

	jpeg_start_decompress(&cinfo);

	uint16 rowspan = cinfo.image_width * cinfo.out_color_components;
	uint width = cinfo.image_width;
	uint height = cinfo.image_height;

	uint8* output = new uint8[rowspan * height], **row_ptr = new uint8* [height];

	for (uint i = 0; i < height; ++i)
		row_ptr[i] = &output[i * rowspan];

	uint rowsRead = 0;

	while (cinfo.output_scanline < cinfo.output_height )
		rowsRead += jpeg_read_scanlines(&cinfo, &row_ptr[rowsRead], cinfo.output_height - rowsRead);

	delete[] row_ptr;

	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	delete[] input;

	bool result = _pResMan->CreateTexture(prTex, output, width, height, TDF_RGB8, width % 4 != 0 ? TCF_PIXEL_ALIGNMENT_1 : TCF_DEFAULT, eParams) == S_OK;

	delete[] output;

	return result;
}

// DDS Loader //

bool CPluginCore::_LoadTextureDDS(IFile *pFile, ITexture *&prTex, E_TEXTURE_LOAD_FLAGS eParams)
{
	// Structures and defines form Direct3D 9

#define MAGIC_DDS 0x20534444
#define DDPF_FOURCC 0x00000004
#define DDPF_RGB 0x00000040
#define DDPF_RGBA 0x00000041
#define DDS_CUBEMAP 0x00000200
#define DDS_VOLUME 0x00200000
#define ID_DXT1 0x31545844
#define ID_DXT5 0x35545844

	struct D3D_PixelFormat
	{
		int dwSize;
		int dwFlags;
		int dwFourCC;
		int dwRGBBitCount;
		int dwRBitMask, dwGBitMask, dwBBitMask;
		int dwRGBAlphaBitMask;
	};
 
	struct D3D_Caps2
	{
		int dwCaps1;
		int dwCaps2;
		int Reserved[2];
	};

	struct D3D_SurfaceDesc2
	{
		int dwSize;
		int dwFlags;
		int dwHeight;
		int dwWidth;
		int dwPitchOrLinearSize;
		int dwDepth;
		int dwMipMapCount;
		int dwReserved1[11];
		D3D_PixelFormat ddpfPixelFormat;
		D3D_Caps2 ddsCaps;
		int dwReserved2;
	} header;

	// Reading from file

	uint read;

	int magic;
	pFile->Read(&magic, sizeof(int), read);

	if (magic != MAGIC_DDS)
	{
		LOG("Wrong DDS file.", LT_ERROR);
		return false;
	}

	pFile->Read(&header, sizeof(D3D_SurfaceDesc2), read);

	if (read != sizeof(D3D_SurfaceDesc2))
	{
		LOG("Failed to read DDS file header.", LT_ERROR);
		return false;
	}

	uint block_size = 16;

	E_TEXTURE_DATA_FORMAT texture_format;
	bool b_compressed;

	if (header.ddsCaps.dwCaps2 & DDS_CUBEMAP)
	{
		LOG("DDS cubemap storing is not supported.", LT_ERROR);
		return false;
	}

	if (header.ddsCaps.dwCaps2 & DDS_VOLUME && header.dwDepth > 0)
	{
		LOG("DDS volume texture storing is not supported.", LT_ERROR);
		return false;
	}

	if (header.dwMipMapCount == 0)
		header.dwMipMapCount = 1;

	E_TEXTURE_CREATION_FLAGS cr_flags = header.dwMipMapCount != 1 ? TCF_MIPMAPS_PRESENTED : TCF_DEFAULT;

	if (header.ddpfPixelFormat.dwFlags == DDPF_FOURCC)
	{
		const int &four_cc = header.ddpfPixelFormat.dwFourCC;
		
		b_compressed = true;
		
		if(four_cc == ID_DXT1)
		{
			block_size = 8;
			texture_format = TDF_DXT1;
		}
		else
			if(four_cc == ID_DXT5)
				texture_format = TDF_DXT5;
			else
			{			
				char buf[5];
				
				buf[0] = four_cc & 255;
				buf[1] = (four_cc >> 8) & 255;
				buf[2] = (four_cc >> 16) & 255;
				buf[3] = (four_cc >> 24) & 255;
				buf[4] = 0;
				
				LOG("DDS format \"" + string(buf) + "\" is unsupported by loader. Only DXT1 and DXT5 are supported.", LT_ERROR);
				
				return false;
			}
	} 
	else 
		if (header.ddpfPixelFormat.dwFlags == DDPF_RGB || header.ddpfPixelFormat.dwFlags == DDPF_RGBA)
		{
			b_compressed = false;

			if (header.ddpfPixelFormat.dwRGBBitCount == 32)
			{
				if (header.ddpfPixelFormat.dwRBitMask & 0x00FF0000 && header.ddpfPixelFormat.dwRGBAlphaBitMask & 0xFF000000)
					texture_format = TDF_BGRA8;
				else
					if (header.ddpfPixelFormat.dwRBitMask & 0x000000FF && header.ddpfPixelFormat.dwRGBAlphaBitMask & 0xFF000000)
						texture_format = TDF_RGBA8;
					else
					{
						LOG("Unsupported uncompressed DDS pixel format.", LT_ERROR);
						return false;
					}

				block_size = 4;
			}
			else
				if (header.ddpfPixelFormat.dwRGBBitCount == 24)
				{
					texture_format = TDF_BGR8;
					block_size = 3;
					
					if (header.dwWidth % 4 != 0)
						(int &)cr_flags |= TCF_PIXEL_ALIGNMENT_1;
				}
				else
				{
					LOG("Unsupported uncompressed DDS format. Expected 32 or 24 bit count but got " + IntToStr(header.ddpfPixelFormat.dwRGBBitCount) + ".", LT_ERROR);
					return false;
				}
		}
		else
		{
			LOG("Unsupported uncompressed DDS format.", LT_ERROR);
			return false;
		}

	int w = header.dwWidth, h = header.dwHeight;
	uint data_size = 0;

	for (int i = 0; i < header.dwMipMapCount; ++i)
	{
		if (w == 0) w = 1;
		if (h == 0) h = 1;

		if (!b_compressed)
			data_size += w * h * block_size;
		else
			data_size += ((w + 3) / 4) * ((h + 3) / 4) * block_size;

		w /= 2;
		h /= 2;
	}

	uint8 *data = new uint8[data_size];

	pFile->Read(data, data_size, read);

	if (read != data_size)
	{
		LOG("Failed to read DDS image data.", LT_ERROR);
		delete[] data;
		return false;
	}

	bool result = _pResMan->CreateTexture(prTex, data, header.dwWidth, header.dwHeight, texture_format, cr_flags, eParams) == S_OK;

	delete[] data;

	return result;

#undef MAGIC_DDS
#undef DDPF_FOURCC
#undef DDPF_RGB
#undef DDPF_RGBA
#undef DDS_CUBEMAP
#undef DDS_VOLUME
#undef ID_DXT1
#undef ID_DXT5
}

void DGLE_API CPluginCore::_s_Init(void *pParametr)
{
	((CPluginCore *)pParametr)->_Init();
}

void DGLE_API CPluginCore::_s_Free(void *pParametr)
{
	((CPluginCore *)pParametr)->_Free();
}

bool DGLE_API CPluginCore::_s_LoadTexturePNG(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParametr)
{
	ITexture *ptex = NULL;

	bool ret = ((CPluginCore*)pParametr)->_LoadTexturePNG(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);

	if (ret) prObj = (IEngineBaseObject *&)ptex;

	return ret;
}

bool DGLE_API CPluginCore::_s_LoadTextureJPG(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParametr)
{
	ITexture *ptex = NULL;

	bool ret = ((CPluginCore*)pParametr)->_LoadTextureJPG(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);

	if (ret) prObj = (IEngineBaseObject *&)ptex;

	return ret;
}

bool DGLE_API CPluginCore::_s_LoadTextureDDS(IFile *pFile, IEngineBaseObject *&prObj, uint uiLoadFlags, void *pParametr)
{
	ITexture *ptex = NULL;

	bool ret = ((CPluginCore*)pParametr)->_LoadTextureDDS(pFile, ptex, (E_TEXTURE_LOAD_FLAGS)uiLoadFlags);

	if (ret) prObj = (IEngineBaseObject *&)ptex;

	return ret;
}

DGLE_RESULT DGLE_API CPluginCore::GetPluginInfo(TPluginInfo &stInfo)
{
	strcpy(stInfo.cName, PLUGIN_NAME);
	strcpy(stInfo.cVersion, PLUGIN_VERSION);
	strcpy(stInfo.cVendor, PLUGIN_VENDOR);
	strcpy(stInfo.cDescription, PLUGIN_DESCRIPTION);
	stInfo.ui8PluginSDKVersion = _DGLE_PLUGIN_SDK_VER_;
	
	return S_OK;
}

DGLE_RESULT DGLE_API CPluginCore::GetPluginInterfaceName(char* pcName, uint &uiCharsCount)
{
	if (!pcName)
	{
		uiCharsCount = (uint)strlen(PLUGIN_INTERFACE_NAME) + 1;
		return S_OK;	
	}
	
	if (uiCharsCount < (uint)strlen(PLUGIN_INTERFACE_NAME))
	{
		uiCharsCount = (uint)strlen(PLUGIN_INTERFACE_NAME) + 1;
		strcpy(pcName, "");
		return E_INVALIDARG;
	}

	strcpy(pcName, PLUGIN_INTERFACE_NAME);
	
	return S_OK;
}