#include "bar.h"
#include "draw.h"

// Bar控件为进度条控件，可以使用LoadBarPic加载整张图片，也可以使用AssembleBarPic生成进度条的图片
// Bar控件BarValue为进度条的值，范围0~1000
Bar::Bar(int x, int y, int w, int h, int barMode):Image(x, y, w, h)
{
	BarNotSelbuff = NULL;
	BarSelbuff = NULL;
	BarOffsetX = 0;
	BarOffsetY = 0;
	BarWidth = 0;
	BarHeight = 0;
	BarIconOffsetX = 0;
	BarIconOffsetY = 0;
	BarIconWidth = 0;
	BarIconHeight = 0;
	BarIconbuff = NULL;
	BarOrient = barMode&BIT0;
	BarValue = 200;
	pWidget->pMyCtlObject = this;
}

Bar::~Bar(void)
{
	FreeBmp(BarNotSelbuff);
	FreeBmp(BarSelbuff);
	FreeBmp(BarIconbuff);
}

// pSelPic ---> 进度条高亮时显示的整张图片，pNotSelPic ---> 进度条不高亮时显示的整张图片
// pIconPic ---> 进度条Icon显示的整张图片
void Bar::LoadBarPic(unsigned char* pSelPic, unsigned char* pNotSelPic, unsigned char* pIconPic)
{
	int picID;
	int width,height;
	GetBMP_wid(pSelPic, &BarWidth, &BarHeight, &BarSelbuff, &picID);
	if(pNotSelPic != pSelPic)
	{
		GetBMP_wid(pNotSelPic, &width, &height, &BarNotSelbuff, &picID);
		if((BarWidth != width) || (BarHeight != height))
			ERROR_X("error! Bar should load same width/height for Sel/NotSel pic\n");
	}
	else
	{
		BarSelbuff = NULL;
	}
	if(pIconPic != NULL)
		GetBMP_wid(pIconPic, &BarIconWidth, &BarIconHeight, &BarIconbuff, &picID);
	if(BarOrient == MOVELEFTRIGHT)
	{
		if(BarWidth%2)
			ERROR_X("error! Bar Width should be even\n");
		if(BarIconWidth%2)
			ERROR_X("error! Bar Icon Width should be even\n");
	}
	else
	{
		if(BarHeight%2)
			ERROR_X("error! Bar Height should be even\n");
		if(BarIconHeight%2)
			ERROR_X("error! Bar Icon Height should be even\n");
	}
}

// 通过以下图片组装成完整的pSelPic、pNotSelPic、pIconPic，其中rate为pBarSelPic/pBarNotSelPic重复的次数
void Bar::AssembleBarPic(unsigned char* pLeftOrUpSelPic, unsigned char* pRightOrDownSelPic, unsigned char* pBarSelPic,
						 unsigned char* pLeftOrUpNotSelPic, unsigned char* pRightOrDownNotSelPic, unsigned char* pBarNotSelPic,
						 unsigned char* pIconPic, int rate)
{
	int picID;
	int LeftOrUpSelPicWidth,LeftOrUpSelPicHeight;
	int RightOrDownSelPicWidth,RightOrDownSelPicHeight;
	int BarSelPicWidth,BarSelPicHeight;
	int LeftOrUpNotSelPicWidth,LeftOrUpNotSelPicHeight;
	int RightOrDownNotSelPicWidth,RightOrDownNotSelPicHeight;
	int BarNotSelPicWidth,BarNotSelPicHeight;
	PIXEL *SelPicbuff,*NotSelPicbuff;
	int i,width,height;

	LeftOrUpSelPicWidth = *(unsigned int*)pLeftOrUpSelPic;
	LeftOrUpSelPicHeight = *((unsigned int*)pLeftOrUpSelPic+1);
	RightOrDownSelPicWidth = *(unsigned int*)pRightOrDownSelPic;
	RightOrDownSelPicHeight = *((unsigned int*)pRightOrDownSelPic+1);
	BarSelPicWidth = *(unsigned int*)pBarSelPic;
	BarSelPicHeight = *((unsigned int*)pBarSelPic+1);
	LeftOrUpNotSelPicWidth = *(unsigned int*)pLeftOrUpNotSelPic;
	LeftOrUpNotSelPicHeight = *((unsigned int*)pLeftOrUpNotSelPic+1);
	RightOrDownNotSelPicWidth = *(unsigned int*)pRightOrDownNotSelPic;
	RightOrDownNotSelPicHeight = *((unsigned int*)pRightOrDownNotSelPic+1);
	BarNotSelPicWidth = *(unsigned int*)pBarNotSelPic;
	BarNotSelPicHeight = *((unsigned int*)pBarNotSelPic+1);

	if(BarOrient == MOVELEFTRIGHT)
	{
		if((LeftOrUpSelPicHeight != RightOrDownSelPicHeight) || (LeftOrUpSelPicHeight != BarSelPicHeight))
			ERROR_X("error! Bar SelPic Height should be same\n");
		if((LeftOrUpNotSelPicHeight != RightOrDownNotSelPicHeight) || (LeftOrUpNotSelPicHeight != BarNotSelPicHeight))
			ERROR_X("error! Bar NotSelPic Height should be same\n");
		width = LeftOrUpSelPicWidth+BarSelPicWidth*rate+RightOrDownSelPicWidth;
		height = LeftOrUpSelPicHeight;

		SelPicbuff = (PIXEL*)uimalloc(width*height*EVERY_PIXEL_BYTES+8);
		*(unsigned int*)SelPicbuff = width;
		*((unsigned int*)SelPicbuff+1) = height;		
		AreaDraw(SelPicbuff, 0, 0, (PIXEL*)pLeftOrUpSelPic, 0, 0, 
			     LeftOrUpSelPicWidth, LeftOrUpSelPicHeight, REPLACE_STYLE);
		for(i=0;i<rate;i++)
			AreaDraw(SelPicbuff, LeftOrUpSelPicWidth+i*BarSelPicWidth, 0, (PIXEL*)pBarSelPic, 0, 0, 
				     BarSelPicWidth, BarSelPicHeight, REPLACE_STYLE);
		AreaDraw(SelPicbuff, LeftOrUpSelPicWidth+rate*BarSelPicWidth, 0, (PIXEL*)pRightOrDownSelPic, 0, 0, 
			     RightOrDownSelPicWidth, RightOrDownSelPicHeight, REPLACE_STYLE);

		NotSelPicbuff = (PIXEL*)uimalloc(width*height*EVERY_PIXEL_BYTES+8);
		*(unsigned int*)NotSelPicbuff = width;
		*((unsigned int*)NotSelPicbuff+1) = height;		
		AreaDraw(NotSelPicbuff, 0, 0, (PIXEL*)pLeftOrUpNotSelPic, 0, 0, 
			     LeftOrUpNotSelPicWidth, LeftOrUpNotSelPicHeight, REPLACE_STYLE);
		for(i=0;i<rate;i++)
			AreaDraw(NotSelPicbuff, LeftOrUpNotSelPicWidth+i*BarNotSelPicWidth, 0, (PIXEL*)pBarNotSelPic, 0, 0, 
				     BarNotSelPicWidth, BarNotSelPicHeight, REPLACE_STYLE);
		AreaDraw(NotSelPicbuff, LeftOrUpNotSelPicWidth+rate*BarNotSelPicWidth, 0, (PIXEL*)pRightOrDownNotSelPic, 0, 0, 
			     RightOrDownNotSelPicWidth, RightOrDownNotSelPicHeight, REPLACE_STYLE);
	}
	else
	{
		if((LeftOrUpSelPicWidth != RightOrDownSelPicWidth) || (LeftOrUpSelPicWidth != BarSelPicWidth))
			ERROR_X("error! Bar SelPic Width should be same\n");
		if((LeftOrUpNotSelPicWidth != RightOrDownNotSelPicWidth) || (LeftOrUpNotSelPicWidth != BarNotSelPicWidth))
			ERROR_X("error! Bar NotSelPic Width should be same\n");
		width = LeftOrUpSelPicWidth;
		height = LeftOrUpSelPicHeight+BarSelPicHeight*rate+RightOrDownSelPicHeight;

		SelPicbuff = (PIXEL*)uimalloc(width*height*EVERY_PIXEL_BYTES+8);
		*(unsigned int*)SelPicbuff = width;
		*((unsigned int*)SelPicbuff+1) = height;		
		AreaDraw(SelPicbuff, 0, 0, (PIXEL*)pLeftOrUpSelPic, 0, 0, 
			     LeftOrUpSelPicWidth, LeftOrUpSelPicHeight, REPLACE_STYLE);
		for(i=0;i<rate;i++)
			AreaDraw(SelPicbuff, 0, LeftOrUpSelPicHeight+i*BarSelPicHeight, (PIXEL*)pBarSelPic, 0, 0, 
				     BarSelPicWidth, BarSelPicHeight, REPLACE_STYLE);
		AreaDraw(SelPicbuff, 0, LeftOrUpSelPicHeight+rate*BarSelPicHeight, (PIXEL*)pRightOrDownSelPic, 0, 0, 
			     RightOrDownSelPicWidth, RightOrDownSelPicHeight, REPLACE_STYLE);

		NotSelPicbuff = (PIXEL*)uimalloc(width*height*EVERY_PIXEL_BYTES+8);
		*(unsigned int*)NotSelPicbuff = width;
		*((unsigned int*)NotSelPicbuff+1) = height;		
		AreaDraw(NotSelPicbuff, 0, 0, (PIXEL*)pLeftOrUpNotSelPic, 0, 0, 
			     LeftOrUpNotSelPicWidth, LeftOrUpNotSelPicHeight, REPLACE_STYLE);
		for(i=0;i<rate;i++)
			AreaDraw(NotSelPicbuff, 0, LeftOrUpNotSelPicHeight+i*BarNotSelPicHeight, (PIXEL*)pBarNotSelPic, 0, 0, 
				     BarNotSelPicWidth, BarNotSelPicHeight, REPLACE_STYLE);
		AreaDraw(NotSelPicbuff, 0, LeftOrUpNotSelPicHeight+rate*BarNotSelPicHeight, (PIXEL*)pRightOrDownNotSelPic, 0, 0, 
			     RightOrDownNotSelPicWidth, RightOrDownNotSelPicHeight, REPLACE_STYLE);
	}
	LoadBarPic((unsigned char*)SelPicbuff, (unsigned char*)NotSelPicbuff, pIconPic);
}

// 配置进度条的显示offset值
void Bar::SetBarOffset(int offsetx, int offsety, int iconoffsetx, int iconoffsety)
{
	BarOffsetX = offsetx;
	BarOffsetY = offsety;
	BarIconOffsetX = iconoffsetx;
	BarIconOffsetY = iconoffsety;
	if(BarOrient == MOVELEFTRIGHT)
	{
		if(BarOffsetX != BarIconOffsetX)
			ERROR_X("error! Bar offsetx should be same with iconoffsetx\n");
	}
	else
	{
		if(BarOffsetY != BarIconOffsetY)
			ERROR_X("error! Bar offsety should be same with iconoffsety\n");
	}
}

// 进度条的值，0~1000变化
int Bar::GetBarValue(void)
{
	return BarValue;
}

int Bar::CtlFocus(int focus)
{
	return 0;
}

int Bar::CtlEvent(Event* pEvent)
{
	Any_Event *pany_event = NULL;
	pany_event = &(pEvent->uEvent.any_event);	
	int tmp;
	if(isTSMsg(pany_event->event_type))
	{
		if(BarOrient == MOVELEFTRIGHT)
		{
			tmp = pWidget->x+BgimageOffsetX+BarOffsetX+BarIconWidth/2;
			if(pEvent->uEvent.ts_event.x < tmp)
				BarValue = 0;
			else if(pEvent->uEvent.ts_event.x > (tmp+BarWidth-BarIconWidth))
				BarValue = 1000;
			else
				BarValue = (pEvent->uEvent.ts_event.x-tmp)*1000/(BarWidth-BarIconWidth);
		}
		else
		{
			tmp = pWidget->y+BgimageOffsetY+BarOffsetY+BarIconHeight/2;
			if(pEvent->uEvent.ts_event.y < tmp)
				BarValue = 0;
			else if(pEvent->uEvent.ts_event.y > (tmp+BarHeight-BarIconHeight))
				BarValue = 1000;
			else
				BarValue = (pEvent->uEvent.ts_event.y-tmp)*1000/(BarHeight-BarIconHeight);
		}
		//printf("bar BarValue:%d\n", BarValue);
		CtlRefresh();
	}
	return 0;
}

int Bar::CtlShow(PIXEL* pDesBuf)
{
	int position;
	int tmpx,tmpy;
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, 
				 Bgimagebuff, 0, 0,
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), BltMode);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	

	tmpx = pWidget->x+BgimageOffsetX+BarOffsetX;
	tmpy = pWidget->y+BgimageOffsetY+BarOffsetY;
	if(BarNotSelbuff != NULL)
		AreaDraw(pDesBuf, tmpx, tmpy, BarNotSelbuff, 0, 0, BarWidth, BarHeight, BltMode);
	if(BarOrient == MOVELEFTRIGHT)
	{
		position = BarValue*(BarWidth-BarIconWidth)/1000;
		if((BarSelbuff != NULL) && (position != 0))
			AreaDraw(pDesBuf, tmpx, tmpy, BarSelbuff, 0, 0, position+BarIconWidth/2, BarHeight, BltMode);
		tmpx = pWidget->x+BgimageOffsetX+BarIconOffsetX;
		tmpy = pWidget->y+BgimageOffsetY+BarIconOffsetY;
		if(BarIconbuff != NULL)
			AreaDraw(pDesBuf, tmpx+position, tmpy, BarIconbuff, 0, 0, BarIconWidth, BarIconHeight, ALPHA_STYLE);
	}
	else
	{
		position = BarValue*(BarHeight-BarIconHeight)/1000;
		if((BarSelbuff != NULL) && (position != 0))
			AreaDraw(pDesBuf, tmpx, tmpy, BarSelbuff, 0, 0, BarWidth, position+BarIconHeight/2, BltMode);
		tmpx = pWidget->x+BgimageOffsetX+BarIconOffsetX;
		tmpy = pWidget->y+BgimageOffsetY+BarIconOffsetY;
		if(BarIconbuff != NULL)
			AreaDraw(pDesBuf, tmpx, tmpy+position, BarIconbuff, 0, 0, BarIconWidth, BarIconHeight, ALPHA_STYLE);
	}
	return 0;
}

