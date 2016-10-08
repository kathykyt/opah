#include "text.h"
#include "draw.h"

// text�ؼ����ض��ͼƬ�Ͷ�Ӧ��index������pStr�ж�Ӧ��index��ʾ����Ļ�ϣ�û�а���������
// 1��text�ؼ�ֻ����ʾһ���ַ��������text���Զ������ؼ�����ʾ�����interface1��demo����
Text::Text(int x, int y, int w, int h, int Boundary, const char* pStr):Image(x, y, w, h)
{	
	pTextPic = MulImageInit();
	pTextStr = NULL;
	if(pStr != NULL)
	{
//		pTextStr = (const char*)uimalloc(strlen(pStr));
		pTextStr = (char*)uimalloc(strlen(pStr) + 1);
//printf("----- %s \r\n",pStr);
//memcpy(pTextStr,pStr,strlen(pStr));
		strcpy((char*)pTextStr, pStr);
		//*(pTextStr + strlen(pStr) + 1) = 0;
	}
	TextBoundary = Boundary;
	pWidget->pMyCtlObject = this;
}

Text::~Text(void)
{
	if(pTextStr != NULL)
		uifree((void*)pTextStr);
	MulImageDestory(pTextPic);
}

// ����text�ؼ���Ҫ��ʾ���ַ������ؼ�������pStr�ж�Ӧ��index��ʾ����Ļ�ϣ��ַ�������û������
int Text::SetTextStr(const char* pStr)
{
	if(pTextStr != NULL)
		uifree((void*)pTextStr);
	//daniel
	pTextStr = (char*)uimalloc(strlen(pStr) + 1);
	strcpy((char*)pTextStr, pStr);
	*(pTextStr + strlen(pStr) + 1) = 0;
	//daniel ends
//	pTextStr = (char*)uimalloc(strlen(pStr));
//	strcpy((char*)pTextStr, pStr);
	CtlRefresh();
	return 0;
}

// ����text�ؼ���ʾ���ַ���
const char* Text::GetTextStr(void)
{
	return pTextStr;
}

void Text::SetTextX(int x)
{
	pWidget->x = x;
}

int Text::CtlFocus(int focus)
{
	return 0;
}

int Text::CtlEvent(Event* pEvent)
{
	return 0;
}

int Text::CtlShow(PIXEL* pDesBuf)
{
	//const char* p;
	volatile char *p;
	int offset;
	MulImagePic* pPic;
			
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, Bgimagebuff, 0, 0,
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), BltMode);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	

	if(pTextStr != NULL)
	{
		offset = 0;
		p = pTextStr;
		while(*p != 0)
		{
			pPic = GetMulImageIndex(pTextPic, (int)*p);	
			if(pPic != NULL)
			{
				if(pPic->MulImagebuff != NULL)
				{	
					
					
					if(offset < pWidget->width-(BgimageOffsetX+pPic->MulImageOffsetX))
					{
						AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pPic->MulImageOffsetX+offset, pWidget->y+BgimageOffsetY+pPic->MulImageOffsetY, pPic->MulImagebuff, 0, 0,
								 DIFF(pPic->MulImageWidth,BgimageOffsetX+pPic->MulImageOffsetX,pWidget->width), DIFF(pPic->MulImageHeight,BgimageOffsetY+pPic->MulImageOffsetY,pWidget->height), BltMode);
						offset = offset+DIFF(pPic->MulImageWidth,BgimageOffsetX+pPic->MulImageOffsetX,pWidget->width)+TextBoundary;
					}
					p++;
				}
			}
			else
			{
				ERROR_X("error! Text char:%c not found!\n", *p);
			}
		}
	}
	return 0;
}

