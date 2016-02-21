// PGTexture.cpp: implementation of the PGTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "glogl.h"

#include "PGTexture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline void Convert565to5551( WORD *Buffer1, WORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = ((*Buffer1) & 0xFFC0) |
					(((*Buffer1++) & 0x001F) << 1) |
					0x0001;
		Pixels--;
	}
}

inline void Convert565to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = 0xFF000000 |					// A
			((*Buffer1)		& 0x001F) << 19 |		// B
			((*Buffer1)		& 0x07E0) << 5  |		// G
			((*Buffer1++)	& 0xF800) >> 8;			// R
		Pixels--;
	}
}

inline void Convert4444to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = ((*Buffer1)	& 0xF000) << 16 |	// A
					 ((*Buffer1)	& 0x000F) << 20 |	// B
					 ((*Buffer1)	& 0x00F0) <<  8 |	// G
					 ((*Buffer1++)	& 0x0F00) >>  4;	// R
		Pixels--;
	}
}

inline void Convert4444to4444( WORD *Buffer1, WORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = (((*Buffer1) << 4 ) | ((*Buffer1++) >> 12 ));
		Pixels--;
	}
}

inline void Convert1555to5551( WORD *Buffer1, WORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = (((*Buffer1) << 1 ) | ((*Buffer1++) >> 15 ));
		Pixels--;
	}
}

inline void Convert1555to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = 
			(((*Buffer1)		& 0x8000) ? 0xFF000000 : 0) |		// A
			(((*Buffer1)		& 0x001F)		<< 19)		|		// B
			(((*Buffer1)		& 0x03E0)		<<  6)		|		// G
			(((*Buffer1++)		& 0x7C00)		>>  7);				// R
		Pixels--;
	}
}

inline void ConvertA8toAP88( BYTE *Buffer1, WORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = (((*Buffer1) << 8) | (*Buffer1++));
		Pixels--;
	}
}

inline void ConvertA8to8888( BYTE *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = 
			((*Buffer1) << 24)		|		// A
			((*Buffer1) << 16)		|		// B
			((*Buffer1) <<  8)		|		// G
			(*Buffer1++);					// R
		Pixels--;
	}
}

inline void ConvertI8to8888( BYTE *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = 
			0xFF000000				|		// A
			((*Buffer1) << 16)		|		// B
			((*Buffer1) <<  8)		|		// G
			(*Buffer1++);					// R
		Pixels--;
	}
}

inline void ConvertAI88to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	while(Pixels)
	{
		*Buffer2++ = 
			(((*Buffer1)		& 0xFF00)	<< 16)		|		// A
			(((*Buffer1)		& 0x00FF)	<< 16)		|		// B
			(((*Buffer1)		& 0x00FF)	<<  8)		|		// G
			((*Buffer1++)		& 0x00FF);						// R
		Pixels--;
	}
}

inline void Convert332to8888( BYTE *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	DWORD R, G, B, A = 0xFF000000;
	for( DWORD i = Pixels; i > 0; i-- )
	{
		R = (((*Buffer1) >> 5) & 0x07) << 5;
		G = (((*Buffer1) >> 2) & 0x07) << 5;
		B = ((*Buffer1) & 0x03) << 6;
		*Buffer2 = A | (B << 16) | (G << 8) | R;
		Buffer1++;
		Buffer2++;
	}
}

inline void ConvertAI44toAP88( BYTE *Buffer1, WORD *Buffer2, DWORD Pixels )
{
	for( DWORD i = Pixels; i > 0; i-- )
	{
		*Buffer2++ = ((((*Buffer1) & 0xF0) << 8) | (((*Buffer1++) & 0x0F) << 4));
	}
}

inline void ConvertAI44to8888( BYTE *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	DWORD A, I;
	for( DWORD i = Pixels; i > 0; i-- )
	{
		A = (((*Buffer1) >> 4) & 0x0F) << 4;
		I = ((*Buffer1) & 0x0F) << 4;
		*Buffer2 = (A << 24) | (I << 16) | (I << 8) | I;
		Buffer1++;
		Buffer2++;
	}
}

inline void Convert8332to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels )
{
	DWORD R, G, B, A;
	for( DWORD i = Pixels; i > 0; i-- )
	{
		A = (((*Buffer1) >> 8) & 0xFF);
		R = (((*Buffer1) >> 5) & 0x07) << 5;
		G = (((*Buffer1) >> 2) & 0x07) << 5;
		B = ((*Buffer1) & 0x03) << 6;
		*Buffer2 = (A << 24) | (B << 16) | (G << 8) | R;
		Buffer1++;
		Buffer2++;
	}
}

inline void ConvertP8to8888( BYTE *Buffer1, DWORD *Buffer2, DWORD Pixels, DWORD *palette)
{
	while(Pixels--)
	{
		*Buffer2 = (palette[*Buffer1] & 0xFF000000) |
			((palette[*Buffer1] & 0x000000FF) << 16) | 
			(palette[*Buffer1] & 0x0000FF00) | 
			((palette[*Buffer1] & 0x00FF0000) >> 16);
		Buffer1++;
		Buffer2++;
	}
}

inline void ConvertAP88to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels, DWORD *palette)
{
	DWORD R, G, B, A;
	for( DWORD i = Pixels; i > 0; i-- )
	{
		R = (palette[(*Buffer1)&0x00FF] & 0x00FF0000) >> 16;
		G = (palette[(*Buffer1)&0x00FF] & 0x0000FF00) >> 8;
		B = (palette[(*Buffer1)&0x00FF] & 0x000000FF);
		A = ((*Buffer1) & 0xFF00) >> 8;
		*Buffer2 = (A << 24)| (B << 16) | (G << 8) | R;
		Buffer1++;
		Buffer2++;
	}
}

PGTexture::PGTexture()
{
    m_valid = false;
    m_chromakey_mode = GR_CHROMAKEY_DISABLE;
}

PGTexture::~PGTexture()
{

}

void PGTexture::DownloadMipMap(FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info)
{
    int size = TextureMemRequired(evenOdd, info);

    if(startAddress + size <= TEX_MEMORY)
        memcpy(m_memory + startAddress, info->data, size);
}

void PGTexture::Source(FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info)
{
    int size = TextureMemRequired(evenOdd, info);

    m_startAddress = startAddress;
    m_evenOdd = evenOdd;
    m_info = *info;

	switch(info->aspectRatio)
	{
	case GR_ASPECT_8x1:	m_wAspect = D1OVER256;	m_hAspect = D8OVER256;			
                        break;
	case GR_ASPECT_4x1:	m_wAspect = D1OVER256;	m_hAspect = D4OVER256;			
                        break;
	case GR_ASPECT_2x1:	m_wAspect = D1OVER256;	m_hAspect = D2OVER256;			
                        break;
	case GR_ASPECT_1x1:	m_wAspect = D1OVER256;	m_hAspect = D1OVER256;			
                        break;
	case GR_ASPECT_1x2:	m_wAspect = D2OVER256;	m_hAspect = D1OVER256;			
                        break;
	case GR_ASPECT_1x4:	m_wAspect = D4OVER256;	m_hAspect = D1OVER256;			
                        break;
	case GR_ASPECT_1x8:	m_wAspect = D8OVER256;	m_hAspect = D1OVER256;			
                        break;
	}

    m_valid = (startAddress + size <= TEX_MEMORY);
}

void PGTexture::DownloadTable(GrTexTable_t type, void *data)
{
    if(type == GR_TEXTABLE_PALETTE)
        memcpy(m_palette, data, sizeof(m_palette));
}

void PGTexture::MakeReady()
{
    FxU8 *data;
    TexValues texVals;
    int i;

    if(!m_valid)
        return;
    
    data = m_memory + m_startAddress;

    GetTexValues(&texVals);

    for(i = 0; i < 256; i++)
    {
        if(m_chromakey_mode && (m_palette[i] & 0x00ffffff) == m_chromakey_value)
            m_palette[i] &= 0x00ffffff;
        else
            m_palette[i] |= 0xff000000;
    }

    switch(m_info.format)
    {
    case GR_TEXFMT_RGB_565:
        Convert565to8888( (WORD*)data, m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_ARGB_4444:
        Convert4444to8888( (WORD*)data, m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_ARGB_1555:
        Convert1555to8888( (WORD*)data, m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_P_8:
        ConvertP8to8888( data, m_tex_temp, texVals.nPixels, m_palette );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_AP_88:
        ConvertAP88to8888( (WORD*)data, m_tex_temp, texVals.nPixels, m_palette );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_ALPHA_8:
        ConvertA8toAP88( (BYTE*)data, (WORD*)m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 2, texVals.width, texVals.height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_ALPHA_INTENSITY_88:
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 2, texVals.width, texVals.height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data );
        break;
        
    case GR_TEXFMT_INTENSITY_8:
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 1, texVals.width, texVals.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data );
        break;
        
    case GR_TEXFMT_ALPHA_INTENSITY_44:
        ConvertAI44toAP88( (BYTE*)data, (WORD*)m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 2, texVals.width, texVals.height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_8BIT://GR_TEXFMT_RGB_332
        Convert332to8888( (BYTE*)data, m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_16BIT://GR_TEXFMT_ARGB_8332:
        Convert8332to8888( (WORD*)data, m_tex_temp, texVals.nPixels );
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 4, texVals.width, texVals.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
        
    case GR_TEXFMT_YIQ_422:
    case GR_TEXFMT_AYIQ_8422:
    case GR_TEXFMT_RSVD0:
    case GR_TEXFMT_RSVD1:
    case GR_TEXFMT_RSVD2:
        Error("grTexDownloadMipMapLevel - Unsupported format(%d)\n", m_info.format);
        memset(m_tex_temp, 255, texVals.nPixels * 2);
        glTexImage2D( GL_TEXTURE_2D, texVals.lod, 1, texVals.width, texVals.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_tex_temp );
        break;
    }
}

FxU32 PGTexture::TextureMemRequired(FxU32 evenOdd, GrTexInfo *info)
{
    FxU32 total = 0;
    GrLOD_t i;

    for(i = info->largeLod; i <= info->smallLod; i++)
        total += MipMapMemRequired(i, info->aspectRatio, info->format);

    total = (total + 7) & ~7;

    return total;
}


FxU32 PGTexture::MipMapMemRequired(GrLOD_t lod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format)
{
	FxU32 nLength, nBytes;

	switch(lod)
	{
	case GR_LOD_256:	nLength = 256;	break;
	case GR_LOD_128:	nLength = 128;	break;
	case GR_LOD_64:		nLength = 64;	break;
	case GR_LOD_32:		nLength = 32;	break;
	case GR_LOD_16:		nLength = 16;	break;
	case GR_LOD_8:		nLength = 8;	break;
	case GR_LOD_4:		nLength = 4;	break;
	case GR_LOD_2:		nLength = 2;	break;
	case GR_LOD_1:		nLength = 1;	break;
	}

	switch(aspectRatio)
	{
	case GR_ASPECT_1x1:		nBytes = nLength * nLength;				break;
	case GR_ASPECT_1x2:
	case GR_ASPECT_2x1:		nBytes = (nLength >> 1) * nLength;		break;
	case GR_ASPECT_1x4:
	case GR_ASPECT_4x1:		nBytes = (nLength >> 2) * nLength;		break;
	case GR_ASPECT_1x8:
	case GR_ASPECT_8x1:		nBytes = (nLength >> 3) * nLength;		break;
	}

	switch(format)
	{
	case GR_TEXFMT_RGB_565:
	case GR_TEXFMT_ARGB_8332:
	case GR_TEXFMT_AYIQ_8422:
	case GR_TEXFMT_ARGB_1555:
	case GR_TEXFMT_ARGB_4444:
	case GR_TEXFMT_ALPHA_INTENSITY_88:
	case GR_TEXFMT_AP_88:
		nBytes <<= 1;
		break;
	}

	return nBytes;
}

void PGTexture::GetTexValues(TexValues *tval)
{
	static int nLength;

	switch(m_info.largeLod)
	{
	case GR_LOD_256:	nLength = 256;	break;
	case GR_LOD_128:	nLength = 128;	break;
	case GR_LOD_64:		nLength = 64;	break;
	case GR_LOD_32:		nLength = 32;	break;
	case GR_LOD_16:		nLength = 16;	break;
	case GR_LOD_8:		nLength = 8;	break;
	case GR_LOD_4:		nLength = 4;	break;
	case GR_LOD_2:		nLength = 2;	break;
	case GR_LOD_1:		nLength = 1;	break;
	}

	switch(m_info.aspectRatio)
	{
	case GR_ASPECT_8x1:	tval->width = nLength;		tval->height = nLength >> 3;
                        break;
	case GR_ASPECT_4x1:	tval->width = nLength;		tval->height = nLength >> 2;
                        break;
	case GR_ASPECT_2x1:	tval->width = nLength;		tval->height = nLength >> 1;
                        break;
	case GR_ASPECT_1x1:	tval->width = nLength;		tval->height = nLength;
                        break;
	case GR_ASPECT_1x2:	tval->width = nLength >> 1;	tval->height = nLength;
                        break;
	case GR_ASPECT_1x4:	tval->width = nLength >> 2;	tval->height = nLength;
                        break;
	case GR_ASPECT_1x8:	tval->width = nLength >> 3;	tval->height = nLength;
                        break;
	}

	tval->nPixels = tval->width * tval->height;

    tval->lod = 0;
/*
	switch(Info->format)
	{
	case GR_TEXFMT_RGB_332:
	case GR_TEXFMT_YIQ_422:
	case GR_TEXFMT_ALPHA_8:
	case GR_TEXFMT_INTENSITY_8:
	case GR_TEXFMT_ALPHA_INTENSITY_44:
	case GR_TEXFMT_P_8:
		TexPointer->NBytes = TexPointer->NPixels;
		break;
	case GR_TEXFMT_RGB_565:
	case GR_TEXFMT_ARGB_8332:
	case GR_TEXFMT_AYIQ_8422:
	case GR_TEXFMT_ARGB_1555:
	case GR_TEXFMT_ARGB_4444:
	case GR_TEXFMT_ALPHA_INTENSITY_88:
	case GR_TEXFMT_AP_88:
		TexPointer->NBytes = TexPointer->NPixels << 1;
		break;
	}
*/
}

void PGTexture::Clear()
{

}

void PGTexture::GetAspect(float *hAspect, float *wAspect)
{
    *hAspect = m_hAspect;
    *wAspect = m_wAspect;
}

void PGTexture::ChromakeyValue(GrColor_t value)
{
    m_chromakey_value = value;
}

void PGTexture::ChromakeyMode(GrChromakeyMode_t mode)
{
    m_chromakey_mode = mode;
}
