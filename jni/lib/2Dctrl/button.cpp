#include "button.h"
#include "draw.h"

Button::Button(int x, int y, int w, int h):Image(x, y, w, h)
{
    index = 0;
	ButtonSelOffsetX = 0;
	ButtonSelOffsetY = 0;
	ButtonNotSelOffsetX = 0;
	ButtonNotSelOffsetY = 0;
	ButtonSelOrNot = BUTTONUP;
	ButtonSelbuff = NULL;
	ButtonNotSelbuff = NULL;
	pWidget->pMyCtlObject = this;
}

Button::~Button(void)
{
	FreeBmp(ButtonSelbuff);
	FreeBmp(ButtonNotSelbuff);
}

// load button�İ���/û�а��µ�ͼƬ
void Button::LoadButtonPic(unsigned char* pSelPic, unsigned char* pNotSelPic)
{
	int picID;
	GetBMP_wid(pSelPic, &ButtonSelWidth, &ButtonSelHeight, &ButtonSelbuff, &picID);
	if(pNotSelPic != pSelPic)
	{
		GetBMP_wid(pNotSelPic, &ButtonNotSelWidth, &ButtonNotSelHeight, &ButtonNotSelbuff, &picID);
	}
	else
	{
		ButtonNotSelbuff = ButtonSelbuff;
		ButtonNotSelWidth = ButtonSelWidth;
		ButtonNotSelHeight = ButtonSelHeight;		
	}
}

// load button�İ��µ�ͼƬ��ʾ��ƫ��ֵ
void Button::SetButtonSelOffset(int offsetx, int offsety)
{
	ButtonSelOffsetX = offsetx;
	ButtonSelOffsetY = offsety;
}

// load button��û�а��µ�ͼƬ��ʾ��ƫ��ֵ
void Button::SetButtonNotSelOffset(int offsetx, int offsety)
{
	ButtonNotSelOffsetX = offsetx;
	ButtonNotSelOffsetY = offsety;
}

// ȡbutton��BUTTONDOWN/BUTTONUP����Ϣ
int Button::GetButtonSelOrNot(void)
{
	return ButtonSelOrNot;
}

// daniel ����ֵ
int Button::SetButtonSelOrNot(bool updown)
{
    ButtonSelOrNot = updown;
	return ButtonSelOrNot;
}

// ȡbutton��Selbuff����Ϣ
PIXEL* Button::GetButtonSelbuff(void)
{
	return ButtonSelbuff;
}
// ȡbutton��NotSelbuff����Ϣ
PIXEL* Button::GetButtonNotSelbuff(void)
{
	return ButtonNotSelbuff;
}

int Button::GetButtonSelWidth(void)
{
	return ButtonSelWidth;
}

int Button::GetButtonSelHeight(void)
{
	return ButtonSelHeight;
}

void Button::ButtonRefresh(void)
{
	CtlRefresh();
}

int Button::CtlFocus(int focus)
{
	return 0;
}

int Button::CtlEvent(Event* pEvent)
{
	Any_Event *pany_event = NULL;
	pany_event = &(pEvent->uEvent.any_event);	
	if(pany_event->event_type == MsgTypeMouseDown)
	{
		ButtonSelOrNot = BUTTONDOWN;
		if(ButtonNotSelbuff != ButtonSelbuff)
			CtlRefresh();
	}
	else if(pany_event->event_type == MsgTypeMouseUp)
	{
		ButtonSelOrNot = BUTTONUP;
		if(ButtonNotSelbuff != ButtonSelbuff)
			CtlRefresh();
	}
	return 0;
}

int Button::CtlShow(PIXEL* pDesBuf)
{
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, Bgimagebuff, 0, 0, 
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), BltMode);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	
	
	if(ButtonSelOrNot)
	{
		if(ButtonSelbuff != NULL)
		{
			AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+ButtonSelOffsetX, pWidget->y+BgimageOffsetY+ButtonSelOffsetY, ButtonSelbuff, 0, 0,
					 DIFF(ButtonSelWidth,BgimageOffsetX+ButtonSelOffsetX,pWidget->width), DIFF(ButtonSelHeight,BgimageOffsetY+ButtonSelOffsetY,pWidget->height), BltMode);
		}
	}
	else
	{
		if(ButtonNotSelbuff != NULL)
		{
			AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+ButtonNotSelOffsetX, pWidget->y+BgimageOffsetY+ButtonNotSelOffsetY, ButtonNotSelbuff, 0, 0, 
					 DIFF(ButtonSelWidth,BgimageOffsetX+ButtonNotSelOffsetX,pWidget->width), DIFF(ButtonSelHeight,BgimageOffsetY+ButtonNotSelOffsetY,pWidget->height), BltMode);
		}
	}
	return 0;
}

