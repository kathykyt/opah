#ifndef _DEVDRAW_H_
#define _DEVDRAW_H_

#include "flagdefine.h"
//#include "rect.h"

typedef struct _RGBQUAD
{
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
}RGBQUAD;

PIXEL* CreateArea(int width, int height);
void DelArea(PIXEL* pArea);
void AreaFill(PIXEL* pDesBuf, int x, int y, int width, int height, PIXEL color);
int AreaDraw(PIXEL *desBuf, int desX, int desY, PIXEL *srcBuf, int srcX, int srcY,
             int W, int H, int Mode);
int AreaAlphaMask(PIXEL *desBuf, int desX, int desY, PIXEL *srcBuf, int srcX, int srcY,
                  int W, int H);
// int DrawString(PIXEL *desBuf, Rect* destRect, PIXEL *srcBuf, Rect* srcRect, int Mode);
int DrawString(PIXEL *desBuf, int desX, int desY, PIXEL *srcBuf, int srcX, int srcY,
             int W, int H, int Mode);


#endif
