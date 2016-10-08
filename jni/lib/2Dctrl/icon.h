#ifndef _ICON_H_
#define _ICON_H_

#include "image.h"

class Icon:public Image
{
    private:
	protected:
		
		PIXEL* IconBuff;
		PIXEL* IconOffBuff;
		int IconWidth;
		int IconHeight;
		
		

    public:
    
		bool Status;
		int blinkTimes;
		int blinkPeriod;
		int IconTimerId;
    
		Icon(int x, int y, int w, int h, bool setStatus);
		~Icon(void);

		void LoadIconPic(unsigned char* pSelPic, unsigned char* pNotSelPic);
		bool GetIconStatus(void);
        void SetIconStatus(bool setStatus);
        void SetIconBlink(int blinkSeconds);
        void DesIconTimer(void);
		//static void IconTimerCallback (void* pct1);
		
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
};

#endif
