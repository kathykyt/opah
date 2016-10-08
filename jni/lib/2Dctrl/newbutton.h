#ifndef _NEWBUTTON_H_
#define _NEWBUTTON_H_

#include "image.h"

class NewButton:public Image
{
    private:
	protected:
		int ButtonSelOffsetX;
		int ButtonSelOffsetY;
		PIXEL* ButtonSelbuff;
		int ButtonSelWidth;
		int ButtonSelHeight;
		NewButton*temp;
		int ButtonNotSelOffsetX;
		int ButtonNotSelOffsetY;
		PIXEL* ButtonNotSelbuff;
		int ButtonNotSelWidth;
		int ButtonNotSelHeight;
		int ButtonSelOrNot;
		int index;
		bool isDown;
		bool clickable;
   		bool visible;
		
    public:	
		       	        
		NewButton(int x, int y, int w, int h);
		~NewButton(void);
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
		
		int setIndex(int id);
		int getIndex(void);
		
		bool setDown(bool change);
		bool getDown(void);

		void setVisible(bool vb);
		void setClickable(bool click);
		
                
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
};

#endif
