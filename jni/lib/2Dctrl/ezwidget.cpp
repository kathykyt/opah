#include "text.h"
#include "draw.h"
#include "ezwidget.h"

Ezwidget::Ezwidget(void* pezwin, int x, int y, int w, int h): Image(x, y, w, h)
{
	timerId = -1;
	pEzWin = pezwin;
	type = 0;
}

Ezwidget::~Ezwidget(void)
{
}

void Ezwidget::SetPosition(Point pos)
{
    pWidget->x = pos.x;
    pWidget->y = pos.y;
}

void Ezwidget::GetPosition(Point *point)
{
    point->x = pWidget->x;
    point->y = pWidget->y;
}

void Ezwidget::LoadImage(unsigned char *pUnselect, unsigned char *pSelected)
{
    int picID;
    /*should have the same width and height*/
    GetBMP_wid(pUnselect, &imgWidth, &imgHeight, &pUnselectImg, &picID);
    GetBMP_wid(pSelected, &imgWidth, &imgHeight, &pSelectedImg, &picID);
    //default image
    currentImg = pUnselectImg;
}

void Ezwidget::SetActiveImage(bool flag)
{
    //true:actived, false unactive
    if (flag) {
        currentImg = pSelectedImg;
    } else {
        currentImg = pUnselectImg;
    }
}

void Ezwidget::SetOnClickListener(OnClickListener listener)
{
    Ezwidget::listener = listener;
}

void Ezwidget::SetOnRollListener(OnRollListener listener)
{

	Ezwidget::rollListener = listener;
}

PIXEL *Ezwidget::GetImage()
{
    return currentImg;
}

int Ezwidget::CtlFocus(int focus)
{
    return 0;
}

int Ezwidget::CtlEvent(Event *pEvent)
{
    Any_Event *pany_event = NULL;
    pany_event = &(pEvent->uEvent.any_event);

    if (!isTSMsg(pany_event->event_type)) {
        return -1;
    }

    Touch_Event *tsevent = (Touch_Event *)pEvent;
    pany_event = &(pEvent->uEvent.any_event);

    if (pany_event->event_type == MsgTypeMouseDown) {
		
    } else if (pany_event->event_type == MsgTypeMouseUp) {
        if (listener != NULL) {
            listener(pEvent, this);
        }
    }
	else if(pany_event->event_type == MsgTypeMouseMove){
		if(rollListener != NULL)
			rollListener(pEvent, this);
	}

    //INFO_X("ezwidget event x=%d y=%d", tsevent->x, tsevent->y);
    return 0;
}

int Ezwidget::CtlShow(PIXEL *pDesBuf)
{
    if (Bgimagebuff != NULL) {
        AreaDraw(pDesBuf, pWidget->x, pWidget->y, Bgimagebuff, 0, 0,
                 DIFF(BgimageWidth, 0, pWidget->width), DIFF(BgimageHeight, 0, pWidget->height), BltMode);
    } else if (BgColor != TRANSCOLOR) {
        AreaFill(pDesBuf, pWidget->x, pWidget->y, pWidget->width, pWidget->height, BgColor);
    }

    if (currentImg != NULL) {
        INFO_X("draw select image");
	    AreaDraw(pDesBuf, pWidget->x, pWidget->y, currentImg, 0, 0,
	                 pWidget->width, pWidget->height, BltMode);				
    }

    return 0;
}

