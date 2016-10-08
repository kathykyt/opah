#include "newbutton.h"
#include "draw.h"

NewButton::NewButton(int x, int y, int w, int h):Image(x, y, w, h)
{
    	index = 0;
	ButtonSelOffsetX = 0;
	ButtonSelOffsetY = 0;
	ButtonNotSelOffsetX = 0;
	ButtonNotSelOffsetY = 0;
	ButtonSelOrNot = BUTTONUP;
	ButtonSelbuff = NULL;
	ButtonNotSelbuff = NULL;
	isDown = false;
	clickable =true;
	visible = true;
	pWidget->pMyCtlObject = this;
}

NewButton::~NewButton(void)
{
	FreeBmp(ButtonSelbuff);
	FreeBmp(ButtonNotSelbuff);
}
int NewButton::setIndex(int id){
     index = id;
}
int NewButton::getIndex(void){
     return index;
}

bool NewButton::setDown(bool change){
	isDown = change;
}

bool NewButton::getDown(void){
        return isDown;
}

void NewButton::setClickable(bool click){
	clickable = click;	
	}

void NewButton::setVisible(bool vb){
 	visible = vb;
}
// load button的按下/没有按下的图片
void NewButton::LoadButtonPic(unsigned char* pSelPic, unsigned char* pNotSelPic)
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

// load button的按下的图片显示的偏移值
void NewButton::SetButtonSelOffset(int offsetx, int offsety)
{
	ButtonSelOffsetX = offsetx;
	ButtonSelOffsetY = offsety;
}

// load button的没有按下的图片显示的偏移值
void NewButton::SetButtonNotSelOffset(int offsetx, int offsety)
{
	ButtonNotSelOffsetX = offsetx;
	ButtonNotSelOffsetY = offsety;
}

// 取button的BUTTONDOWN/BUTTONUP的信息
int NewButton::GetButtonSelOrNot(void)
{
	return ButtonSelOrNot;
}

// daniel 设置值
int NewButton::SetButtonSelOrNot(bool updown)
{
    ButtonSelOrNot = updown;
	return ButtonSelOrNot;
}

// 取button的Selbuff的信息
PIXEL* NewButton::GetButtonSelbuff(void)
{
	return ButtonSelbuff;
}
// 取button的NotSelbuff的信息
PIXEL* NewButton::GetButtonNotSelbuff(void)
{
	return ButtonNotSelbuff;
}

int NewButton::GetButtonSelWidth(void)
{
	return ButtonSelWidth;
}

int NewButton::GetButtonSelHeight(void)
{
	return ButtonSelHeight;
}

void NewButton::ButtonRefresh(void)
{
	CtlRefresh();
}

int NewButton::CtlFocus(int focus)
{
	return 0;
}

int NewButton::CtlEvent(Event* pEvent)
{
   	if(!clickable)
	return 0;
	Any_Event *pany_event = NULL;
	pany_event = &(pEvent->uEvent.any_event);
	if(pany_event->event_type == MsgTypeMouseDown)
	{	
   		
		//ButtonSelOrNot = BUTTONDOWN;
		//if(ButtonNotSelbuff != ButtonSelbuff)
			isDown = !isDown;
			CtlRefresh();
	}
	else if(pany_event->event_type == MsgTypeMouseUp)
	{	
		//ButtonSelOrNot = BUTTONUP;
		//if(ButtonNotSelbuff != ButtonSelbuff)
			//CtlRefresh();
	}
  
	return 0;
}

int NewButton::CtlShow(PIXEL* pDesBuf)
{       
        if(!visible) 
   	return 0;
	if(Bgimagebuff != NULL)
	{
		AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, Bgimagebuff, 0, 0, 
				 DIFF(BgimageWidth,BgimageOffsetX,pWidget->width), DIFF(BgimageHeight,BgimageOffsetY,pWidget->height), BltMode);
	}
	else if(BgColor != TRANSCOLOR)
	{
		AreaFill(pDesBuf, pWidget->x+BgimageOffsetX, pWidget->y+BgimageOffsetY, pWidget->width-BgimageOffsetX, pWidget->height-BgimageOffsetY, BgColor);
	}	
	
	if(isDown)
	{	//button down
		if(ButtonSelbuff)
		{       
			AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+ButtonSelOffsetX, pWidget->y+BgimageOffsetY+ButtonSelOffsetY, ButtonSelbuff, 0, 0,
					 DIFF(ButtonSelWidth,BgimageOffsetX+ButtonSelOffsetX,pWidget->width), DIFF(ButtonSelHeight,BgimageOffsetY+ButtonSelOffsetY,pWidget->height), BltMode);
			
		}
	}
	else
	{    //button up
		if(ButtonNotSelbuff)
		{
			AreaDraw(pDesBuf, pWidget->x+BgimageOffsetX+ButtonNotSelOffsetX, pWidget->y+BgimageOffsetY+ButtonNotSelOffsetY, ButtonNotSelbuff, 0, 0, 
					 DIFF(ButtonSelWidth,BgimageOffsetX+ButtonNotSelOffsetX,pWidget->width), DIFF(ButtonSelHeight,BgimageOffsetY+ButtonNotSelOffsetY,pWidget->height), BltMode);
			
		}
	}
	return 0;
}

