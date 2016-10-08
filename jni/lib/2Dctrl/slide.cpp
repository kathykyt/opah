#include "slide.h"
#include "draw.h"

static void SlideTimerCallback(void* pctl);

// slide控件加载多幅图片控件，对图片进行滑动处理
// slide加载的图片子控件可以包括类似button...等孙子控件，方便灵活扩展
// Interval:slide控件间隔多少ms自行滑动到下一张图片，若为0则不定时滑动，最小值为100ms
// Rate:触屏Up后滑动的快慢。数值越大滑动越快。最小值为2
// Boundary:子控件间隔的像素点，中间用背景色填充，
// slideMode:  方向-->MOVELEFTRIGHT/MOVEUPDOWN  
//             循环-->MOVECYCLE  
//             在MOVECYCLE=0时，滑动到边缘时显示类似弹簧的效果-->MOVESPRING
Slide::Slide(int x, int y, int w, int h, int Interval, BYTE Rate, int Boundary, int slideMode):Image(x, y, w, h)
{
	memset(&prets, 0, sizeof(Touch_Event));
	prets_diff = 0;
	ts_diff_dec_count = 0;
	pSlide = NULL;
	memset(&SonCtlData, 0, sizeof(SlideSonCtlData));
	SonCtlData.SlideSonCtlTimerID = -1;
	AutoSlideTimerID = -1;
	SlideTimerID = -1;
	SlideCurrent = 0;
	SlideSpeed = 0;
	SlideDes = 0;
	SlideInterval = Interval;
	if((SlideInterval != 0) && (SlideInterval < 100))
		SlideInterval = 100;
	SlideBoundary = Boundary;
	SlideOrient = slideMode&BIT0;
	SlideCycle = (slideMode&BIT1)>>1;
	SlideSpring = (slideMode&BIT2)>>2;
	if(SlideOrient == MOVELEFTRIGHT)
		SlideRate = Rate*w/LCDW;
	else
		SlideRate = Rate*h/LCDH;
	if(SlideRate <= 2)
		SlideRate = 2;
	SlideLength = 0;
	SlideMaxIndex = 0;
	pWidget->pMyCtlObject = this;
}

Slide::~Slide(void)
{
	SlideItem* pItem;
	SlideItem* pPreItem;
	if(pSlide != NULL)
	{
		TimerDel(&(SonCtlData.SlideSonCtlTimerID));
		TimerDel(&AutoSlideTimerID);
		TimerDel(&SlideTimerID);
		while(pSlide->NextSlideItem != NULL)
		{
			pItem = pSlide;
			pPreItem = pSlide;
			while(pItem->NextSlideItem != NULL)
			{
				pPreItem = pItem;
				pItem = pItem->NextSlideItem;
			}
			delete(pItem->pSlideItemImage);
			uifree((void*)pItem);
			pPreItem->NextSlideItem = NULL;
		}
		uifree((void*)pSlide);
	}
}

// slide控件循环时计算坐标是否超出边界
static int CycleCalc(int cyclemode, int currentnum, int length)
{
	if(cyclemode)
	{
		if(currentnum < 0)
			return currentnum-(currentnum/length)*length+length;
		else if(currentnum >= length)
			return currentnum-(currentnum/length)*length;
	}
	return currentnum;
}

// slide控件中某一子项控件中的子控件，即孙子控件的显示
static PIXEL* SlideSonCtlShow(SlideItem* pItem, Widget* pSlideWidget)
{
	PIXEL* pTemp;
	pTemp = CreateArea(pSlideWidget->width, pSlideWidget->height);
	AreaDraw(pTemp, 0, 0, pItem->pSlideItemImage->Bgimagebuff, 0, 0,
	         pSlideWidget->width, pSlideWidget->height, REPLACE_STYLE);
	pSlideWidget->pSonWidget->WidgetSetFocus(1);
	pSlideWidget->pSonWidget->WidgetSetDirty(1);
	pSlideWidget->pSonWidget->WidgetOnDraw(NULL, pTemp);
	return pTemp;
}

// slide控件滑动效果定时器启动函数
static int SlideStartTimer(void* pctl)
{
	int TimerID;
#if (LCDW>=800)			//	根据实际分辨率和效果调整
	TimerID = AddTimerAction(30, 0, SlideTimerCallback, pctl);
#else
	TimerID = AddTimerAction(10, 0, SlideTimerCallback, pctl);
#endif
	return TimerID;
}

// slide控件的自动滑动定时器callback函数
static void SlideAutoTimerCallback(void* pctl)
{
        printf("SlideAutoTimerCallback\n");

	Slide* pCtl;
	int i;
	pCtl = (Slide*)pctl;
	i = pCtl->GetSlideShowItem();
	if(i < pCtl->SlideMaxIndex-1)
	{
		if(pCtl->SlideOrient == MOVELEFTRIGHT)
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(i+1)->xshowstart;
		else
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(i+1)->yshowstart;
		pCtl->SlideTimerID = SlideStartTimer(pctl);
		((CtlObject*)pCtl)->CtlRefresh();
	}
	else if(pCtl->SlideCycle == 0)
	{
		TimerDel(&(pCtl->AutoSlideTimerID));
	}
	else
	{
		if(pCtl->SlideOrient == MOVELEFTRIGHT)
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(0)->xshowstart;
		else
			pCtl->SlideDes = pCtl->GetSlideItemFromIndex(0)->yshowstart;
		pCtl->SlideTimerID = SlideStartTimer(pctl);
		((CtlObject*)pCtl)->CtlRefresh();
	}
}

// slide控件的孙子控件ts down时启动的定时器callback函数
static void SlideSonCtlTimerCallback(void* pctl)
{

	printf("SlideSonCtlTimerCallback\n");

	Slide* pCtl;

	printf("SlideSonCtlTimerCallback\n");

	pCtl = (Slide*)pctl;
	TimerDel(&(pCtl->SonCtlData.SlideSonCtlTimerID));
	pCtl->SonCtlData.pFocusWidget->pMyCtlObject->CtlEvent((Event*)(&(pCtl->SonCtlData.downevent)));
	if(pCtl->SonCtlData.pFocusWidget->pCallback != NULL)
		pCtl->SonCtlData.pFocusWidget->pCallback((Event*)(&(pCtl->SonCtlData.downevent)));	
	((CtlObject*)pCtl)->CtlRefresh();
	
	printf("SlideSonCtlTimerCallback 2\n");

}

// slide控件定时器callback函数
static void SlideTimerCallback(void* pctl)
{
	int i;
	int step;
	int slideeffect = 2;		//	滑动快慢效果调节
	int steparray[256];
	Slide* pCtl;
	pCtl = (Slide*)pctl;
	if((pCtl->SlideDes == pCtl->SlideCurrent) || (pCtl->SlideDes == pCtl->SlideCurrent-pCtl->SlideLength))
	{
		if(TimerDel(&(pCtl->SlideTimerID)) == 0)
			pCtl->SlideCurrent = pCtl->SlideDes;
		if((pCtl->AutoSlideTimerID == -1) && (pCtl->SlideInterval > 0))
		{
			if((pCtl->SlideCycle != 0) || (pCtl->GetSlideShowItem() < pCtl->SlideMaxIndex-1))
				pCtl->AutoSlideTimerID = AddTimerAction(pCtl->SlideInterval, 0, SlideAutoTimerCallback, pctl);
		}
		return;
	}
	memset(steparray, 0, sizeof(steparray)/sizeof(int));
	for(i=pCtl->SlideRate;i>0;i--)
		steparray[pCtl->SlideRate-i] = i;
	if((pCtl->SlideDes == 0) && (pCtl->SlideCurrent > pCtl->SlideLength/2))
		step = GETABS(pCtl->SlideDes+pCtl->SlideLength-pCtl->SlideCurrent);
	else
		step = GETABS(pCtl->SlideDes-pCtl->SlideCurrent);
	for(i=1;i<pCtl->SlideRate+1;i++)
	{
		if(step >= steparray[i]*slideeffect)
		{
			i--;
			if((pCtl->SlideDes == 0) && (pCtl->SlideCurrent > pCtl->SlideLength/2))
				step = steparray[i];
			else if(pCtl->SlideDes >= pCtl->SlideCurrent)
				step = steparray[i];
			else
				step = 0-steparray[i];
			break;
		}
	}
	//printf("SlideLength:%d, SlideDes:%d, SlideCurrent:%d, diff:%d, SlideSpeed:%d, step:%d\n", pCtl->SlideLength, pCtl->SlideDes, pCtl->SlideCurrent, pCtl->SlideDes-pCtl->SlideCurrent, pCtl->SlideSpeed, step);
	pCtl->SlideCurrent = pCtl->SlideCurrent+step;
	((CtlObject*)pCtl)->CtlRefresh();
}

// slide控件加载子图片控件，只需要指定对应的图片即可
// 想象有一块虚拟的连续内存，保存有slide控件的多个子图片控件的图像，子图片控件在内存的位置为该控件的x、y、width、height
// 每个图片在slide显示出来，对应内存中显示的位置为xshowstart、yshowstart、widthshow、heightshow
// --------
// |     B| A-------
// | (1)  |  | (2) |  
// |      |  -------
// --------
// 例如显示窗口(2),窗口(2)在虚拟内存位置A为Image控件的x/y/width/height。
// 但窗口(2)需要显示居中，所以应从B点显示窗口(2)，将窗口(1)的一部分也显示了。B的位置为xshowstart、yshowstart、widthshow、heightshow
// 将各种位置先计算出来可以加速显示，简化计算
int Slide::AddSlideItem(unsigned char* pPic)
{
	Image* pImage;
	SlideItem* pItem;
	int length;
	int width,height;
	int offset;
	width = *(unsigned int*)pPic;
	height = *((unsigned int*)pPic+1);
	if((width > pWidget->width) || (height > pWidget->height))
	{
		ERROR_X("error! SlideItem Width/Height exceed Slide Width/Height\n");
		return -1;
	}
	offset = 0;
	if(pSlide != NULL)
		offset = SlideBoundary;
	if((SlideCycle != 0) && (SlideLength > 0))
		SlideLength	= SlideLength-SlideBoundary;
	if(SlideOrient == MOVELEFTRIGHT)
		pImage = new Image(SlideLength+offset, (pWidget->height-height)/2, width, height);
	else
		pImage = new Image((pWidget->width-width)/2, SlideLength+offset, width, height);
	pImage->LoadImagePic(pPic);
	if(pSlide == NULL)
	{
		pSlide = (SlideItem*)uimalloc(sizeof(SlideItem));
		pSlide->SlideIndex = 0;
		pSlide->pSlideItemImage = pImage;
		pSlide->PreSlideItem = NULL;
		pSlide->NextSlideItem = NULL;
		pItem = pSlide;
	}
	else
	{
		pItem = pSlide;
		while(pItem->NextSlideItem != NULL)
			pItem = pItem->NextSlideItem;
		pItem->NextSlideItem = (SlideItem*)uimalloc(sizeof(SlideItem));
		pItem = pItem->NextSlideItem;
		pItem->SlideIndex = SlideMaxIndex;
		pItem->pSlideItemImage = pImage;
		pItem->PreSlideItem = pItem;
		pItem->NextSlideItem = NULL;
	}
	if(SlideOrient == MOVELEFTRIGHT)
	{
		pItem->xshowstart = SlideLength+offset+width/2-pWidget->width/2;
		pItem->yshowstart = 0;
		length = width+offset;
	}
	else
	{
		pItem->xshowstart = 0;
		pItem->yshowstart = SlideLength+offset+height/2-pWidget->height/2;
		length = height+offset;
	}
	pItem->widthshow = pWidget->width;
	pItem->heightshow = pWidget->height;
	SlideLength = SlideLength+length;
	if(SlideCycle != 0)
		SlideLength	= SlideLength+SlideBoundary;
	SlideMaxIndex++;
	//printf("%d --->x:%d,y:%d,w:%d,h:%d\n", SlideMaxIndex, pItem->xshowstart, pItem->yshowstart, pItem->widthshow, pItem->heightshow);
	return 0;
}

// 取slide控件index对应的item项
SlideItem* Slide::GetSlideItemFromIndex(int ItemIndex)
{
	SlideItem* pItem;
	int i;
	if(pSlide == NULL)
	{
		ERROR_X("error! Slide have no item\n");
		return NULL;
	}
	if(ItemIndex >= SlideMaxIndex)
	{
		ERROR_X("error! Slide ItemIndex exceed MaxIndex\n");
		return NULL;
	}
	pItem = pSlide;
	for(i=0;i<ItemIndex;i++)
	{
		if(pItem->NextSlideItem == NULL)
		{
			ERROR_X("error! SlideItem Index:%d not found\n", ItemIndex);
			return NULL;
		}
		pItem=pItem->NextSlideItem;
	}
	return pItem;
}

// 取slide控件正在显示的item项的Index值
int Slide::GetSlideShowItem(void)
{
	SlideItem* pItem;
	int i;
	int result;
	int showindex;
	int temp;
	if(pSlide == NULL)
	{
		ERROR_X("error! Slide have no item\n");
		return NULL;
	}
	showindex = 0;
	result = SlideLength;
	pItem = pSlide;
	for(i=0;i<SlideMaxIndex;i++)
	{
		if(SlideOrient == MOVELEFTRIGHT)
			temp = GETABS(SlideCurrent-pItem->xshowstart);
		else
			temp = GETABS(SlideCurrent-pItem->yshowstart);
		if(result >= temp)
		{
			result = temp;
			showindex = i;
		}
		pItem=pItem->NextSlideItem;
	}
	return showindex;
}

// slide控件index对应的item项增加子控件
int Slide::SlideItemAddChild(int ItemIndex, CtlObject* pCtl)
{
	SlideItem* pItem;
	pItem = GetSlideItemFromIndex(ItemIndex);
	if(pItem != NULL)
	{
		pItem->pSlideItemImage->BindChild(pCtl);
		return 0;
	}
	return -1;
}

// slide控件设置是否自动滑动，Interval为0时不自动滑动
int Slide::SlideSetInterval(int Interval)
{
	SlideInterval = Interval;
	if((SlideInterval != 0) && (SlideInterval < 100))
		SlideInterval = 100;	
	TimerDel(&AutoSlideTimerID);
	if(SlideInterval > 0)
		AutoSlideTimerID = AddTimerAction(SlideInterval, 0, SlideAutoTimerCallback, (void*)this);
	return 0;
}

int Slide::CtlFocus(int focus)
{
	if(focus)
	{
		if((AutoSlideTimerID == -1) && (SlideInterval > 0))
			AutoSlideTimerID = AddTimerAction(SlideInterval, 0, SlideAutoTimerCallback, (void*)this);
	}
	else
	{
		TimerDel(&(SonCtlData.SlideSonCtlTimerID));
		TimerDel(&AutoSlideTimerID);
		TimerDel(&SlideTimerID);
	}
	return 0;
}

int Slide::CtlEvent(Event* pEvent)
{
	int i,j;
	int spring;
	SlideItem* pItem;
	Widget* pSlideWidget;
	Touch_Event tsevent;
	int length;
	int prelength;
	int springeffect = 160;			//	弹簧效果阻尼的大小
	Any_Event *pany_event = NULL;
	pany_event = &(pEvent->uEvent.any_event);	
	if(isTSMsg(pany_event->event_type))
	{
		TimerDel(&AutoSlideTimerID);
		if(pany_event->event_type == MsgTypeMouseDown)
		{
			if(TimerDel(&SlideTimerID) == 0)
			{
				TimerDel(&(SonCtlData.SlideSonCtlTimerID));
				SonCtlData.pFocusWidget = NULL;
			}
			else
			{
				pItem = GetSlideItemFromIndex(GetSlideShowItem());
				pSlideWidget = pItem->pSlideItemImage->pWidget;
				if(pSlideWidget->pSonWidget != NULL)
				{
					pSlideWidget->pSonWidget->WidgetSetFocus(1);
					memcpy(&tsevent, pEvent, sizeof(Touch_Event));
					tsevent.x = tsevent.x-pWidget->x-(pSlideWidget->x-pItem->xshowstart);
					tsevent.y = tsevent.y-pWidget->y-(pSlideWidget->y-pItem->yshowstart);
					SonCtlData.pFocusWidget = pSlideWidget->pSonWidget->WidgetTSFocus(&tsevent);
					if(SonCtlData.pFocusWidget != NULL)
					{
						memcpy(&SonCtlData.downevent, &tsevent, sizeof(Touch_Event));
						TimerDel(&(SonCtlData.SlideSonCtlTimerID));
						SonCtlData.SlideSonCtlTimerID = AddTimerAction(40, 1, SlideSonCtlTimerCallback, (void*)this);
					}
				}
			}
			SlideSpeed = 0;
			prets_diff = 0;
			ts_diff_dec_count = 0;
			//printf("*** down ***\n");
		}
		else if(pany_event->event_type == MsgTypeMouseMove)
		{
			if(SonCtlData.pFocusWidget != NULL)
			{
				pItem = GetSlideItemFromIndex(GetSlideShowItem());
				pSlideWidget = pItem->pSlideItemImage->pWidget;	
				if(SonCtlData.SlideSonCtlTimerID == -1)
				{
					memcpy(&tsevent, pEvent, sizeof(Touch_Event));
					tsevent.x = tsevent.x-pWidget->x-(pSlideWidget->x-pItem->xshowstart);
					tsevent.y = tsevent.y-pWidget->y-(pSlideWidget->y-pItem->yshowstart);
					SonCtlData.pFocusWidget->pMyCtlObject->CtlEvent((Event*)(&tsevent));
					if(SonCtlData.pFocusWidget->pCallback != NULL)
						SonCtlData.pFocusWidget->pCallback((Event*)(&tsevent));
					CtlRefresh();
					return 0;
				}
				else
				{
					if(SlideOrient == MOVELEFTRIGHT)
						i = GETABS(pEvent->uEvent.ts_event.x-pWidget->x-(pSlideWidget->x-pItem->xshowstart)-SonCtlData.downevent.x);
					else
						i = GETABS(pEvent->uEvent.ts_event.y-pWidget->y-(pSlideWidget->y-pItem->yshowstart)-SonCtlData.downevent.y);
					if(i >= 5)
					{
						TimerDel(&(SonCtlData.SlideSonCtlTimerID));
						memset(&SonCtlData, 0, sizeof(SlideSonCtlData));
						SonCtlData.SlideSonCtlTimerID = -1;	
					}
					else
					{
						return 0;

					}
				}
			}
			if(SlideOrient == MOVELEFTRIGHT)
			{
				i = prets.x-pEvent->uEvent.ts_event.x;
				if(GETABS(prets.y-pEvent->uEvent.ts_event.y) > GETABS(i))
				{
					memcpy(&prets, &(pEvent->uEvent.ts_event), sizeof(Touch_Event));
					return 0;
				}
				spring = pWidget->width;
			}
			else
			{
				i = prets.y-pEvent->uEvent.ts_event.y;
				if(GETABS(prets.x-pEvent->uEvent.ts_event.x) > GETABS(i))
				{
					memcpy(&prets, &(pEvent->uEvent.ts_event), sizeof(Touch_Event));
					return 0;
				}
				spring = pWidget->height;
			}
			if(GETABS(i) > GETABS(prets_diff))
			{
				ts_diff_dec_count = 0;
				SlideSpeed = i;
			}
			else
			{
				ts_diff_dec_count++;
				if(ts_diff_dec_count > 1)
					prets_diff = 0;
			}
			prets_diff = i;
			SlideCurrent = SlideCurrent+i;
			if(SlideCycle == 0)
			{
				if(SlideCurrent < 0)
				{
					if(SlideSpring == 0)
						SlideCurrent = 0;
					else
						SlideCurrent = SlideCurrent-i+i/(1+springeffect*(0-SlideCurrent)/spring);
				}
				else if(SlideCurrent+spring > SlideLength)
				{
					if(SlideSpring == 0)
						SlideCurrent = SlideLength-spring;
					else
						SlideCurrent = SlideCurrent-i+i/(1+springeffect*(SlideCurrent+spring-SlideLength)/spring);
				}				
			}
			else
			{
				SlideCurrent = CycleCalc(SlideCycle, SlideCurrent, SlideLength);
			}
			//printf("move SlideLength:%d, SlideCurrent:%d, i:%d\n", SlideLength, SlideCurrent, i);
			CtlRefresh();
		}
		else if(pany_event->event_type == MsgTypeMouseUp)
		{
			if(SonCtlData.pFocusWidget != NULL)
			{
				if(SonCtlData.SlideSonCtlTimerID != -1)
					SlideSonCtlTimerCallback((void*)this);
				memcpy(&tsevent, pEvent, sizeof(Touch_Event));
				pItem = GetSlideItemFromIndex(GetSlideShowItem());
				pSlideWidget = pItem->pSlideItemImage->pWidget;	
				tsevent.x = tsevent.x-pWidget->x-(pSlideWidget->x-pItem->xshowstart);
				tsevent.y = tsevent.y-pWidget->y-(pSlideWidget->y-pItem->yshowstart);
				SonCtlData.pFocusWidget->pMyCtlObject->CtlEvent((Event*)(&tsevent));
				if(SonCtlData.pFocusWidget->pCallback != NULL)
					SonCtlData.pFocusWidget->pCallback((Event*)(&tsevent));
				memset(&SonCtlData, 0, sizeof(SlideSonCtlData));
				SonCtlData.SlideSonCtlTimerID = -1;	
				CtlRefresh();
				return 0;
			}
 			if(SlideOrient == MOVELEFTRIGHT)
			{
				if(SlideCurrent < 0)
					SlideDes = GetSlideItemFromIndex(0)->xshowstart;
				else if(SlideCurrent+pWidget->width > SlideLength)
				{
					if(SlideCycle == 0)
						SlideDes = GetSlideItemFromIndex(SlideMaxIndex-1)->xshowstart;
					else if(SlideSpeed < 0)
						SlideDes = GetSlideItemFromIndex(SlideMaxIndex-1)->xshowstart;
					else 
						SlideDes = GetSlideItemFromIndex(0)->xshowstart;
				}
				else
				{
					for(i=0;i<SlideMaxIndex;i++)
					{
						pItem = GetSlideItemFromIndex(i);
						if(INRANGE(SlideCurrent, pItem->xshowstart, pItem->xshowstart+pItem->widthshow) == 1)
							break;
					}
					if(SlideSpeed < 0)
						SlideDes = GetSlideItemFromIndex(i)->xshowstart;
					else
						SlideDes = GetSlideItemFromIndex(i+1)->xshowstart;
				}
			}
			else
			{
				if(SlideCurrent < 0)
					SlideDes = GetSlideItemFromIndex(0)->yshowstart;
				else if(SlideCurrent+pWidget->height > SlideLength)
				{
					if(SlideCycle == 0)
						SlideDes = GetSlideItemFromIndex(SlideMaxIndex-1)->yshowstart;
					else if(SlideSpeed < 0)
						SlideDes = GetSlideItemFromIndex(SlideMaxIndex-1)->yshowstart;
					else
						SlideDes = GetSlideItemFromIndex(0)->yshowstart;
				}
				else
				{
					for(i=0;i<SlideMaxIndex;i++)
					{
						pItem = GetSlideItemFromIndex(i);
						if(INRANGE(SlideCurrent, pItem->yshowstart, pItem->yshowstart+pItem->heightshow) == 1)
							break;
					}
					if(SlideSpeed < 0)
						SlideDes = GetSlideItemFromIndex(i)->yshowstart;
					else
						SlideDes = GetSlideItemFromIndex(i+1)->yshowstart;
				}
			}
			SlideTimerID = SlideStartTimer((void*)this);
			//printf("up SlideCurrent:%d, SlideDes:%d\n", SlideCurrent, SlideDes);
		}
		memcpy(&prets, &(pEvent->uEvent.ts_event), sizeof(Touch_Event));
	}
	return 0;
}

int Slide::CtlShow(PIXEL* pDesBuf)
{
	int i,j;
	SlideItem* pItem;
	Widget* pSlideWidget;
	PIXEL* pTemp;
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, Bgimagebuff, 0, 0,
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), BltMode);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	

	for(i=0;i<SlideMaxIndex;i++)
	{
		pItem = GetSlideItemFromIndex(i);
		pSlideWidget = pItem->pSlideItemImage->pWidget;
		if(SlideOrient == MOVELEFTRIGHT)
		{
			if(SlideCycle != 0)
			{
				if(SlideCurrent+pItem->widthshow >= SlideLength)
				{
					if((pSlideWidget->x <= CycleCalc(SlideCycle, SlideCurrent+pItem->widthshow, SlideLength)) || (pSlideWidget->x+pSlideWidget->width > SlideCurrent))
					{
						if(pSlideWidget->pSonWidget != NULL)
							pTemp = SlideSonCtlShow(pItem, pSlideWidget);
						else
							pTemp = pItem->pSlideItemImage->Bgimagebuff;
						if(pSlideWidget->x+pSlideWidget->width > SlideCurrent)
						{
							AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY+pSlideWidget->y, 
							         pTemp, SlideCurrent-pSlideWidget->x, 0, 
							         pSlideWidget->width-SlideCurrent+pSlideWidget->x, pSlideWidget->height, BltMode);
						}
						else
						{
							AreaDraw(pDesBuf, CycleCalc(SlideCycle, pWidget->x+BgimageOffsetX+pSlideWidget->x-SlideCurrent, SlideLength), pWidget->y+BgimageOffsetY+pSlideWidget->y, 
							         pTemp, 0, 0, 
							         GETMIN(CycleCalc(SlideCycle, SlideCurrent+pItem->widthshow, SlideLength)-pSlideWidget->x, pSlideWidget->width), pSlideWidget->height, BltMode);
						}
						if(pSlideWidget->pSonWidget != NULL)
							DelArea(pTemp);
						continue;
					}
				}
			}
			if(pSlideWidget->x > SlideCurrent+pItem->widthshow)
				continue;
			if(pSlideWidget->x+pSlideWidget->width < SlideCurrent)
				continue;
			if(pSlideWidget->pSonWidget != NULL)
				pTemp = SlideSonCtlShow(pItem, pSlideWidget);
			else
				pTemp = pItem->pSlideItemImage->Bgimagebuff;			
			if(pSlideWidget->x < SlideCurrent)
			{
				AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY+pSlideWidget->y,
				         pTemp, SlideCurrent-pSlideWidget->x, 0, 
				         pSlideWidget->width-SlideCurrent+pSlideWidget->x, pSlideWidget->height, BltMode);
			}
			else
			{
				j = pWidget->x+BgimageOffsetX+pSlideWidget->x-SlideCurrent;
				if(INRANGE(j, pWidget->x, pWidget->x+pWidget->width))
				{
					AreaDraw(pDesBuf, j, pWidget->y+BgimageOffsetY+pSlideWidget->y, 
					         pTemp, 0, 0, 
					         GETMIN(SlideCurrent+pItem->widthshow-pSlideWidget->x, pSlideWidget->width), pSlideWidget->height, BltMode);
				}
			}
			if(pSlideWidget->pSonWidget != NULL)
				DelArea(pTemp);
		}
		else
		{
			if(SlideCycle != 0)
			{
				if(SlideCurrent+pItem->heightshow >= SlideLength)
				{
					if((pSlideWidget->y <= CycleCalc(SlideCycle, SlideCurrent+pItem->heightshow, SlideLength)) || (pSlideWidget->y+pSlideWidget->height > SlideCurrent))
					{
						if(pSlideWidget->pSonWidget != NULL)
							pTemp = SlideSonCtlShow(pItem, pSlideWidget);
						else
							pTemp = pItem->pSlideItemImage->Bgimagebuff;			
						if(pSlideWidget->y+pSlideWidget->height > SlideCurrent)
						{
							AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pSlideWidget->x, pWidget->y+BgimageOffsetY, 
							         pTemp, 0, SlideCurrent-pSlideWidget->y, 
							         pSlideWidget->width, pSlideWidget->height-SlideCurrent+pSlideWidget->y, BltMode);
						}
						else
						{
							AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pSlideWidget->x, CycleCalc(SlideCycle, pWidget->y+BgimageOffsetY+pSlideWidget->y-SlideCurrent, SlideLength), 
							         pTemp, 0, 0, 
							         pSlideWidget->width, GETMIN(CycleCalc(SlideCycle, SlideCurrent+pItem->heightshow, SlideLength)-pSlideWidget->y, pSlideWidget->height), BltMode);
						}
						if(pSlideWidget->pSonWidget != NULL)
							DelArea(pTemp);
						continue;
					}
				}
			}
			if(pSlideWidget->y > SlideCurrent+pItem->heightshow)
				continue;
			if(pSlideWidget->y+pSlideWidget->height < SlideCurrent)
				continue;
			if(pSlideWidget->pSonWidget != NULL)
				pTemp = SlideSonCtlShow(pItem, pSlideWidget);
			else
				pTemp = pItem->pSlideItemImage->Bgimagebuff;			
			if(pSlideWidget->y < SlideCurrent)
			{
				AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pSlideWidget->x, pWidget->y+BgimageOffsetY, 
				         pTemp, 0, SlideCurrent-pSlideWidget->y, 
				         pSlideWidget->width, pSlideWidget->height-SlideCurrent+pSlideWidget->y, BltMode);
			}
			else
			{
				j = pWidget->y+BgimageOffsetY+pSlideWidget->y-SlideCurrent;
				if(INRANGE(j, pWidget->y, pWidget->y+pWidget->height))
				{
					AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+pSlideWidget->x, j, 
					         pTemp, 0, 0, 
					         pSlideWidget->width, GETMIN(SlideCurrent+pItem->heightshow-pSlideWidget->y, pSlideWidget->height), BltMode);
				}
			}
			if(pSlideWidget->pSonWidget != NULL)
				DelArea(pTemp);
		}
	}
	return 0;
}

