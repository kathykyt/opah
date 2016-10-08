#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "ctlobject.h"

class Image:public CtlObject
{
    private:
	protected:
		int BltMode;
		PIXEL BgColor;
		
    public:
		int BgimageOffsetX;
		int BgimageOffsetY;
		PIXEL* Bgimagebuff;
		int BgimageWidth;
		int BgimageHeight;
		Image(int x, int y, int w, int h);
		~Image(void);
		void SetImageRefresh(void);
		void LoadImagePic(unsigned char* pBgPic);
		void SetImagePic(unsigned char* pBgPic);
		void SetImageOffset(int offsetx, int offsety);
		void SetImageBgColor(PIXEL color);
		void SetImageBltMode(int bltmode);
		void SetImageX(int x);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);		
};

#endif
