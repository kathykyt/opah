#include "rbutton.h"
#include "draw.h"

RButton::RButton(int x, int y, int w, int h):Image(x, y, w, h)
{
    index = 0;
	ButtonSelOffsetX = 0;
	ButtonSelOffsetY = 0;
	ButtonNotSelOffsetX = 0;
	ButtonNotSelOffsetY = 0;
	ButtonSelOrNot = BUTTONUP;
	ButtonSelbuff = NULL;
	ButtonNotSelbuff = NULL;
	align = ALIGN_CENTER;
	id = 0;
	pWidget->pMyCtlObject = this;
}

RButton::~RButton(void)
{
	FreeBmp(ButtonSelbuff);
	FreeBmp(ButtonNotSelbuff);
}

void RButton::SetId(int id)
{
	RButton::id = id;

}

int RButton::GetId()
{
	return id;
}

// load button的按下/没有按下的图片
void RButton::LoadButtonPic(unsigned char* pSelPic, unsigned char* pNotSelPic)
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
void RButton::SetButtonSelOffset(int offsetx, int offsety)
{
	ButtonSelOffsetX = offsetx;
	ButtonSelOffsetY = offsety;
}

// load button的没有按下的图片显示的偏移值
void RButton::SetButtonNotSelOffset(int offsetx, int offsety)
{
	ButtonNotSelOffsetX = offsetx;
	ButtonNotSelOffsetY = offsety;
}

// 取button的BUTTONDOWN/BUTTONUP的信息
int RButton::GetButtonSelOrNot(void)
{
	return ButtonSelOrNot;
}

// daniel 设置值
int RButton::SetButtonSelOrNot(bool updown)
{
    ButtonSelOrNot = updown;
	return ButtonSelOrNot;
}

// 取button的Selbuff的信息
PIXEL* RButton::GetButtonSelbuff(void)
{
	return ButtonSelbuff;
}
// 取button的NotSelbuff的信息
PIXEL* RButton::GetButtonNotSelbuff(void)
{
	return ButtonNotSelbuff;
}

int RButton::GetButtonSelWidth(void)
{
	return ButtonSelWidth;
}

int RButton::GetButtonSelHeight(void)
{
	return ButtonSelHeight;
}

int RButton::CtlFocus(int focus)
{
	return 0;
}

int RButton::CtlEvent(Event* pEvent)
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

		if(btnListener != NULL){
			btnListener(pEvent, this);
		}
		if(ButtonNotSelbuff != ButtonSelbuff)
			CtlRefresh();
	}
	return 0;
}

void RButton::SetTextSize(int size){
	textSize = size;

}

void RButton::SetTextColor(Color color){
	textColor.a = color.a;
	textColor.b = color.b;
	textColor.r = color.r;
	textColor.g = color.g;
}

void RButton::SetOnClickListener(OnClickListener listener){

	btnListener = listener;
}

void RButton::SetText(char* text){
	if (pText != NULL) uifree((void *)pText);
	textLen = strlen(text);
	pText = (char *)uimalloc(textLen + 1);
	strcpy((char *)pText, text);
	*(pText + textLen + 1) = 0;
	CtlRefresh();

}

char* RButton::GetText()
{
	return pText;
}

extern FT_Face face;
int RButton::CtlShow(PIXEL* pDesBuf)
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


	if (pText != NULL) {
		const char *p;
		int pen;
		unsigned short code;
		Bitmap fontBmp, *pFontBmp;
		FT_Bitmap *ftbitmap;
		FT_GlyphSlot glyph;
		
		pen = 0;
		p = pText;
		font_set_size(textSize);
		unsigned char* t;
		//INFO_X("start to find code=%s\n", p);
		int i = 0;
		int px = 0, py= 0;
		int ascender = face->size->metrics.ascender  >> 6;
		int descender = face->size->metrics.descender >> 6;
		int fontheight = ((face->size->metrics.height >> 6) + ascender + descender) / 2;
		while (*p && (p - pText) < textLen){
			code = utf8_get_char(p, &p);
			//INFO_X("get code %x  %c\n", code, code);
			if (code == 32){
				//INFO_X("space, add 4 pix");
				pen += 4;
				continue;
			}
			glyph = font_get_font(code);
			ftbitmap = &glyph->bitmap;
			t = _font_render(code, &textColor, ftbitmap, &fontBmp);
			pFontBmp = (Bitmap*)t;
			
			if (align == ALIGN_CENTER){
				px = pWidget->x + BgimageOffsetX + pen + pWidget->width/2 - ftbitmap->width/2;
			}else
				px = pWidget->x + BgimageOffsetX + pen;
			py = pWidget->y + (pWidget->height + fontheight)/2 - glyph->bitmap_top + BgimageOffsetY;
			DrawString(pDesBuf, px,
				py,
				(PIXEL *)t,
				0, 0,
				DIFF(pFontBmp->width, BgimageOffsetX, pWidget->width),
				DIFF(pFontBmp->height, BgimageOffsetY, pWidget->height),
				BltMode);
			
			pen = pen + DIFF(pFontBmp->width, BgimageOffsetX, pWidget->width);
			free(t);

		}
		//INFO_X("get code finish\n");
	}

	
	return 0;
}

