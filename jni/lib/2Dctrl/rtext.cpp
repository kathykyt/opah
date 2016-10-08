#include "rtext.h"
#include "draw.h"
#include "font.h"
#include "rect.h"

// text控件加载多幅图片和对应的index，解析pStr中对应的index显示到屏幕上，没有按键处理功能
// 1个text控件只能显示一个字符串，多个text可以定义多个控件来显示，详见interface1的demo程序
RText::RText(int x, int y, int w, int h, int Boundary, const char *pStr) : Image(x, y, w, h) {
	pTextStr = NULL;
	if (pStr != NULL) {
		//		pTextStr = (const char*)uimalloc(strlen(pStr));
		textLength = strlen(pStr);
		pTextStr = (char *)uimalloc(textLength + 1);
		strcpy((char *)pTextStr, pStr);
		*(pTextStr + textLength + 1) = 0;
	}
	TextBoundary = Boundary;
	pWidget->pMyCtlObject = this;
	textSize = 12;
	fontColor.a = 0;
	fontColor.r = 255;
	fontColor.g = 255;
	fontColor.b = 255;
	BltMode = ALPHA_STYLE;

	BgimageOffsetY = 5;
}

RText::~RText(void) {
	if (pTextStr != NULL) uifree((void *)pTextStr);
}

// 设置text控件需要显示的字符串，控件将解析pStr中对应的index显示到屏幕上，字符串长度没有限制
int RText::SetTextStr(const char *pStr) {
	if (pTextStr != NULL) uifree((void *)pTextStr);
	//daniel
	textLength = strlen(pStr);
	pTextStr = (char *)uimalloc(textLength + 1);
	strcpy((char *)pTextStr, pStr);
	*(pTextStr + textLength + 1) = 0;
	CtlRefresh();
	return 0;
}

void RText::SetFontColor(Color color)
{

	fontColor.a = color.a;
	fontColor.b = color.b;
	fontColor.r = color.r;
	fontColor.g = color.g;
}

void RText::SetTextSize(int size)
{
	textSize = size;
}


// 返回text控件显示的字符串
const char* RText::GetTextStr(void) {
	return pTextStr;
}

void RText::SetTextX(int x) {
	pWidget->x = x;
}

int RText::CtlFocus(int focus) {
	return 0;
}

int RText::CtlEvent(Event *pEvent) {
	return 0;
}

void initRect(Rect *rect, PIXEL* buf){
if((buf == GetBackupBuffer()) || (buf == GetDisplayBuffer()))
{
	rect->w = LCDW;
	rect->h = LCDH;
}
else
{
#if (COLORPIXEL565 == 1)
	rect->w = *(unsigned int*)buf;
	rect->h = *((unsigned int*)buf+1);
#else
	rect->w = *(unsigned int*)buf;
	rect->h = *((unsigned int*)buf+1);
#endif
}

}

extern FT_Face face;

int RText::CtlShow(PIXEL *pDesBuf) {
	//const char* p;
	//volatile char *p;
	const char *p;
	int pen;
	unsigned short code;
	Bitmap fontBmp, *pFontBmp;
	FT_Bitmap *ftbitmap;
	FT_GlyphSlot glyph;

	if (Bgimagebuff != NULL) {
		AreaDraw(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, Bgimagebuff, 0, 0,
			DIFF(BgimageWidth, BgimageOffsetX, pWidget->width), DIFF(BgimageHeight, BgimageOffsetY, pWidget->height), BltMode);
	}
	else if (BgColor != TRANSCOLOR) {
		AreaFill(pDesBuf, pWidget->x + BgimageOffsetX, pWidget->y + BgimageOffsetY, pWidget->width - BgimageOffsetX, pWidget->height - BgimageOffsetY, BgColor);
	}

	if (pTextStr != NULL) {
		pen = 0;
		p = pTextStr;
		font_set_size(textSize);
		unsigned char* t;
		//INFO_X("start to find code=%s\n", p);
		int px = 0, py= 0;
		int ascender = face->size->metrics.ascender  >> 6;
		int descender = face->size->metrics.descender >> 6;
		int fontheight = ((face->size->metrics.height >> 6) + ascender + descender) / 2;
		while (*p && (p - pTextStr) < textLength){
			code = utf8_get_char(p, &p);
			//space
			if (code == 32){
				//INFO_X("space, add 4 pix");
				pen += 4;
				continue;
			}
			//INFO_X("get code %x  %c\n", code, code);
			glyph = font_get_font(code);
			ftbitmap = &glyph->bitmap;
			t = _font_render(code, &fontColor, ftbitmap, &fontBmp);
			pFontBmp = (Bitmap*)t;

			//INFO_X("face fh=%d , h=%d", fontheight, face->size->metrics.height>>6);
			px = pWidget->x + BgimageOffsetX + pen;
			py = pWidget->y + (pWidget->height + fontheight)/2 - glyph->bitmap_top + BgimageOffsetY;
			py -= 4;

			DrawString(pDesBuf, px,
				py,
				(PIXEL *)t,
				0, 0,
				DIFF(pFontBmp->width, BgimageOffsetX, pWidget->width),
				DIFF(pFontBmp->height, BgimageOffsetY, pWidget->height),
				BltMode);
		
			pen = pen + DIFF(pFontBmp->width, BgimageOffsetX, pWidget->width) + TextBoundary;
			free(t);
			//INFO_X("draw end");
		}
		//INFO_X("get code finish\n");
	}

	return 0;
}


