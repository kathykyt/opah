#ifndef _LOADBMP_H_
#define _LOADBMP_H_

#include <stdio.h>
#include <stdlib.h>
#include "flagdefine.h"

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        DWORD      biWidth;
        DWORD      biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        DWORD      biXPelsPerMeter;
        DWORD      biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBMPDATA {
		unsigned int width;
		unsigned int height;
		unsigned int DataSize;
		PIXEL* pData;
} BMPDATA;

#define MAXBMPCOLORMAP	256
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define LM_to_uint(a,b)   (((b)<<8)|(a)) 
#define	CHAR_TO_DWORD(a,b,c,d)  ((a << 24)|(b<<16)|(c<<8)|(d))  

int GetBMP(unsigned char* InputPic, int* width, int* height, PIXEL** pBmpData, void** objectID);
int FreeBmp(PIXEL* pBmpData);
int SetPixelToBuf(PIXEL Color, int width, int height, PIXEL* pData);
int CreateBMP(PIXEL Color, int width, int height, PIXEL** pBmpData);

#define GetBMP_wid(InputPic, width, height, pBmpData, objectID) GetBMP(InputPic, width, height, pBmpData, NULL)


#endif
