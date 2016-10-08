#ifndef _BAR_H_
#define _BAR_H_

#include "image.h"

class Bar:public Image
{
    private:
	protected:
		PIXEL* BarNotSelbuff;
		PIXEL* BarSelbuff;
		int BarOffsetX;
		int BarOffsetY;
		int BarWidth;
		int BarHeight;
		
		int BarIconOffsetX;
		int BarIconOffsetY;
		PIXEL* BarIconbuff;
		int BarIconWidth;
		int BarIconHeight;

		int BarOrient;
		int BarValue;
    public:
		Bar(int x, int y, int w, int h, int barMode);
		~Bar(void);
		void LoadBarPic(unsigned char* pSelPic, unsigned char* pNotSelPic, unsigned char* pIconPic);
		void AssembleBarPic(unsigned char* pLeftOrUpSelPic, unsigned char* pRightOrDownSelPic, unsigned char* pBarSelPic,
						 unsigned char* pLeftOrUpNotSelPic, unsigned char* pRightOrDownNotSelPic, unsigned char* pBarNotSelPic,
						 unsigned char* pIconPic, int rate);
		void SetBarOffset(int offsetx, int offsety, int iconoffsetx, int iconoffsety);
		int GetBarValue(void);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);				
};

#endif
