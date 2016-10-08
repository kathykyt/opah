#include "mulimage.h"
#include "draw.h"

// MulImage��ӦΪ�ؼ�������ʹ��MulImage�ṹ��Զ��ͼƬ���й���
MulImage* MulImageInit(void)
{
	MulImage* pMulImage;
	pMulImage = (MulImage*)uimalloc(sizeof(MulImage));
	pMulImage->MulImageCount = 0;
	pMulImage->NextMulImagePic = NULL;
	return pMulImage;
}

// ���ݶ��ͼƬ�γɵ��������ݻٲ��ͷ��ڴ�
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

// load���ͼƬ���ڴ���
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

// ���ȫ��һ����ͼƬ���Բ�����
void MulImageLoadSamePic(MulImage** pDesMulImage, MulImage** pSrcMulImage)
{
	MulImageDestory(*pDesMulImage);
	*pDesMulImage = *pSrcMulImage;
}

int GetMulImageCount(MulImage* pMulImage)
{
	return pMulImage->MulImageCount;
}

// ����index������ͼƬ��Ӧ��ָ��
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

