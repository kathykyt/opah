#include <stdio.h>
#include <stdlib.h>
#include "flagdefine.h"
#include "screen.h"
#include "log.h"
#include "memorymanage.h"
#include "loadbmp.h"

// bmp数据进行xy镜像处理
static int XYMirrorBmp(int XMirror, int YMirror, int w, int h, PIXEL* pBmpData)
{
	int i,j;
	PIXEL Data;
	PIXEL *src,*des;

	if(XMirror)
	{
		for(i=0;i<w/2;i++)
		{
			for(j=0;j<h;j++)
			{
				src = ((PIXEL*)(pBmpData))+i+j*w; 
				des = ((PIXEL*)(pBmpData))+w-1-i+j*w; 
				Data = *des;
				*des = *src;
				*src = Data;
			}
		}
	}
	if(YMirror)
	{
		for(j=0;j<h/2;j++)
		{
			for(i=0;i<w;i++)
			{
				src = ((PIXEL*)(pBmpData))+i+j*w; 
				des = ((PIXEL*)(pBmpData))+i+(h-1-j)*w; 
				Data = *des;
				*des = *src;
				*src = Data;
			}
		}
	}
	return 0;
}

// 从库中对应的指针加载数据
int GetBMPFromArray(unsigned char* pData, int* width, int* height, PIXEL** pBmpData)
{
    unsigned char* p;
    if(pData == NULL)
    {
    	ERROR_X("error! GetBMPFromArray NULL\n");
    	return -1;
    }
    p = pData;
    *width = *((unsigned int*)p);
    p = p+4;
    *height = *((unsigned int*)p);
    p = p+4;
    *pBmpData = (PIXEL*)pData; 
    return 0;
}

// 从文件中读取Bmp的数据并加载到内存中
int GetBMPFromDir(char* InputPic, int* width, int* height, PIXEL** pBmpData)
{
	BITMAPFILEHEADER   bmpheader;   
	BITMAPINFOHEADER   bmpinfor;
	WORD BmpCompMeth = 0xffff;
	char TempBuf[256];
	int fd;

	strcpy(TempBuf, PICDIR);
	strcat(TempBuf, InputPic);
	fd = open(TempBuf, O_RDONLY);
	if (fd < 0)
	{
		ERROR_X("error! can not open BMP:%s\n",TempBuf);
		return -1;
	}
	read(fd, TempBuf, 60);

	bmpheader.bfType = LM_to_uint(TempBuf[0],TempBuf[1]);
	if(bmpheader.bfType != 0x4d42)//''bm"
	{
		ERROR_X("error! Not BMP File\n");
		close(fd);
		return -1;
	}
	bmpheader.bfSize = CHAR_TO_DWORD(TempBuf[5],TempBuf[4],TempBuf[3],TempBuf[2]);
	bmpheader.bfOffBits = CHAR_TO_DWORD(TempBuf[13],TempBuf[12],TempBuf[11],TempBuf[10]);
	bmpinfor.biSize = CHAR_TO_DWORD(TempBuf[17],TempBuf[16],TempBuf[15],TempBuf[14]);
	bmpinfor.biWidth = CHAR_TO_DWORD(TempBuf[21],TempBuf[20],TempBuf[19],TempBuf[18]);
	bmpinfor.biHeight = CHAR_TO_DWORD(TempBuf[25],TempBuf[24],TempBuf[23],TempBuf[22]);
	bmpinfor.biPlanes = LM_to_uint(TempBuf[26],TempBuf[27]);
	if(bmpinfor.biPlanes != 1)
	{	
		close(fd);
		return -1;
	}
	bmpinfor.biBitCount = LM_to_uint(TempBuf[28],TempBuf[29]);
	bmpinfor.biCompression = CHAR_TO_DWORD(TempBuf[33],TempBuf[32],TempBuf[31],TempBuf[30]);
	switch(bmpinfor.biCompression)
	{
		case 0:
			BmpCompMeth = BI_RGB;
			break;
		case 1:
			BmpCompMeth = BI_RLE8;
			break;
		case 2:
			BmpCompMeth = BI_RLE4;
			break;
		case 3:
			BmpCompMeth = BI_BITFIELDS;
			break;
		default:
			break;
	}
	bmpinfor.biSizeImage = CHAR_TO_DWORD(TempBuf[37],TempBuf[36],TempBuf[35],TempBuf[34]);
	bmpinfor.biXPelsPerMeter = CHAR_TO_DWORD(TempBuf[41],TempBuf[40],TempBuf[39],TempBuf[38]);
	bmpinfor.biYPelsPerMeter = CHAR_TO_DWORD(TempBuf[45],TempBuf[44],TempBuf[43],TempBuf[42]);
	bmpinfor.biClrUsed = CHAR_TO_DWORD(TempBuf[49],TempBuf[48],TempBuf[47],TempBuf[46]);
	bmpinfor.biClrImportant = CHAR_TO_DWORD(TempBuf[53],TempBuf[52],TempBuf[51],TempBuf[50]);

#if (COLORPIXEL565 == 1)
	if(bmpinfor.biBitCount != 16)
	{
		ERROR_X("error! only support 16bit 565 BMP!\n");
		close(fd);
		return -1;
	}
#else
	if(bmpinfor.biBitCount != 32)
	{
		ERROR_X("error! only support 32bit 8888 BMP!\n");
		close(fd);
		return -1;
	}
#endif
	if((BmpCompMeth == 1) || (BmpCompMeth == 2))
	{
		ERROR_X("error! only support uncompressed BMP!\n");
		close(fd);
		return -1;
	}

	*width = 0;
	*height = 0;
	*pBmpData = (PIXEL*)uimalloc(bmpinfor.biSizeImage+8);
	if(*pBmpData == NULL)
	{
		ERROR_X("error! BMP malloc Fail\n");
		close(fd);
		return -1;
	}
	lseek(fd, bmpheader.bfOffBits, SEEK_SET);
	read(fd, (unsigned char*)(*pBmpData)+8, bmpinfor.biSizeImage);
	close(fd);   	
	*width = bmpinfor.biWidth;
	*height = bmpinfor.biHeight;
	*(unsigned int*)(*pBmpData) = *width;
	*((unsigned int*)(*pBmpData)+1) = *height;	
	XYMirrorBmp(0, 1, *width, *height, (PIXEL*)((unsigned char*)(*pBmpData)+8));
	return 0;
}

static int iBmpNum = 0;
int GetBMPFromDataFile(unsigned char* InputPic, int* width, int* height, PIXEL** pBmpData, void** objectID)
{
#if 0
	char TempBuf[256];
	int fd;
	unsigned int size;
	int* ptr;

	strcpy(TempBuf, PICDIR);
	strcat(TempBuf, InputPic);
	fd = open(TempBuf, O_RDONLY);
	if (fd < 0)
	{
		ERROR_X("can not open BMP:%s\n",TempBuf);
		return -1;
	}
	read(fd, TempBuf, 8);

	*width = CHAR_TO_DWORD(TempBuf[3],TempBuf[2],TempBuf[1],TempBuf[0]);
	*height = CHAR_TO_DWORD(TempBuf[7],TempBuf[6],TempBuf[5],TempBuf[4]);
	if((*width < 0) || (*height < 0))
	{
		close(fd);
		return -1;
	}

	size = (*width) * (*height) * EVERY_PIXEL_BYTES;

	*pBmpData = GrAllocBuffer(size+8, objectID);
	if(*pBmpData == NULL)
	{
		ERROR_X("BMP malloc Fail\n");
		close(fd);
		return -1;
	}
	lseek(fd, 8, SEEK_SET);
	read(fd, (unsigned char*)(*pBmpData)+8, size);
	close(fd);
	*(unsigned int*)(*pBmpData) = *width;
	*((unsigned int*)(*pBmpData)+1) = *height;
	iBmpNum++;
	INFO_X("Bmp %d: (%d, %d)\n", iBmpNum, *width, *height);
	
#endif	
	return 0;
}

int GetBMP(unsigned char* InputPic, int* width, int* height, PIXEL** pBmpData, void** objectID)
{
	return GetBMPFromArray(InputPic, width, height, pBmpData);

}

// 释放bmp的数据内存
int FreeBmp(PIXEL* pBmpData)
{
	if(pBmpData != NULL)
		uifree(pBmpData);
	return 0;
}

int SetPixelToBuf(PIXEL Color, int width, int height, PIXEL* pData)
{
	int i;
	for(i=0; i<width*height; i++)
		*(pData+i) = Color;
	return 0;
}


int CreateBMP(PIXEL Color, int width, int height, PIXEL** pBmpData)
{
	*pBmpData = (PIXEL*)uimalloc(width*height*EVERY_PIXEL_BYTES);
	if(*pBmpData == NULL)
	{
		ERROR_X("error! CreateBMP malloc Fail\n");
		return -1;
	}
	SetPixelToBuf(Color, width, height, *pBmpData);
	return 0;
}

