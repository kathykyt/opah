#ifndef _FREETYPE_H_
#define _FREETYPE_H_

#include "ft2build.h"
#include FT_FREETYPE_H

class FreeType2 {
public:
    FT_Library library;
    FT_Face face;
    FT_Error error;
    FT_UInt glyph_index;
    FT_Bitmap* bitmap;
    FT_GlyphSlot slot;
    int height;
    int baseHeight;
    int spaceWidth;
    unsigned char* imageRGB;
    int colorR;
    int colorG;
    int colorB;

    FreeType2(const char * fontPath, unsigned int fontWidth, unsigned int fontHeight, const char * fontColor);
    ~FreeType2(void);
    unsigned char* getBmpData(unsigned long text);
	unsigned char* getBmpDataEn(unsigned long text);		
};

#endif
