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
static int picnum = 0;
int fd_c_dataadd_des;
int fd_h_des;
int fd_allbmpcheck;

void usage()
{
    printf("Usage: bmptoc [ <options> ] [ <BmpFileSrcPath> ]\n\n");
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
	*pBmpData = malloc(bmpinfor.biSizeImage);
	if(*pBmpData == NULL)
	{
		printf("BMP malloc Fail\n");
		close(fd);
		return -1;
	}
	lseek(fd, bmpheader.bfOffBits, SEEK_SET);
	if(read(fd, *pBmpData, bmpinfor.biSizeImage) < 0)
    {
        printf("read bmp data fail\n");
        close(fd);
        return -1;
    }
	close(fd);   	
	*width = bmpinfor.biWidth;
	*height = bmpinfor.biHeight;
	XYMirrorBmp(0, 1, *width, *height, *pBmpData);
	return bmpinfor.biSizeImage;
}

int FreeBmp(unsigned char* pBmpData)
{
	if(pBmpData != NULL)
		free(pBmpData);
	return 0;
}

void WriteStrToFile(int* fd, char* data)
{
    if(write(*fd, data, strlen(data)) < 0)
        printf("WriteStrToFile %s fail\n", data);
}

void WriteByteToFile(int* fd, int data)
{
    char tmp[256];
    sprintf(tmp, "0x%02x, ", data);
    if(write(*fd, tmp, strlen(tmp)) < 0)	
        printf("WriteByteToFile %d fail\n", data);
}

int WriteCFileProc(int* fdc, char* path, char* filename)
{
    int width,height;
    unsigned char* data;
    int length;
    char tmp[MAX_PATH];
    //char filename[MAX_PATH];
    int i;
    char* p;
    //printf("%s %s\n", path, pSrcFileName);
    length = GetBMP(path, &width, &height, (unsigned char**)(&data));
    if(length != -1)
    {
//        p = pSrcFileName;
//        while((*p == '.') || (*p == '/'))
//            p++;
//        strcpy(filename, p);
//        p = filename;
//        while(*p != 0){
//            if(*p == '.')
//                *p = '_';
//            p++;
//        }
//        while(1){
//            p = strchr(filename, '/');
//            if(p == NULL)
//                break;
//            *p = '_';
//        }
//        p = filename+strlen(filename)-strlen(".bmp");
//        *p = 0;
        sprintf(tmp, "unsigned char %s[] = {\n", filename);
        WriteStrToFile(fdc, tmp);
        WriteStrToFile(fdc, "// width,height---type:int\n");
        for(i=0;i<4;i++)
            WriteByteToFile(fdc, (width>>(i*8))&0xff);
        for(i=0;i<4;i++)
            WriteByteToFile(fdc, (height>>(i*8))&0xff);
        WriteStrToFile(fdc, "\n// bmp data");
        for(i=0;i<length;i++)
        {
            if(i%8 == 0)
                WriteStrToFile(fdc, "\n");
            WriteByteToFile(fdc, (*((unsigned char*)data+i))&0xff);
        }
        WriteStrToFile(fdc, "};\n");
        sprintf(tmp, "extern unsigned char %s[]; // %d\n", filename, picnum);
        WriteStrToFile(&fd_h_des, tmp);

        sprintf(tmp, "   %s, // %d\n", filename,picnum);
        WriteStrToFile(&fd_allbmpcheck, tmp);
        picnum++;
    }
    FreeBmp((unsigned char*)data);
	return 0;
}

int ReadDirProc(char* pPath, char * pSrcPathName)
{
    char name[MAX_PATH];
    char tmp[MAX_PATH];
    char filename[MAX_PATH];
    char tmp1[MAX_PATH];
    struct dirent *dp;
    struct stat s;
    DIR *dfd;
    char* p;
    char* pend;
    int fd_c_des;
    sprintf(tmp, "%s/%s/%s", pPath, OUT_PATH, pSrcPathName);
    if ((dfd = opendir(tmp)) == NULL) {
        fprintf(stderr, "bmptoc: can't open %s\n", pSrcPathName);
        return -1;
    }
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        if (strlen(pSrcPathName)+strlen(dp->d_name)+2 > MAX_PATH)
        {
            fprintf(stderr, "bmptoc: name %s %s too long\n", pSrcPathName, dp->d_name);
        }
        else
        {
            sprintf(tmp, "%s/%s/%s/%s", pPath, OUT_PATH, pSrcPathName, dp->d_name);
            sprintf(name, "%s/%s", pSrcPathName, dp->d_name);
            lstat(tmp,&s);
            if(S_ISDIR(s.st_mode))
            {
                printf("****** scan bmp in dir:%s ******\n",name);
                ReadDirProc(pPath, name);
            }
            else
            {
                p = dp->d_name+strlen(dp->d_name)-strlen(".bmp");
                if(strcasecmp(p, ".bmp") == 0)
                {
                    p = name;
                    while((*p == '.') || (*p == '/'))
                        p++;
                    pend = p+strlen(p)-strlen(".bmp");
                    *pend = 0;
                    strcpy(filename, p);
                    *pend = '.';
                    p = filename;
                    while(*p != 0){
                        if(*p == '.')
                            *p = '_';
                        p++;
                    }
                    while(1){
                        p = strchr(filename, '/');
                        if(p == NULL)
                            break;
                        *p = '_';
                    }
                    sprintf(tmp, "%s/%s/%s/%s.c", pPath, OUT_PATH, BMP_OUT_PATH, filename);
                    fd_c_des = open(tmp, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
                    if(fd_c_des < 0)
                    {
                        printf("can't open %s, pls check!\n", tmp);
                        return -1;
                    }
                    WriteStrToFile(&fd_c_des, "#include \"BmpDataArray.h\"\n\n");
                    sprintf(tmp, "%s/%s/%s", pPath, OUT_PATH, name);
                    WriteCFileProc(&fd_c_des, tmp, filename);
                    close(fd_c_des);
                }
            }
        }
    }
    closedir(dfd);
}


int BuildFilePath(char* pPath, char* pSrcPathName)
{
    char name[MAX_PATH];
    char tmp[MAX_PATH];
    char filename[MAX_PATH];
    char tmp1[MAX_PATH];
    struct dirent *dp;
    struct stat s;
    DIR *dfd;
    char* p;
    char* pend;
    int fd_c_des;
    //printf("1. %s, %s \n", pPath, pSrcPathName);
    sprintf(tmp, "%s/%s/%s", pPath, OUT_PATH, pSrcPathName);
    if ((dfd = opendir(tmp)) == NULL) {
        fprintf(stderr, "bmptoc: can't open %s\n", pSrcPathName);
        return -1;
    }
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        if (strlen(pSrcPathName)+strlen(dp->d_name)+2 > MAX_PATH)
        {
            fprintf(stderr, "bmptoc: name %s %s too long\n", pSrcPathName, dp->d_name);
        }
        else
        {
            sprintf(name, "%s/%s", pSrcPathName, dp->d_name);
            sprintf(tmp, "%s/%s/%s", OUT_PATH, pSrcPathName, dp->d_name);
            lstat(tmp,&s);
            if(S_ISDIR(s.st_mode))
            {
                printf("****** scan bmp in dir:%s ******\n",tmp);
                BuildFilePath(pPath, name);
            }
            else
            {
                p = dp->d_name+strlen(dp->d_name)-strlen(".data");
                if(strcasecmp(p, ".data") == 0)
                {
                    p = name;
                    while((*p == '.') || (*p == '/'))
                        p++;
                    strcpy(tmp1, p);
                    pend = p+strlen(p)-strlen(".data");
                    *pend = 0;
                    strcpy(filename, p);
                    *pend = '.';
                    p = filename;
                    while(*p != 0){
                        if(*p == '.')
                            *p = '_';
                        p++;
                    }
                    while(1){
                        p = strchr(filename, '/');
                        if(p == NULL)
                            break;
                        *p = '_';
                    }
                    //printf("3. %s %s\n", filename, tmp1);
                    sprintf(tmp, "unsigned char %s[] = { \"%s\" };\n", filename, tmp1);
                    WriteStrToFile(&fd_c_dataadd_des, tmp);
                }
            }
        }
    }
    closedir(dfd);
}

int main(int argc, char *argv[])
{
    char * psrcpathname;
    char * p;
    char * colorstr;
    char tmp[MAX_PATH];
    char path[MAX_PATH];

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
    picnum = 0;
    getcwd(path,MAX_PATH);
    sprintf(tmp, "%s/%s/%s/BmpDataArray.h", path, OUT_PATH, BMP_OUT_PATH);
	fd_h_des = open(tmp, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if(fd_h_des < 0)
	{
		printf("can't open %s, pls check!\n", tmp);
		return -1;
	}
    sprintf(tmp, "%s/%s/%s/AllBmpArrayCheck.h", path, OUT_PATH, BMP_OUT_PATH);
	fd_allbmpcheck = open(tmp, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if(fd_allbmpcheck < 0)
	{
		printf("can't open %s, pls check!\n", tmp);
		return -1;
	}
    sprintf(tmp, "%s/%s/BmpDataPath.c", path, OUT_PATH);
	fd_c_dataadd_des = open(tmp, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if(fd_c_dataadd_des < 0)
	{
		printf("can't open %s, pls check!\n", tmp);
		return -1;
	}
    WriteStrToFile(&fd_h_des, "#ifndef __BMPDATAARRAY_H\n");
    WriteStrToFile(&fd_h_des, "#define __BMPDATAARRAY_H\n");
    WriteStrToFile(&fd_h_des, "\n");
    WriteStrToFile(&fd_allbmpcheck, "#ifndef __ALLBMPCHECK_H\n");
    WriteStrToFile(&fd_allbmpcheck, "#define __ALLBMPCHECK_H\n");
    WriteStrToFile(&fd_allbmpcheck, "#include \"BmpDataArray.h\"\n\n");
    WriteStrToFile(&fd_allbmpcheck, "unsigned char* picall[] = {\n");
    WriteStrToFile(&fd_c_dataadd_des, "\n");
    ReadDirProc(path, psrcpathname);
    BuildFilePath(path, psrcpathname);
    WriteStrToFile(&fd_h_des, "#endif\n");
    WriteStrToFile(&fd_allbmpcheck, "};\n");
    WriteStrToFile(&fd_allbmpcheck, "#endif\n");
    close(fd_c_dataadd_des);
    close(fd_allbmpcheck);
    close(fd_h_des);
	return 0; 
}
