#ifndef __BMPTOC_H
#define __BMPTOC_H

typedef unsigned short WORD;
typedef	unsigned int DWORD;

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

#define MAXBMPCOLORMAP	256
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define LM_to_uint(a,b)   (((b)<<8)|(a)) 
#define	CHAR_TO_DWORD(a,b,c,d)  ((a << 24)|(b<<16)|(c<<8)|(d))  

#define skip(n) do { argc -= (n); argv += (n); } while (0)
#define MAX_PATH 1024

#define OUT_PATH "../out"
#define BMP_OUT_PATH "BmpDataArray"

#endif 
