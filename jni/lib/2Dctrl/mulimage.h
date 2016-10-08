#ifndef _MULIMAGE_H_
#define _MULIMAGE_H_

#include "image.h"

typedef struct _MulImagePic MulImagePic;
typedef struct _MulImage MulImage;

struct _MulImagePic
{
	int MulImageOffsetX;
	int MulImageOffsetY;
	PIXEL* MulImagebuff;
	int MulImageWidth;
	int MulImageHeight;
	int MulImageIndex;		//	Í¼Æ¬µÄË÷Òý
	MulImagePic* NextMulImagePic;
};

struct _MulImage
{
	int MulImageCount;
	MulImagePic* NextMulImagePic;
};

MulImage* MulImageInit(void);
int MulImageDestory(MulImage* pMulImage);
void MulImageLoadPic(MulImage* pMulImage, unsigned char* pPic, int offsetx, int offsety, int imageIndex);
void MulImageLoadSamePic(MulImage** pDesMulImage, MulImage** pSrcMulImage);
int GetMulImageCount(MulImage* pMulImage);
MulImagePic* GetMulImageIndex(MulImage* pMulImage, int index);

#endif
