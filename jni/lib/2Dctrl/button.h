#ifndef _IMAGEBUTTON_H_
#define _IMAGEBUTTON_H_

#include "image.h"

class Button:public Image
{
    private:
	protected:
		int ButtonSelOffsetX;
		int ButtonSelOffsetY;
		PIXEL* ButtonSelbuff;
		int ButtonSelWidth;
		int ButtonSelHeight;

		int ButtonNotSelOffsetX;
		int ButtonNotSelOffsetY;
		PIXEL* ButtonNotSelbuff;
		int ButtonNotSelWidth;
		int ButtonNotSelHeight;

		int ButtonSelOrNot;
		

    public:
        int index;
		Button(int x, int y, int w, int h);
		~Button(void);
		void LoadButtonPic(unsigned char* pSelPic, unsigned char* pNotSelPic);
		void SetButtonSelOffset(int offsetx, int offsety);
		void SetButtonNotSelOffset(int offsetx, int offsety);
		int GetButtonSelOrNot(void);
                int SetButtonSelOrNot(bool updown);
                PIXEL* GetButtonSelbuff(void);
                PIXEL* GetButtonNotSelbuff(void);
                
                int GetButtonSelWidth(void);
                int GetButtonSelHeight(void);
		void ButtonRefresh(void);
                
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
};

#endif
