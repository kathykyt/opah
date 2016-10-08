#include "gif.h"
#include "draw.h"

// gif控件每隔gifinterval毫秒，定时刷新图片
Gif::Gif(int x, int y, int w, int h, int gifinterval):Image(x, y, w, h)
{
	pGifPic = MulImageInit();
	GifTimerID = -1;
	GifStartNum = 0;
	GifEndNum = 0;
	GifShowNum = 0;
	GifStartStop = GIFSTOP;
	GifInterval = gifinterval;
	pWidget->pMyCtlObject = this;
}

Gif::~Gif(void)
{
	TimerDel(&GifTimerID);
	MulImageDestory(pGifPic);
}

// gif控件的callback函数
static void GifTimerCallback(void* pctl)
{
	((CtlObject*)pctl)->CtlRefresh();
}

// gif控件配置从第几张图片开始显示
void Gif::SetGifStartEndNum(int start, int end)
{
	if(start < end)
	{
		if(start < GetMulImageCount(pGifPic))
			GifStartNum = start;
		else
			ERROR_X("error! GifStartNum out of size\n");
		if(end < GetMulImageCount(pGifPic))
			GifEndNum = end;
		else
			ERROR_X("error! GifEndNum out of size\n");
		GifShowNum = GifStartNum;
	}
}

// gif控件为焦点后配置定时器
int Gif::CtlFocus(int focus)
{
	if(focus)
	{
		GifStartStop = GIFSTART;
		GifTimerID = AddTimerAction(GifInterval, 0, GifTimerCallback, (void*)this);
	}
	else
	{
		GifStartStop = GIFSTOP;
		TimerDel(&GifTimerID);
	}
	return 0;
}

int Gif::CtlEvent(Event* pEvent)
{
	return 0;
}

int Gif::CtlShow(PIXEL* pDesBuf)
{
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

	pPic = GetMulImageIndex(pGifPic, GifShowNum);
	if((pPic != NULL) && (pPic->MulImagebuff != NULL))
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pPic->MulImageOffsetX, pWidget->y+BgimageOffsetY+pPic->MulImageOffsetY, pPic->MulImagebuff, 0, 0,
				 DIFF(pPic->MulImageWidth,BgimageOffsetX+pPic->MulImageOffsetX,pWidget->width), DIFF(pPic->MulImageHeight,BgimageOffsetY+pPic->MulImageOffsetY,pWidget->height), BltMode);
	}
	GifShowNum++;
	if(GifShowNum > GifEndNum)
		GifShowNum = GifStartNum;
	return 0;
}
