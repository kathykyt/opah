#ifndef _LOOPBAR_H_
#define _LOOPBAR_H_

#include "image.h"

class LoopBar:public Image
{
    private:
	protected:
		PIXEL* BarNotSelbuff1;
		PIXEL* BarSelbuff1;
                PIXEL* BarNotSelbuff2;
		PIXEL* BarSelbuff2;
                PIXEL* BarNotSelbuff3;
		PIXEL* BarSelbuff3;
                
		int BarWidth;
		int BarHeight;
                double Radius;
                int OriginX;
                int OriginY;
                int Length;
                
                int Width1;
                int Height1;
                int Width2;
                int Height2;
                int Width3;
                int Height3;
                
                int touchError;
		
		PIXEL* BarIconbuff;
		int BarIconWidth;
		int BarIconHeight;

		int BarValue;
		bool Touchable;
		int BarType;
    public:
		LoopBar(int x, int y, int w, int h);
		~LoopBar(void);
		void LoadLoopBarPic(unsigned char* pSelPic1, unsigned char* pNotSelPic1, unsigned char* pSelPic2, unsigned char* pNotSelPic2,
                        unsigned char* pSelPic3, unsigned char* pNotSelPic3, unsigned char* pIconPic);
		int GetLoopBarValue(void);
		void SetLoopBarValue(int value);
		void SetLoopBarType(int type);
		void SetTouchable(bool touch);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);				
};

#endif
