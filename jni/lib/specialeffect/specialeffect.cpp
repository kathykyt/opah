#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "screen.h"
#include "specialeffect.h"

static void EffectDelay(int delay)
{
	volatile int i = delay;
	while(i--)
		;
}

int LeftPushToRight(PIXEL *desBuf, int desX, int desY, int desW, int desH, 
                         PIXEL *srcBuf, int srcX, int srcY, int srcW, int srcH, 
                         int W, int H)
{
	int i,j,k;
	PIXEL* pDes;
	PIXEL* pSrc;
	int step = 2;
	pDes = (PIXEL*)desBuf+desY*desW+desX;
	pSrc = (PIXEL*)srcBuf+srcY*srcW+srcX;
	for(k=0; k<W/step; k++)
	{
		for(j=0; j<H; j++)
		{

#if defined(LCD_ROTATE_90)

			for(i=0; i<W-step; i++)
				*(pDes+(W-1-i)*desW+(LCDH-1)-j) = *(pDes+(W-1-i-step)*desW+(LCDH-1)-j);
			for(i=0; i<step; i++)
				*(pDes+i*desW+(LCDH-1)-j) = *(pSrc+(W-1-k*step-step+1+i)*srcW+(LCDH-1)-j);

#elif defined(LCD_ROTATE_270)
			
			for(i=0; i<W-step; i++)
				*(pDes+((LCDW-1)-(W-1-i))*desW+j) = *(pDes+((LCDW-1)-(W-1-i-step))*desW+j);
			for(i=0; i<step; i++)
				*(pDes+((LCDW-1)-i)*desW+j) = *(pSrc+((LCDW-1)-(W-1-k*step-step+1+i))*srcW+j);

#else

			for(i=0; i<W-step; i++)
				*(pDes+j*desW+W-1-i) = *(pDes+j*desW+W-1-i-step);
			for(i=0; i<step; i++)
				*(pDes+j*desW+i) = *(pSrc+j*srcW+W-1-k*step-step+1+i);

#endif
		}
		EffectDelay(100);
	}
	return 0;
}

int RightPushToLeft(PIXEL *desBuf, int desX, int desY, int desW, int desH,
                         PIXEL *srcBuf, int srcX, int srcY, int srcW, int srcH,
                         int W, int H)
{
	int i,j,k;
	PIXEL* pDes;
	PIXEL* pSrc;
	int step = 2;
	pDes = (PIXEL*)desBuf+desY*desW+desX;
	pSrc = (PIXEL*)srcBuf+srcY*srcW+srcX;
	for(k=0; k<W/step; k++)
	{
		for(j=0; j<H; j++)
		{
#if defined(LCD_ROTATE_90)

			for(i=0; i<W-step; i++)
				*(pDes+i*desW+(LCDH-1)-j) = *(pDes+(i+step)*desW+(LCDH-1)-j);
			for(i=0; i<step; i++)
				*(pDes+(W-1-step+1+i)*desW+(LCDH-1)-j) = *(pSrc+(k*step+i)*srcW+(LCDH-1)-j);
			
#elif defined(LCD_ROTATE_270)

			for(i=0; i<W-step; i++)
				*(pDes+((LCDW-1)-i)*desW+j) = *(pDes+((LCDW-1)-(i+step))*desW+j);
			for(i=0; i<step; i++)
				*(pDes+((LCDW-1)-(W-1-step+1+i))*desW+j) = *(pSrc+((LCDW-1)-(k*step+i))*srcW+j);
			
#else

			for(i=0; i<W-step; i++)
				*(pDes+j*desW+i) = *(pDes+j*desW+i+step);
			for(i=0; i<step; i++)
				*(pDes+j*desW+W-1-step+1+i) = *(pSrc+j*srcW+k*step+i);
			
#endif
		}
		EffectDelay(100);
	}
	return 0;
}

int RGBScale(PIXEL *desBuf,int desX,int desY,int desW,int desH, 
                 PIXEL *srcBuf,int srcX,int srcY,int srcW,int srcH)
{
	PIXEL *pDestDate,*pSrcDate;
	PIXEL *pDestDatetemp,*pSrcDatetemp;
	int i=0;
	int j=0;
	int row=0;
	int col=0;
	for(i=0;i<desH;i++)
	{
		row=(i*srcH)/desH;
		pSrcDatetemp=srcBuf+(srcY+row)*srcW+srcX;
		pDestDatetemp=desBuf+(desY+i)*desW+desX;
		for(j=0;j<desW;j++)
		{
			col=(j*srcW)/desW;
			pSrcDate=pSrcDatetemp+col;
			pDestDate=pDestDatetemp+j;
			*pDestDate=*pSrcDate;
		}
	}
	return 0;
}

int SwapBufferEffect(int EffectMode)
{
	switch(EffectMode)
	{
		case EFFECT_NONE:
			LcdSwapBuffer();
			break;
		case EFFECT_LEFTPUSHTORIGHT:
			LeftPushToRight(GetDisplayBuffer(),0,0,LCDW,LCDH,
			                GetBackupBuffer(),0,0,LCDW,LCDH,
			                LCDW,LCDH);
			break;
		case EFFECT_RIGHTPUSHTOLEFT:
			RightPushToLeft(GetDisplayBuffer(),0,0,LCDW,LCDH,
			                GetBackupBuffer(),0,0,LCDW,LCDH,
			                LCDW,LCDH);
			break;
		default:
			ERROR_X("error! EffectMode should be set\n");
			break;
	}
	return 0;
}
