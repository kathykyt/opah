#include "mulimage.h"
#include "draw.h"

// MulImage不应为控件，可以使用MulImage结构体对多幅图片进行管理
MulImage* MulImageInit(void)
{
	MulImage* pMulImage;
	pMulImage = (MulImage*)uimalloc(sizeof(MulImage));
	pMulImage->MulImageCount = 0;
	pMulImage->NextMulImagePic = NULL;
	return pMulImage;
}

// 根据多幅图片形成的链表来摧毁并释放内存
int MulImageDestory(MulImage* pMulImage)
{
	MulImagePic* pMulImagePic;
	if(pMulImage == NULL)
		return 0;
	while(pMulImage->NextMulImagePic != NULL)
	{
		pMulImagePic = pMulImage->NextMulImagePic;
		while(pMulImagePic->NextMulImagePic != NULL)
			pMulImagePic = pMulImagePic->NextMulImagePic;
		FreeBmp(pMulImagePic->MulImagebuff);
		uifree((void*)pMulImagePic);
	}
	pMulImage->MulImageCount = 0;
	uifree((void*)pMulImage);
	return 0;
}

// load多幅图片到内存中
void MulImageLoadPic(MulImage* pMulImage, unsigned char* pPic, int offsetx, int offsety, int imageIndex)
{
	int width,height;
	int picID;
	MulImagePic* pMulImagePic;
	if(pMulImage->NextMulImagePic == NULL)
	{
		pMulImage->NextMulImagePic = (MulImagePic*)uimalloc(sizeof(MulImagePic));
		pMulImagePic = pMulImage->NextMulImagePic;
	}
	else
	{
		pMulImagePic = pMulImage->NextMulImagePic;
		while(pMulImagePic->NextMulImagePic != NULL)
			pMulImagePic = pMulImagePic->NextMulImagePic;		
		pMulImagePic->NextMulImagePic = (MulImagePic*)uimalloc(sizeof(MulImagePic));
		pMulImagePic = pMulImagePic->NextMulImagePic;
	}
	pMulImagePic->MulImagebuff = NULL;
	pMulImagePic->NextMulImagePic = NULL;
	pMulImagePic->MulImageOffsetX = offsetx;
	pMulImagePic->MulImageOffsetY = offsety;
	pMulImagePic->MulImageIndex = imageIndex;
	GetBMP_wid(pPic, &(pMulImagePic->MulImageWidth), &(pMulImagePic->MulImageHeight), &(pMulImagePic->MulImagebuff), &picID);
	pMulImage->MulImageCount++;
}

// 如果全部一样的图片可以不加载
void MulImageLoadSamePic(MulImage** pDesMulImage, MulImage** pSrcMulImage)
{
	MulImageDestory(*pDesMulImage);
	*pDesMulImage = *pSrcMulImage;
}

int GetMulImageCount(MulImage* pMulImage)
{
	return pMulImage->MulImageCount;
}

// 根据index来查找图片对应的指针
MulImagePic* GetMulImageIndex(MulImage* pMulImage, int index)
{
	MulImagePic* pMulImagePic;
	pMulImagePic = pMulImage->NextMulImagePic;
	while(pMulImagePic != NULL)
	{
		if(pMulImagePic->MulImageIndex == index)
			return pMulImagePic;
		pMulImagePic = pMulImagePic->NextMulImagePic;
	}
	ERROR_X("error! GetMulImageIndex can't find index:0x%x image\n", index);
	return NULL;
}

