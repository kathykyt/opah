#include "icon.h"
#include "draw.h"

Icon::Icon(int x, int y, int w, int h, bool setStatus):Image(x, y, w, h)
{
    Status = setStatus;
    IconTimerId = -1;
    blinkTimes = 0;
    blinkPeriod = 0;
	pWidget->pMyCtlObject = this;
}

Icon::~Icon(void)
{
	TimerDel(&IconTimerId);
	FreeBmp(IconBuff);
}



void Icon::LoadIconPic(unsigned char* pSelPic, unsigned char* pNotSelPic)
{
	int picID;
	int width1, height1;
	GetBMP_wid(pSelPic, &IconWidth, &IconHeight, &IconBuff, &picID);
	GetBMP_wid(pNotSelPic, &width1, &height1, &IconOffBuff, &picID);
}

bool Icon::GetIconStatus(void)
{
	return Status;
}

void Icon::SetIconStatus(bool setStatus)
{
    Status = setStatus;
    CtlRefresh();
}

static void IconTimerCallback (void* pctl) 
{
	Icon* pCtl;
	pCtl = (Icon*) pctl;
	pCtl->Status = !pCtl->Status;
	if (pCtl->blinkTimes >= pCtl->blinkPeriod * 2) {
		pCtl->blinkTimes = 0;
		pCtl->DesIconTimer();
		pCtl->Status = true;
		((CtlObject*)pCtl)->CtlRefresh();
	} else {
		pCtl->blinkTimes++;
		((CtlObject*)pCtl)->CtlRefresh();
	}
}

void Icon::SetIconBlink(int blinkSeconds)
{
	if(IconTimerId != -1) {
		//UILog("In DesIconTimer, IconTimerId = %d\n", IconTimerId);
		DesIconTimer();
		blinkTimes = 0;
	}
	//UILog("In SetIconBlink, IconTimerId = %d\n", IconTimerId);
	blinkPeriod = blinkSeconds;
	IconTimerId = AddTimerAction(500, 0, IconTimerCallback, (void*)this);
	UILog("In Icon::SetIconBlink, Addtimer, IconTimerId = %d\n", IconTimerId);
}

void Icon::DesIconTimer(void) 
{
	UILog("In Icon::DesIconTimer, DelTimer, IconTimerId = %d\n", IconTimerId);
	TimerDel(&IconTimerId);
	IconTimerId = -1;
}

int Icon::CtlFocus(int focus)
{
	return 0;
}

int Icon::CtlEvent(Event* pEvent)
{
	return 0;
}

int Icon::CtlShow(PIXEL* pDesBuf)
{
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, Bgimagebuff, 0, 0, 
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), ALPHA_STYLE);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	
	
	if(Status)
	{
		if(IconBuff != NULL)
		{
			AreaDraw(pDesBuf, pWidget->x, pWidget->y, IconBuff, 0, 0,
					 DIFF(IconWidth, 0, pWidget->width), DIFF(IconHeight, 0, pWidget->height), ALPHA_STYLE);
		}
	}
	else
	{
		if(IconOffBuff != NULL)
		{
			AreaDraw(pDesBuf, pWidget->x, pWidget->y, IconOffBuff, 0, 0,
					 DIFF(IconWidth, 0, pWidget->width), DIFF(IconHeight, 0, pWidget->height), ALPHA_STYLE);
		}
	}
	return 0;
}

