#include <linux/fb.h>
#include "log.h"
#include "screen.h"
#include "videodev2.h"
#include "devdraw.h"
#include "loadbmp.h"

//david hack
//#define OUTPUT_RGB565_FB
//#undef OUTPUT_RGB565_FB

#ifdef OUTPUT_RGB565_FB
#define RGB565_BYTE_PER_PIXEL  2
#define EVERY_PIXEL_BYTES RGB565_BYTE_PER_PIXEL
#endif



extern unsigned char pic_gif_chaocai_chaocai3[]; 

extern unsigned char pic_home_1_02_png[];

struct fb_var_screeninfo screeninfo;
struct fb_fix_screeninfo fix;
SCREENDEVICE g_screenInfo; 

int	Screen_open( )
{ 
	//the width of the LCD,in pixel
	g_screenInfo.xres = LCDW; // ric change 160
	//the height of the LCD,in pixel
	g_screenInfo.yres  = LCDH;//ric change 105 
	g_screenInfo.size = (LCDH * LCDW * EVERY_PIXEL_BYTES);
	//g_screenInfo.size = (LCDH * LCDW * RGB565_BYTE_PER_PIXEL);

	// open file
	g_screenInfo.hfile = open("/dev/graphics/fb0", O_RDWR);
	if(g_screenInfo.hfile <= 0)
	{
		g_screenInfo.hfile = open("/dev/fb0", O_RDWR);
		if(g_screenInfo.hfile <= 0)
		{
			ERROR_X("error! Can not open /dev/fb0 or /dev/graphics/fb0\n");
			return -1;
		}
	}

	if (ioctl(g_screenInfo.hfile,FBIOGET_VSCREENINFO,&screeninfo) < 0) {
		ERROR_X("error! ioctl FBIOGET_VSCREENINFO\n");
		close(g_screenInfo.hfile);
		return -1;
	}
	INFO_X("open lcd file:%d, W:%d, h:%d, bitperpixel = %d, yoffset=%x\n", g_screenInfo.hfile, screeninfo.xres, screeninfo.yres, screeninfo.bits_per_pixel, screeninfo.yoffset);

	
	if (ioctl(g_screenInfo.hfile, FBIOGET_FSCREENINFO, &fix) < 0) {
		ERROR_X("error! ioctl FBIOGET_FSCREENINFO\n");
		close(g_screenInfo.hfile);
		return -1;
	}

#ifdef GOLDFISH
	g_screenInfo.pBackupBuffer = (PIXEL*)mmap(NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, g_screenInfo.hfile, 0);
	if (g_screenInfo.pBackupBuffer == MAP_FAILED) {
		ERROR_X("error! LCD Video Map failed!\n");
		exit(0);
	}
	memset(g_screenInfo.pBackupBuffer, 0, fix.smem_len);
	g_screenInfo.pLCDBuffer1 = g_screenInfo.pBackupBuffer;
	g_screenInfo.pLCDBuffer2 = (PIXEL*)((unsigned char *)g_screenInfo.pBackupBuffer + g_screenInfo.size);
	g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer1;
	g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer2;
#else
#if (LCDBUFFER == 3)
	g_screenInfo.pLCDBuffer0 = (PIXEL*)mmap(NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, g_screenInfo.hfile, 0);
	if (g_screenInfo.pLCDBuffer0 == MAP_FAILED) {
		ERROR_X("error! LCD Video Map failed!\n");
		exit(0);
	}
	memset(g_screenInfo.pLCDBuffer0, 0, fix.smem_len);
#ifdef OUTPUT_RGB565_FB
	  g_screenInfo.pLCDBuffer1 = (unsigned short*)((unsigned char *)g_screenInfo.pLCDBuffer0 + fix.smem_len/(screeninfo.yres_virtual/screeninfo.yres));
	  g_screenInfo.pLCDBuffer2 = (unsigned short *)((unsigned char *)g_screenInfo.pLCDBuffer0 + 2*fix.smem_len/(screeninfo.yres_virtual/screeninfo.yres));

#else
	  g_screenInfo.pLCDBuffer1 = (PIXEL*)((unsigned char *)g_screenInfo.pLCDBuffer0 + fix.smem_len/(screeninfo.yres_virtual/screeninfo.yres));
	  g_screenInfo.pLCDBuffer2 = (PIXEL*)((unsigned char *)g_screenInfo.pLCDBuffer0 + 2*fix.smem_len/(screeninfo.yres_virtual/screeninfo.yres));

#endif
	g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer1;
	g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer0;
#endif
#if (LCDBUFFER == 2)
	g_screenInfo.pBackupBuffer = (PIXEL*)mmap(NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, g_screenInfo.hfile, 0);
	if (g_screenInfo.pBackupBuffer == MAP_FAILED) {
		ERROR_X("error! LCD Video Map failed!\n");
		exit(0);
	}
	memset(g_screenInfo.pBackupBuffer, 0, fix.smem_len);
	INFO_X("fix.smem_len = %d, screeninfo.yres_virtual=%d\n", fix.smem_len, screeninfo.yres_virtual);
	g_screenInfo.pLCDBuffer1 = g_screenInfo.pBackupBuffer;
	
//david hack, we need to check if the following is correct.
/*
	g_screenInfo.pLCDBuffer2 = (PIXEL*)((unsigned char *)g_screenInfo.pBackupBuffer + fix.smem_len/(screeninfo.yres_virtual/screeninfo.yres));
*/
	g_screenInfo.pLCDBuffer2 = (PIXEL*)((unsigned char *)g_screenInfo.pBackupBuffer +  
				 ((screeninfo.xres*screeninfo.yres*screeninfo.bits_per_pixel) / 8) );

	g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer1;
	g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer2;
#endif
#endif

#if 0
	char* buff1;
	char* buff2;
	struct timeval tv,tv1,tv2;
	buff1 = malloc(LCDW*LCDH*4);
	buff2 = malloc(LCDW*LCDH*4);
	gettimeofday(&tv,NULL);
	memset(buff2,0,LCDW*LCDH*4);
	gettimeofday(&tv1,NULL);
	memcpy(buff1,buff2,LCDW*LCDH*4);
	gettimeofday(&tv2,NULL);
	printf("tv1:%dus\n",((tv1.tv_sec-tv.tv_sec)*1000000+tv1.tv_usec-tv.tv_usec));
	printf("tv2:%dus\n",((tv2.tv_sec-tv1.tv_sec)*1000000+tv2.tv_usec-tv1.tv_usec));
	while(1);
#endif
#if 0
	int i,j,k,m;
	for(m=0xc0;m>0x40;m--)
	{
		for(j=0;j<LCDH;j++)
		{
			k = j*LCDW;
			for(i=0;i<LCDW;i++)
			{
				*((PIXEL*)(g_screenInfo.pDisplayBuffer)+k+i) = m;
			}
		}
		printf("***%d***\n",m);
	}	
#endif
#if 0
	clearlogo();
	printf("***1***\n");
	AreaFill(GetBackupBuffer(), 0, 0, LCDW, LCDH, COLOR_RED);
	LcdSwapBuffer();
	usleep(1000000);
//	printf("***2***\n");
//	testBmp();
//	usleep(1000000);
	printf("***3***\n");
	AreaFill(GetBackupBuffer(), 0, 0, LCDW, LCDH, COLOR_LIGHT);
	LcdSwapBuffer();
	usleep(1000000);
//	printf("***4***\n");
//	testBmp();
//	usleep(1000000);
	printf("***5***\n");
	while(1);
#endif
	return 0;
}

void Screen_close( )
{ 
	close ( g_screenInfo.hfile);
}

SCREENDEVICE* Screen_GetScreeninfo( )
{
	return &g_screenInfo;
}

PIXEL* GetBackupBuffer(void)
{
	return g_screenInfo.pBackupBuffer;
}

#if (LCDBUFFER == 3)
PIXEL* GetNextBackupBuffer(void)
{
	if(g_screenInfo.pBackupBuffer == g_screenInfo.pLCDBuffer0)
	{
		if(g_screenInfo.pDisplayBuffer == g_screenInfo.pLCDBuffer1)
			return g_screenInfo.pLCDBuffer2;
		else
			return g_screenInfo.pLCDBuffer1;
	}
	else if(g_screenInfo.pBackupBuffer == g_screenInfo.pLCDBuffer1)
	{
		if(g_screenInfo.pDisplayBuffer == g_screenInfo.pLCDBuffer0)
			return g_screenInfo.pLCDBuffer2;
		else
			return g_screenInfo.pLCDBuffer0;
	}
	else if(g_screenInfo.pBackupBuffer == g_screenInfo.pLCDBuffer2)
	{
		if(g_screenInfo.pDisplayBuffer == g_screenInfo.pLCDBuffer0)
			return g_screenInfo.pLCDBuffer1;
		else
			return g_screenInfo.pLCDBuffer0;
	}
}
#endif

void SaveBackupBuffer(void)
{
#ifdef OUTPUT_RGB565_FB

	memcpy((unsigned char*)GetBackupBuffer(), (unsigned char*)GetDisplayBuffer(), LCDW*LCDH*RGB565_BYTE_PER_PIXEL);
#else
	memcpy((unsigned char*)GetBackupBuffer(), (unsigned char*)GetDisplayBuffer(), LCDW*LCDH*EVERY_PIXEL_BYTES);

#endif

#if (LCDBUFFER == 3)
	memcpy((unsigned char*)GetNextBackupBuffer(), (unsigned char*)GetDisplayBuffer(), LCDW*LCDH*EVERY_PIXEL_BYTES);
#endif
}

PIXEL** GetBackupBufferAddr(void)
{
	return &(g_screenInfo.pBackupBuffer);
}

PIXEL* GetDisplayBuffer(void)
{
	return g_screenInfo.pDisplayBuffer;
}

void LcdSwapBuffer(void)
{
#if 0
	int fd ;
	int len;
	fd = open("imageFile.dat", O_RDWR|O_CREAT);
	if(fd < 0)
	{
		ERROR_X("open imageFile.data fail\n");
		return;	
	}
	len = LCDW*LCDH*4;
	if( len == write(fd, g_screenInfo.pVideoBuffer, len))
	{
		INFO_X("save date success\n");
	}
	close(fd );
#endif
	//screeninfo.activate = FB_ACTIVATE_NOW;
	screeninfo.activate = FB_ACTIVATE_VBL;
#if (LCDBUFFER == 2)	
	if(g_screenInfo.pBackupBuffer == g_screenInfo.pLCDBuffer1){
		g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer2;
		g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer1;
		screeninfo.yoffset = 0;
	}else{
		g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer1;
		g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer2;
		screeninfo.yoffset = screeninfo.yres;
	}
#endif
#if (LCDBUFFER == 3)	
	if(g_screenInfo.pBackupBuffer == g_screenInfo.pLCDBuffer0){
		g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer1;
		g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer0;
		screeninfo.yoffset = 0;
	}
	else if(g_screenInfo.pBackupBuffer == g_screenInfo.pLCDBuffer1){
		g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer2;
		g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer1;
		screeninfo.yoffset = screeninfo.yres;
	}
	else{
		g_screenInfo.pBackupBuffer = g_screenInfo.pLCDBuffer0;
		g_screenInfo.pDisplayBuffer = g_screenInfo.pLCDBuffer2;
		screeninfo.yoffset = screeninfo.yres*2;
	}
#endif	
	if (ioctl(g_screenInfo.hfile, FBIOPUT_VSCREENINFO, &screeninfo) == -1) {
		ERROR_X("error! FBIOPUT_VSCREENINFO failed\n");
	}
}

void clearlogo(void)
{
#ifndef GOLDFISH
    int dev_fd = 0;
    int type  = 0;
    dev_fd = open("dev/video1",O_RDWR | O_NDELAY);
    ioctl(dev_fd, BASE_VIDIOC_PRIVATE+1, &type);
    close(dev_fd);
	INFO_X("clear logo\n");
#endif        
}

void ScreenInit(void)
{
	Screen_open();

	//LoadFont();
}

int testBmp()
{
#if 0
	PIXEL* pBmpData;
	int width,height;
	void* ObjectID = NULL;
	//if(GetBMP_wid(pic_heater_bg_newzhengzhu, &width, &height, &pBmpData, &ObjectID) == 0)
	//if(GetBMP_wid(pic_home_1_02_png, &width, &height, &pBmpData, &ObjectID) == 0)
	//if(GetBMP_wid(pic_gif_chaocai_chaocai8, &width, &height, &pBmpData, &ObjectID) == 0)
	

	//{
		//AreaDraw(GetBackupBuffer(), 0, 0, pBmpData, 0, 0, width, height, REPLACE_STYLE);
		//AreaDraw(GetBackupBuffer(), 100, 100, pBmpData, 0, 0, width, height, TRANS_STYLE);
		//INFO_X("bmp test end\n");
	//}
	LcdSwapBuffer();
#endif	
	return 0;
}
