#ifndef _RTEXT_H_
#define _RTEXT_H_

#include "mulimage.h"
#include "color.h"

class RText:public Image
{
    private:
	protected:
		//const char* pTextStr;
		char* pTextStr;
		int TextBoundary;
		Color fontColor;
		int textLength;
		int textSize;
		
    public:
		RText(int x, int y, int w, int h, int Boundary, const char* pStr);
		~RText(void);
		int SetTextStr(const char* pStr);
		const char* GetTextStr(void);
		void SetTextX(int x);
		void SetFontColor(Color color);
		void SetTextSize(int size);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);				
};

#endif
