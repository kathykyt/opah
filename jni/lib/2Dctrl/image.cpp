#include "image.h"
#include "draw.h"

// image控件加载底图的控件，没有按键处理。该控件为所有带有底图的父控件
Image::Image(int x, int y, int w, int h):CtlObject(x, y, w, h)
{
 	BltMode = REPLACE_STYLE;
	BgimageOffsetX = 0;
	BgimageOffsetY = 0;
	Bgimagebuff = NULL;
	BgColor = TRANSCOLOR;
	pWidget->pMyCtlObject = this;
}

Image::~Image(void)
{
	FreeBmp(Bgimagebuff);
}

void Image::SetImageRefresh(void)
{
	CtlRefresh();
}

// image控件加载背景图片
void Image::LoadImagePic(unsigned char* pBgPic)
{
	int width,height;
	int picID;
	GetBMP_wid(pBgPic, &BgimageWidth, &BgimageHeight, &Bgimagebuff, &picID);
}

void Image::SetImagePic(unsigned char* pBgPic)
{
	LoadImagePic(pBgPic);
	CtlRefresh();
}

// image控件背景图片显示的offset值
void Image::SetImageOffset(int offsetx, int offsety)
{
	BgimageOffsetX = offsetx;
	BgimageOffsetY = offsety;
}

void Image::SetImageBgColor(PIXEL color)
{
	BgColor = color;
}

void Image::SetImageBltMode(int bltmode)
{
	if((bltmode >= REPLACE_STYLE) && (bltmode <= ALPHA_STYLE))
		BltMode = bltmode;
	else
		ERROR_X("error! bltmode isn't exist\n");
}

void Image::SetImageX(int x)
{
	pWidget->x = x;
}

int Image::CtlFocus(int focus)
{
	return 0;
}

int Image::CtlEvent(Event* pEvent)
{
	return 0;
}

int Image::CtlShow(PIXEL* pDesBuf)
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
	return 0;
}
