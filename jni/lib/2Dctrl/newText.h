#ifndef _NEWTEXT_H_
#define _NEWTEXT_H_

#include "mulimage.h"

class NewText:public Image
{
    private:
	protected:
		//const char* pTextStr;
		int TextSet [20];
		int TextBoundary;
		
    public:
		MulImage* pTextPic;
		NewText(int x, int y, int w, int h, int Boundary, int Set[], int length);
		~NewText(void);
		int SetTextStr(int Set[], int length);
		int SetTextNone(void);
		void SetTextX(int x);
		//int[] GetTextStr(void);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);				
};

#endif
