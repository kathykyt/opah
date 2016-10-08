#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include "bmptoc.h"

int color565;
int fd_data;

void usage()
{
    printf("Usage: bmptodata [ <options> ] [ <BmpFileSrcPath> ]\n\n");
    printf("Options:\n");
    printf("  -h, --help - This help text.\n");
    printf("\n");
    printf("BmpFileSrcPath: Source file pathname\n");
    printf("\n");
    exit(1);
}

static int XYMirrorBmp(int XMirror, int YMirror, int w, int h, unsigned char* pBmpData)
{
	int i,j;
    unsigned short DataShort;
    unsigned int DataInt;
    unsigned short *srcShort,*desShort;
    unsigned int *srcInt,*desInt;

	if(XMirror)
	{
        if(color565 == 1)
        {
            for(i=0;i<w/2;i++)
            {
                for(j=0;j<h;j++)
                {
                    srcShort = ((unsigned short*)(pBmpData))+i+j*w; 
                    desShort = ((unsigned short*)(pBmpData))+w-1-i+j*w; 
                    DataShort = *desShort;
                    *desShort = *srcShort;
                    *srcShort = DataShort;
                }
            }
        }
        else
        {
            for(i=0;i<w/2;i++)
            {
                for(j=0;j<h;j++)
                {
                    srcInt = ((unsigned int*)(pBmpData))+i+j*w; 
                    desInt = ((unsigned int*)(pBmpData))+w-1-i+j*w; 
                    DataInt = *desInt;
                    *desInt = *srcInt;
                    *srcInt = DataInt;
                }
            }
        }
	}
	if(YMirror)
	{
        if(color565 == 1)
        {
            for(j=0;j<h/2;j++)
            {
                for(i=0;i<w;i++)
                {
                    srcShort = ((unsigned short*)(pBmpData))+i+j*w; 
                    desShort = ((unsigned short*)(pBmpData))+i+(h-1-j)*w; 
                    DataShort = *desShort;
                    *desShort = *srcShort;
                    *srcShort = DataShort;
                }
            }
        }
        else
        {
            for(j=0;j<h/2;j++)
            {
                for(i=0;i<w;i++)
                {
                    srcInt = ((unsigned int*)(pBmpData))+i+j*w; 
                    desInt = ((unsigned int*)(pBmpData))+i+(h-1-j)*w; 
                    DataInt = *desInt;
                    *desInt = *srcInt;
                    *srcInt = DataInt;
                }
            }
        }
	}
	return 0;
}

int GetBMP(char* InputPic, int* width, int* height, unsigned char** pBmpData)
{
	BITMAPFILEHEADER   bmpheader;   
	BITMAPINFOHEADER   bmpinfor;
	WORD BmpCompMeth;
	unsigned char TempBuf[256];
	int fd;

	fd = open(InputPic, O_RDONLY);
	if (fd < 0)
	{
		printf("can not open BMP:%s\n",TempBuf);
		return -1;
	}
	if(read(fd, TempBuf, 60) < 0)
    {
        printf("read bmp head fail\n");
        close(fd);
        return -1;
    }

	bmpheader.bfType = LM_to_uint(TempBuf[0],TempBuf[1]);
	if(bmpheader.bfType != 0x4d42)//''bm"
	{
		printf("Not BMP File\n");
		close(fd);
		return -1;
	}
	bmpheader.bfSize = CHAR_TO_DWORD(TempBuf[5],TempBuf[4],TempBuf[3],TempBuf[2]);
	bmpheader.bfOffBits = CHAR_TO_DWORD(TempBuf[13],TempBuf[12],TempBuf[11],TempBuf[10]);
	bmpinfor.biSize = CHAR_TO_DWORD(TempBuf[17],TempBuf[16],TempBuf[15],TempBuf[14]);
	bmpinfor.biWidth = CHAR_TO_DWORD(TempBuf[21],TempBuf[20],TempBuf[19],TempBuf[18]);
	bmpinfor.biHeight = CHAR_TO_DWORD(TempBuf[25],TempBuf[24],TempBuf[23],TempBuf[22]);
	if((bmpinfor.biWidth < 0) || (bmpinfor.biHeight < 0))
	{
		close(fd);
		return -1;
	}
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

    if(color565 == 1)
    {
        if(bmpinfor.biBitCount != 16)
        {
            printf("only support 16bit Bmp, BMP:%s is %d BitCount!\n", InputPic, bmpinfor.biBitCount);
            close(fd);
            return -1;
        }
    }
    else
    {
        if(bmpinfor.biBitCount != 32)
        {
            printf("only support 32bit Bmp, BMP:%s is %d BitCount!\n", InputPic, bmpinfor.biBitCount);
            close(fd);
            return -1;
        }
    }

	if((BmpCompMeth == 1) || (BmpCompMeth == 2))
	{
		printf("only support uncompressed BMP!\n");
		close(fd);
		return -1;
	}

	*width = 0;
	*height = 0;
	*pBmpData = malloc(bmpinfor.biSizeImage+8);
	if(*pBmpData == NULL)
	{
		printf("BMP malloc Fail\n");
		close(fd);
		return -1;
	}
	lseek(fd, bmpheader.bfOffBits, SEEK_SET);
	if(read(fd, ((unsigned char*)*pBmpData)+8, bmpinfor.biSizeImage) < 0)
    {
        printf("read bmp data fail\n");
        close(fd);
        return -1;
    }
	close(fd);   	
	*width = bmpinfor.biWidth;
	*height = bmpinfor.biHeight;
    *((unsigned int*)((unsigned char*)(*pBmpData)+0)) = *width;
    *((unsigned int*)((unsigned char*)(*pBmpData)+4)) = *height;
    //printf("GetBMP %s, w=%d, h=%d, size=%d\n", InputPic, *width, *height, bmpinfor.biSizeImage+8);
	XYMirrorBmp(0, 1, *width, *height, (unsigned char*)(((unsigned char*)*pBmpData)+8));
	return bmpinfor.biSizeImage;
}

int FreeBmp(unsigned char* pBmpData)
{
	if(pBmpData != NULL)
		free(pBmpData);
	return 0;
}

int main(int argc, char *argv[])
{
    char * psrcpathname;
    char * p;
    char * colorstr;
    char tmp[MAX_PATH];
    char tmpsrc[MAX_PATH];
    char path[MAX_PATH];
    int width,height;
    unsigned char* data;
    int length;

    skip(1);    
    if (argc == 0) 
    {
        usage();
        return 0;
    }
    if(argc > 0)
    {
        if((strcmp(*argv, "-h") == 0)||(strcmp(*argv, "--help") == 0))
        {
            usage();
            return 0;
        }
        psrcpathname = *argv;
        p = psrcpathname+strlen(psrcpathname)-1;
        if(*p == '/')
            *p = 0;

        skip(1);    
        colorstr = *argv;
        color565 = atoi(colorstr);
    }
    getcwd(path,MAX_PATH);
    p = psrcpathname;
    if((*p == '.') && (*(p+1) == '/'))
        p+=2; 
    psrcpathname = p;
    sprintf(tmpsrc, "%s/%s", path,psrcpathname);
    p = psrcpathname+strlen(psrcpathname)-strlen(".bmp");
    *p = 0;
    sprintf(tmp, "%s/%s.data", path,psrcpathname);
    *p = '.';
    fd_data = open(tmp, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if(fd_data < 0)
    {
        printf("can't open BmpData, pls check!\n");
        return -1;
    }
    length = GetBMP(tmpsrc, &width, &height, (unsigned char**)(&data));
    if(length != -1)
    {
        if(color565 == 1)
            write(fd_data, data, width*height*sizeof(unsigned short)+8);
        else
            write(fd_data, data, width*height*sizeof(unsigned int)+8);
    }
    FreeBmp((unsigned char*)data);
    close(fd_data);
    return 0; 
}
