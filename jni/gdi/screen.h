#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "flagdefine.h"

typedef struct _screendevice SCREENDEVICE;
struct _screendevice 
{
	int	xres;		/* X screen res (real) */
	int	yres;		/* Y screen res (real) */
	int	xvirtres;	/* X drawing res (will be flipped in portrait mode) */
	int yvirtres;	/* Y drawing res (will be flipped in portrait mode) */
 	int	bpp;		/* # bpp*/
	int	linelen;	/* line length in bytes for bpp 1,2,4,8*/
					/* line length in pixels for bpp 16, 24, 32*/
	int	size;		/* size of memory allocated*/
	long ncolors;	/* # screen colors*/
	int	 pixtype;	/* format of pixel value*/
	int	flags;		/* device flags*/
	int hfile;
#if (LCDBUFFER == 3)
	PIXEL *pLCDBuffer0;	
#endif
	PIXEL *pLCDBuffer1;
	PIXEL *pLCDBuffer2;
	PIXEL *pBackupBuffer;
	PIXEL *pDisplayBuffer;
};

int	Screen_open( );
void Screen_close( );
PIXEL* GetBackupBuffer(void);
PIXEL** GetBackupBufferAddr(void);
PIXEL* GetDisplayBuffer(void);
void SaveBackupBuffer(void);
void LcdSwapBuffer(void);
void clearlogo(void);
void ScreenInit(void);
int testBmp();

#endif
