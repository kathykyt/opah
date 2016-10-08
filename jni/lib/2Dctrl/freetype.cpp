#include "freetype.h"

FreeType2::FreeType2(const char * fontPath, unsigned int fontWidth, unsigned int fontHeight, const char * fontColor) {

    //default color is white.
    colorR = 255;
    colorG = 255;
    colorB = 255;

    if(strcmp(fontColor, "gray") == 0)
    {
	colorR = 200;
	colorG = 200;
	colorB = 200;
    }    
    if(strcmp(fontColor, "yellow") == 0)
    {
	colorR = 255;
	colorG = 255;
	colorB = 0;
    }


    error = FT_Init_FreeType(&library);
    if (error)
        printf("Library Error\n");
    error = FT_New_Face(library, fontPath, 0, &face);

    if (error)
        printf("Face Error\n");
    //fontWidth * 64 is char_width in 1/64th of points
    //100 are horizontal and vertical device resolution(100dpi)
    //    error = FT_Set_Char_Size(face, fontWidth * 64, fontHeight * 64, 100, 100);

    //handle the align baseline issue
    //fontWidth and fontHeight are in pixels, fontHeight should equal to the height of text view
    //    int height = fontHeight;
    //    int baseHeight = 0.75 * height;//assume a baseline
    height = fontHeight;
    baseHeight = height * 0.85;
    spaceWidth = fontWidth * 0.3;//TODO!!! HACK!!! DANGEROUS!!! this value cause segmentation fault!!!
    if(spaceWidth == 0) {
		spaceWidth = 4;
	}
    error = FT_Set_Pixel_Sizes(face, fontWidth, height * 0.8);//should not use a fix value 0.6
    if (error)
        printf("Set_Char_Size Error\n");
}

FreeType2::~FreeType2() {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

unsigned char* FreeType2::getBmpData(unsigned long text) {
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
    bitmap = &slot->bitmap;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#if 0
//this is the baseline for character and numbers in English

//handle the case under the baseline 
//176 represent temperature
if(text!='g' && text!='j' && text!='p' && text!='q' && text!= 'y' && text!=176 && text!='-' && text!=' ' && 
		!(0x0030 <= text && text <= 0x0039)){
    int wholeArea = bitmap->width * height;
    int topArea = bitmap->width * ((height - bitmap->rows) / 2);
    
    int baseArea = bitmap->width * bitmap->rows + topArea;
    //int topArea = bitmap->width * (baseHeight - bitmap->rows);
    //int baseArea = bitmap->width * baseHeight;
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char) + 8);
    memset(imageRGB, 0, 4 * wholeArea);

    //put width and height into header
    imageRGB[0] = bitmap->width;

    //imageRGB[4] = bitmap->rows;
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
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
}
else if (0x0030 <= text && text <= 0x0039){
	int wholeArea = bitmap->width * height;
    int topArea = bitmap->width * (baseHeight - bitmap->rows);
    int baseArea = bitmap->width * baseHeight;
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);

    //put width and height into header
    imageRGB[0] = bitmap->width;

    //imageRGB[4] = bitmap->rows;
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
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
}
else if(text=='g' || text=='j' || text=='p' || text=='q' || text== 'y') {
    int wholeArea = height * bitmap->width;
    int topArea = bitmap->width * (height - bitmap->rows);
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
            imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count - topArea];
        }
    }
}
//176 is the degree sign
else if(text==176){
    int wholeArea = height * bitmap->width;
    int topArea = bitmap->width * (2 * bitmap->rows);
    int baseArea = bitmap->width * (3 * bitmap->rows);
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
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
}
else if(text == ' '){
    int wholeArea = height * spaceWidth;
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);
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
//#endif
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    return imageRGB;
}


unsigned char* FreeType2::getBmpDataEn(unsigned long text) {
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
    bitmap = &slot->bitmap;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#if 0
//this is the baseline for character and numbers in English

//handle the case under the baseline 
//176 represent temperature
if(text!='g' && text!='j' && text!='p' && text!='q' && text!= 'y' && text!=176 && text!='-' && text!=' ' && 
		!(0x0030 <= text && text <= 0x0039)){
    int wholeArea = bitmap->width * height;
    //int topArea = bitmap->width * ((height - bitmap->rows) / 2);
    //int baseArea = bitmap->width * bitmap->rows + topArea;
    int topArea = bitmap->width * (baseHeight - bitmap->rows);
    int baseArea = bitmap->width * baseHeight;
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char) + 8);
    memset(imageRGB, 0, 4 * wholeArea);

    //put width and height into header
    imageRGB[0] = bitmap->width;

    //imageRGB[4] = bitmap->rows;
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
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
}
else if (0x0030 <= text && text <= 0x0039){
	int wholeArea = bitmap->width * height;
    int topArea = bitmap->width * (baseHeight - bitmap->rows);
    int baseArea = bitmap->width * baseHeight;
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);

    //put width and height into header
    imageRGB[0] = bitmap->width;

    //imageRGB[4] = bitmap->rows;
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
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
}
else if(text=='g' || text=='j' || text=='p' || text=='q' || text== 'y') {
    int wholeArea = height * bitmap->width;
    int topArea = bitmap->width * (height - bitmap->rows);
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
            imageRGB[count * 4 + 3 + 8] = bitmap->buffer[count - topArea];
        }
    }
}
//176 is the degree sign
else if(text==176){
    int wholeArea = height * bitmap->width;
    int topArea = bitmap->width * (2 * bitmap->rows);
    int baseArea = bitmap->width * (3 * bitmap->rows);
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);
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
            imageRGB[count * 4 + 8] = colorB;
            imageRGB[count * 4 + 1 + 8] = colorG;
            imageRGB[count * 4 + 2 + 8] = colorR;
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
}
else if(text == ' '){
    int wholeArea = height * spaceWidth;
    imageRGB = NULL;
    imageRGB = (unsigned char*) malloc(4 * wholeArea * sizeof (unsigned char));
    memset(imageRGB, 0, 4 * wholeArea);
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
//#endif
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    return imageRGB;
}


