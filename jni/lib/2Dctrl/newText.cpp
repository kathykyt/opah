#include "newText.h"
#include "draw.h"

NewText::NewText(int x, int y, int w, int h, int Boundary, int Set[], int length):Image(x, y, w, h)
{
	pTextPic = MulImageInit();
	TextSet[0] = 0;
	int count = 0;
	for(; count < length; count++) {
		TextSet[count] = Set[count];
	}
	TextSet[length] = 0;
	TextBoundary = Boundary;
	pWidget->pMyCtlObject = this;
}

NewText::~NewText(void)
{
	MulImageDestory(pTextPic);
}

int NewText::SetTextStr(int Set[], int length)
{
	int count = 0;
	for(; count < length; count++) {
		TextSet[count] = Set[count];
	}
	TextSet[length] = 0;
	CtlRefresh();
	return 0;
}

int NewText::SetTextNone()
{
	TextSet[0] = 0;
	CtlRefresh();
	return 0;
}

void NewText::SetTextX(int x)
{
	pWidget->x = x;
}

//int[] NewText::GetTextStr(void)
//{
//	return TextSet;
//}

int NewText::CtlFocus(int focus)
{
	return 0;
}

int NewText::CtlEvent(Event* pEvent)
{
	return 0;
}

int NewText::CtlShow(PIXEL* pDesBuf)
{
	//const char* p;
	int p;
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

	if(TextSet[0] != 0)
	{
		offset = 0;
		//p = TextSet;
		int count = 0;
		//while(count < sizeof(TextSet)/sizeof(TextSet[0]))
		while(TextSet[count] != 0)
		{
			pPic = GetMulImageIndex(pTextPic, TextSet[count]);	
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
					count++;
				}
			}
			else
			{
				ERROR_X("error! Text char:%d not found!\n", TextSet[count]);
			}
		}
	}
	return 0;
}

