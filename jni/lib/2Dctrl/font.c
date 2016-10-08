#include "font.h"
#include "bitmap.h"
#include "log.h"

static FT_Library library;
FT_Face face;


void font_init(const char* path)
{
	FT_Error error;
    error = FT_Init_FreeType(&library);
    if (error)
        printf("Library Error\n");
	
    error = FT_New_Face(library, path, 0, &face);
}

void font_deinit()
{
	FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void font_set_size(int size)
{
	FT_Error error;

    error = FT_Set_Char_Size(
          face,    /* handle to face object           */
          0,       /* char_width in 1/64th of points  */
          size*64,   /* char_height in 1/64th of points */
          96,     /* horizontal device resolution    */
          96 );   /* vertical device resolution      */

    if (error) {
        INFO_X("set font size error\n");
    }
}

unsigned short utf8_get_char (const char *p, const char** next)
{
  int i, mask = 0, len;
  unsigned short result;
  unsigned char c = (unsigned char) *p;
	
  UTF8_COMPUTE (c, mask, len);

  if (len == -1)
    return (unsigned short)-1;
  UTF8_GET (result, p, i, mask, len);

  if(next != NULL)
  {
    *next = g_utf8_next_char(p);
  }

  return result;
}

FT_GlyphSlot font_get_font(unsigned short text)
{
    FT_UInt glyph_index;
    FT_Error error; 
    FT_GlyphSlot slot;
    int height;
    int baseHeight;
    int spaceWidth;
    FT_Bitmap* bitmap;

    //get the index of input text in font, for now, using ASCII
    glyph_index = FT_Get_Char_Index(face, text); //'A' = 65
                                                 //Loading a glyph image into the slot
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if (error)
        printf("Load_Glyph Error\n");

    //convert to a bitmap
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (error)
        printf("Render_Glyph Error\n");

    slot = face->glyph;
	//printf("bitmap_left=%d bitmap_top=%d", slot->bitmap_left, slot->bitmap_top);
    bitmap = &slot->bitmap;

    return slot; 
}

unsigned char*  _font_render(unsigned short text, Color *color, FT_Bitmap *bitmap, Bitmap* fontBitmap)
{
	int wholeArea = bitmap->width * bitmap->rows;
	//INFO_X("whole pix=%d  width=%d height=%d \n", wholeArea, bitmap->width, bitmap->rows);
	unsigned char* imageRGB;

	//fontBitmap = (Bitmap*)malloc(4 * wholeArea * sizeof(unsigned char)+8);

	
	imageRGB = NULL;
	imageRGB = (unsigned char*)malloc(4 * wholeArea * sizeof(unsigned char)+8);
	memset(imageRGB, 0, 4 * wholeArea);

	int height = bitmap->width;
	int baseHeight = bitmap->width * 0.85;
	int	spaceWidth = 4;

	//put width and height into header
	imageRGB[0] = bitmap->width;
	
	//imageRGB[4] = bitmap->rows;
	imageRGB[4] = bitmap->rows;
	int count = 0;

#if 1
	        for (; count < wholeArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                imageRGB[count * 4 + 8] = 255;
                imageRGB[count * 4 + 1 + 8] = 255;
                imageRGB[count * 4 + 2 + 8] = 255;
                imageRGB[count * 4 + 3 + 8] = 0;
            } else if (bitmap->buffer[count] > 0) {
                //3 * 255 means white color, transparent equals gray
                imageRGB[count * 4 + 8] = color->b;
                imageRGB[count * 4 + 1 + 8] = color->g;
                imageRGB[count * 4 + 2 + 8] = color->r;
                imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count];
            }
        }
#endif
#if 0
    if (text != 'g' && text != 'j' && text != 'p' && text != 'q' && text != 'y' && text != 176 && text != '-' && text != ' ' &&
        !(0x0030 <= text && text <= 0x0039)) {

        int topArea = bitmap->width * ((height - bitmap->rows) / 2);

        int baseArea = bitmap->width * bitmap->rows + topArea;
        //int topArea = bitmap->width * (baseHeight - bitmap->rows);
        //int baseArea = bitmap->width * baseHeight;



        int count;
        //fill the top blank

        for (count = 0; count < topArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < baseArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                imageRGB[count * 4 + 8] = 255;
                imageRGB[count * 4 + 1 + 8] = 255;
                imageRGB[count * 4 + 2 + 8] = 255;
                imageRGB[count * 4 + 3 + 8] = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                imageRGB[count * 4 + 8] = color->b;
                imageRGB[count * 4 + 1 + 8] = color->g;
                imageRGB[count * 4 + 2 + 8] = color->r;
                imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count - topArea];
            }
        }
        //fill buttom blank
        for (; count < wholeArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
    } else if (0x0030 <= text && text <= 0x0039) {
        int wholeArea = bitmap->width * height;
        int topArea = bitmap->width * (baseHeight - bitmap->rows);
        int baseArea = bitmap->width * baseHeight;

        int count;
        //fill the top blank

        for (count = 0; count < topArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < baseArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                imageRGB[count * 4 + 8] = 255;
                imageRGB[count * 4 + 1 + 8] = 255;
                imageRGB[count * 4 + 2 + 8] = 255;
                imageRGB[count * 4 + 3 + 8] = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                imageRGB[count * 4 + 8] = color->b;
                imageRGB[count * 4 + 1 + 8] = color->g;
                imageRGB[count * 4 + 2 + 8] = color->r;
                imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count - topArea];
            }
        }
        //fill buttom blank
        for (; count < wholeArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
    } else if (text == 'g' || text == 'j' || text == 'p' || text == 'q' || text == 'y') {
        int wholeArea = height * bitmap->width;
        int topArea = bitmap->width * (height - bitmap->rows);

        int count;
        //fill the top blank
        for (count = 0; count < topArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < wholeArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                imageRGB[count * 4 + 8] = 255;
                imageRGB[count * 4 + 1 + 8] = 255;
                imageRGB[count * 4 + 2 + 8] = 255;
                imageRGB[count * 4 + 3 + 8] = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                imageRGB[count * 4 + 8] = color->b;
                imageRGB[count * 4 + 1 + 8] = color->g;
                imageRGB[count * 4 + 2 + 8] = color->r;
                imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count - topArea];
            }
        }
    }
//176 is the degree sign
    else if (text == 176) {
        int wholeArea = height * bitmap->width;
        int topArea = bitmap->width * (2 * bitmap->rows);
        int baseArea = bitmap->width * (3 * bitmap->rows);

        //put width and height into header
        imageRGB[0] = bitmap->width;
        imageRGB[4] = height;
        int count;
        //fill the top blank
        for (count = 0; count < topArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < baseArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                imageRGB[count * 4 + 8] = 255;
                imageRGB[count * 4 + 1 + 8] = 255;
                imageRGB[count * 4 + 2 + 8] = 255;
                imageRGB[count * 4 + 3 + 8] = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                imageRGB[count * 4 + 8] = color->b;
                imageRGB[count * 4 + 1 + 8] = color->g;
                imageRGB[count * 4 + 2 + 8] = color->r;
                imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count - topArea];
            }
        }
        //fill buttom blank
        for (; count < wholeArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
    } else if (text = ' ') {
        int wholeArea = height * spaceWidth;

        //put width and height into header
        imageRGB[0] = spaceWidth;
        imageRGB[4] = height;
        int count;
        //fill blank
        for (count = 0; count < wholeArea; count++) {
            imageRGB[count * 4 + 8] = 255;
            imageRGB[count * 4 + 1 + 8] = 255;
            imageRGB[count * 4 + 2 + 8] = 255;
            imageRGB[count * 4 + 3 + 8] = 0;
        }
    }

#endif

	return imageRGB;
}



#if 0

void font_render(unsigned short text, Color *color, FT_Bitmap *bitmap, Bitmap* fontBitmap)
{
    int wholeArea = bitmap->width * bitmap->width;
    int topArea;
    int baseArea;
    int count;
	Color c;

    fontBitmap->data = (Color *)malloc(sizeof(Color) * wholeArea);
    memset(fontBitmap->data, 0, sizeof(Color) * wholeArea);

    fontBitmap->width = bitmap->width;
    fontBitmap->height = bitmap->width;

	int i;
	int baseHeight = bitmap->width * 0.85;


#if 1
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#if 0
//this is the baseline for character and numbers in English

//handle the case under the baseline
//176 represent temperature
    /**one pix include 4 byte, rgba color */
    if (text != 'g' && text != 'j' && text != 'p' && text != 'q' && text != 'y' && text != 176 && text != '-' && text != ' ' &&
        !(0x0030 <= text && text <= 0x0039)) {

        topArea = bitmap->width * ((bitmap->width - bitmap->rows) / 2);
        baseArea = (bitmap->width * bitmap->rows + topArea);

        //fill the top blank
        for (count = 0; count < topArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < baseArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                fontBitmap->data[count].b = 255;
                fontBitmap->data[count].g = 255;
                fontBitmap->data[count].r = 255;
                fontBitmap->data[count].a = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                fontBitmap->data[count].b = color->b; 
                fontBitmap->data[count].g = color->g; 
                fontBitmap->data[count].r = color->r;
                fontBitmap->data[count].a = bitmap->buffer[count - topArea];
            }
        }
        //fill buttom blank
        for (; count < wholeArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
    } else if (0x0030 <= text && text <= 0x0039) {
        topArea = bitmap->width * (baseHeight - bitmap->rows);
        baseArea = bitmap->width * baseHeight;

        //fill the top blank

        for (count = 0; count < topArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < baseArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                fontBitmap->data[count].b = 255;
                fontBitmap->data[count].g = 255;
                fontBitmap->data[count].r = 255;
                fontBitmap->data[count].a = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                fontBitmap->data[count].b = color->b;
                fontBitmap->data[count].g = color->g;
                fontBitmap->data[count].r = color->r; 
                fontBitmap->data[count].a = bitmap->buffer[count - topArea];
            }
        }
        //fill buttom blank
        for (; count < wholeArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
    } else if (text == 'g' || text == 'j' || text == 'p' || text == 'q' || text == 'y') {
        topArea = bitmap->width * (bitmap->width - bitmap->rows);

        //fill the top blank
        for (count = 0; count < topArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < wholeArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                fontBitmap->data[count].b = 255;
                fontBitmap->data[count].g = 255;
                fontBitmap->data[count].r = 255;
                fontBitmap->data[count].a = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                fontBitmap->data[count].b = color->b;
                fontBitmap->data[count].g = color->g;
                fontBitmap->data[count].r = color->r; 
                fontBitmap->data[count].a = bitmap->buffer[count - topArea];
            }
        }
    }
//176 is the degree sign
    else if (text == 176) {
        topArea = bitmap->width * (2 * bitmap->rows);
        baseArea = bitmap->width * (3 * bitmap->rows);

        //fill the top blank
        for (count = 0; count < topArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
        //for the text data format, put the gray to the fourth bit
        for (; count < baseArea; count++) {
            //follow the rules in original bmp data
            if (bitmap->buffer[count - topArea] == 0) {
                // 3 * 255 means white color, 0 means full transparent
                fontBitmap->data[count].b = 255;
                fontBitmap->data[count].g = 255;
                fontBitmap->data[count].r = 255;
                fontBitmap->data[count].a = 0;
            } else if (bitmap->buffer[count - topArea] > 0) {
                //3 * 255 means white color, transparent equals gray
                fontBitmap->data[count].b = color->b;
                fontBitmap->data[count].g = color->g;
                fontBitmap->data[count].r = color->r; 
                fontBitmap->data[count].a = bitmap->buffer[count - topArea];
            }
        }
        //fill buttom blank
        for (; count < wholeArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
    } else if (text = ' ') {
        //fill blank
        for (count = 0; count < wholeArea; count++) {
            fontBitmap->data[count].b = 255;
            fontBitmap->data[count].g = 255;
            fontBitmap->data[count].r = 255;
            fontBitmap->data[count].a = 0;
        }
    }
	#endif
}
#endif
