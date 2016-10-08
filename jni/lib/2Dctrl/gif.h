#ifndef _GIF_H_
#define _GIF_H_

#include "mulimage.h"

#define GIFSTART			1
#define GIFSTOP				2

class Gif:public Image
{
    private:
	protected:
		int GifTimerID;
		int GifStartNum;
		int GifEndNum;
		int GifShowNum;
		int GifInterval;
		int GifStartStop;
		
    public:
		MulImage* pGifPic;
		Gif(int x, int y, int w, int h, int gifinterval);
		~Gif(void);
		void SetGifStartEndNum(int start, int end);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);		
};

#endif
