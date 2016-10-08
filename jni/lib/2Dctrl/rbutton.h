#ifndef __RBUTTON_H__
#define __RBUTTON_H__

#include "image.h"
#include "color.h"
#include "bitmap.h"
#include "font.h"

typedef void (*OnClickListener)(Event* pEvent, void *data);


typedef enum _Align{
	ALIGN_CENTER,
	ALIGN_LEFT,
	ALIGN_RIGHT
}Align;

class RButton:public Image
{
    private:
		int id;
		char* pText;
		int textLen;
		int align;
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

		Color textColor;
		int textSize;
		OnClickListener btnListener;
    public:
        int index;
		RButton(int x, int y, int w, int h);
		~RButton(void);
		void SetId(int id);
		int GetId();
		void LoadButtonPic(unsigned char* pSelPic, unsigned char* pNotSelPic);
		void SetButtonSelOffset(int offsetx, int offsety);
		void SetButtonNotSelOffset(int offsetx, int offsety);
		int GetButtonSelOrNot(void);
        int SetButtonSelOrNot(bool updown);
        PIXEL* GetButtonSelbuff(void);
        PIXEL* GetButtonNotSelbuff(void);
        int GetButtonSelWidth(void);
        int GetButtonSelHeight(void);
		void SetText(char *text);
		char* GetText();
		void SetTextSize(int size);
        void SetTextColor(Color color);
		void SetOnClickListener(OnClickListener listener);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
};

#endif
