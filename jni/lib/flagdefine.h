#ifndef _FLAGDEFINE_H_
#define _FLAGDEFINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <assert.h>
#include <termios.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/rtc.h>
#include <asm/unistd.h>

#define SOFT_VERSION		"V1.0"

#define ALIGN_32(a) (((a) + 31) & (~31))
#define ALIGN_16(a) (((a) + 15) & (~15))
#define ALIGN_8(a) (((a) + 7) & (~7))
#define ALIGN_4(a) (((a) + 3) & (~3))
#define ALIGN_2(a) (((a) + 1) & (~1))
#define ALIGN_CUT_32(a) ((a) & (~31))
#define ALIGN_CUT_16(a) ((a) & (~15))
#define ALIGN_CUT_8(a) ((a) & (~7))
#define ALIGN_CUT_4(a) ((a) & (~3))
#define ALIGN_CUT_2(a) ((a) & (~1))

#define false 0
#define true  1

#ifndef ON
	#define ON 0
#endif

#ifndef OFF
	#define OFF -1
#endif

#if (COLORPIXEL565 == 1)
#define PIXEL   unsigned short
#define EVERY_PIXEL_BYTES	2
#define TRANSCOLOR    0xf81f
#define lcd_bpp       16 
#define RGB2PIXEL(r,g,b)	 ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))
#define RGB565(R, G, B)	 ((((R) & 0xf8) << 8) | (((G) & 0xfc) << 3) | (((B) & 0xf8) >> 3))
#define PIXELRED8(pixelval)          (((pixelval) >> 11) & 0x1f)
#define PIXELGREEN8(pixelval)        (((pixelval) >> 5) & 0x3f)
#define PIXELBLUE8(pixelval)         ((pixelval) & 0x1f)
#define COLOR_LIGHT  RGB565(200, 200, 200)
#define COLOR_DARK RGB565(0, 0, 0)
#define COLOR_RED RGB565(255, 0, 0)
#define PICDIR    "/data/pic/"
#else
#define PIXEL   unsigned int
#define EVERY_PIXEL_BYTES	4
#define TRANSCOLOR    0xff00ff
#define lcd_bpp       32 
#define RGB2PIXEL(r,g,b)	 	( ( r << 16) |  ( g << 8) | ( b))
#define RGB888(R,G,B)	 ( ( R << 16) |  ( G << 8) | ( B))
#define RGB8888(A,R,G,B)	 (( A << 24) | ( R << 16) |  ( G << 8) | ( B))
#define PIXELALPHA(pixelval)         (((pixelval) >>  24) & 0xff)
#define PIXELRED8(pixelval)          (((pixelval) >>  16) & 0xff)
#define PIXELGREEN8(pixelval)        (((pixelval) >> 8) & 0xff)
#define PIXELBLUE8(pixelval)         ((pixelval)       & 0xff)
#define COLOR_LIGHT   RGB888(200,200,200)
#define COLOR_DARK	RGB888(0,0,0)
#define COLOR_RED	RGB888(255,0,0)
#define PICDIR    "/data/pic8888/"
#endif

#define GETABS(a)   (((a)<0)?(0-(a)):(a))
#define GETMAX(a,b) (((a)>(b))?(a):(b))
#define GETMIN(a,b) (((a)<(b))?(a):(b))
#define INRANGE(x,a,b)		((((x)>=(a))&&((x)<=(b)))?1:0)
#define DIFF(x,a,b)			((((x)+(a))>(b))?((b)-(a)):(x))

#define REPLACE_STYLE			0
#define TRANS_STYLE				1
#define ALPHA_STYLE				2

#ifndef _DWORD
#define _DWORD
typedef	unsigned int DWORD;
#endif

#ifndef _WORD
#define _WORD
typedef unsigned short WORD;
#endif

#ifndef _BYTE
#define _BYTE
typedef unsigned char BYTE;
#endif

#define BIT0 0x0001
#define BIT1 0x0002
#define BIT2 0x0004
#define BIT3 0x0008
#define BIT4 0x0010
#define BIT5 0x0020
#define BIT6 0x0040
#define BIT7 0x0080
#define BIT8 0x0100
#define BIT9 0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#define BUTTONDOWN			1
#define BUTTONUP			0
#define MOVELEFTRIGHT		(0&BIT0)
#define MOVEUPDOWN			(BIT0)
#define MOVECYCLE			(BIT1)
#define MOVESPRING			(BIT2)

#endif
