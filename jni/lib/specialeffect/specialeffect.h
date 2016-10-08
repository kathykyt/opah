#ifndef _SPECIALEFFECT_H_
#define _SPECIALEFFECT_H_

#include "flagdefine.h"

#define EFFECT_NONE					1
#define EFFECT_LEFTPUSHTORIGHT		2
#define EFFECT_RIGHTPUSHTOLEFT		3

int LeftPushToRight(PIXEL *desBuf, int desX, int desY, int desW, int desH, 
                    PIXEL *srcBuf, int srcX, int srcY, int srcW, int srcH, 
                    int W, int H);
int RightPushToLeft(PIXEL *desBuf, int desX, int desY, int desW, int desH,
                         PIXEL *srcBuf, int srcX, int srcY, int srcW, int srcH,
                         int W, int H);

int RGBScale(PIXEL *desBuf,int desX,int desY,int desW,int desH, 
                 PIXEL *srcBuf,int srcX,int srcY,int srcW,int srcH);
int SwapBufferEffect(int EffectMode);

#endif
