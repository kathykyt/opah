#ifndef _TEXT_H_
#define _TEXT_H_

#include "mulimage.h"


class Text:public Image
{
    private:
	protected:
		//const char* pTextStr;
		//char* pTextStr;
		//int TextBoundary;
		
    public:	
		int TextBoundary;
		char* pTextStr;
		MulImage* pTextPic;
		Text(int x, int y, int w, int h, int Boundary, const char* pStr);
		~Text(void);
		int SetTextStr(const char* pStr);
		const char* GetTextStr(void);
		void SetTextX(int x);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
		
						
};

#endif
