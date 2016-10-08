#ifndef _GETBMPFT_H_
#define _GETBMPFT_H_

#include "freetype.h"

class GetBmpFT {
public:

    int width;
    int height;
    int count;
    int numOfWords;
    FreeType2* pFreeType2;
    unsigned char* FT2Image;

    GetBmpFT(const char * fontPath, int fontWidth, int fontHeight, const char * fontColor);
    ~GetBmpFT(void);
    void GetWord(unsigned char* pic_time_morning[], int index, unsigned long code);
    void GetLowercase(unsigned char* pic_texta_z[]);
    void GetNumber(unsigned char* pic_text0_9[]);
    void GetCapital(unsigned char* pic_textA_Z[]);
    void GetPunctuation(unsigned char* pic_punctuation[]);       
    void GetLowercaseEn(unsigned char* pic_texta_z[]);
    void GetNumberEn(unsigned char* pic_text0_9[]);
    void GetCapitalEn(unsigned char* pic_textA_Z[]);
    void GetPunctuationEn(unsigned char* pic_punctuation[]);   

};

#endif
