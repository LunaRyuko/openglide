
#ifndef __TEXTURE_CONVERSION_H__
#define __TEXTURE_CONVERSION_H__

#include "glogl.h"

void Convert565to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels );
void Convert565to5551( DWORD *Buffer1, DWORD *Buffer2, int Pixels );
void Convert4444to4444special( DWORD *Buffer1, DWORD *Buffer2, int Pixels );
void Convert1555to5551( DWORD *Buffer1, DWORD *Buffer2, int Pixels );
void MMXConvert1555to5551( void *Src, void *Dst, int NumberOfPixels );
void MMXConvert565to5551( void *Src, void *Dst, int NumberOfPixels );
void MMXConvert565to8888( void *Src, void *Dst, DWORD NumberOfPixels );
void MMXConvert4444to4444special( void *Src, void *Dst, int NumberOfPixels );
void ConvertA8toAP88( BYTE *Buffer1, WORD *Buffer2, DWORD Pixels );
void Convert8332to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels );
void ConvertP8to8888( BYTE *Buffer1, DWORD *Buffer2, DWORD Pixels, DWORD *palette );
void ConvertAP88to8888( WORD *Buffer1, DWORD *Buffer2, DWORD Pixels, DWORD *palette );
void ConvertYIQto8888( BYTE *in, DWORD *out, DWORD Pixels, GuNccTable *ncc );
void ConvertAYIQto8888( WORD *in, DWORD *out, DWORD Pixels, GuNccTable *ncc );
void SplitAP88( WORD *ap88, BYTE *index, BYTE *alpha, DWORD pixels );

#endif