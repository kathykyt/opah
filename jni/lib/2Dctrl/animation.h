#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "mulimage.h"

typedef void (*StopCallback)(void *data);

class Animation: public Image
{
    private:
		
        int TimerID;
        int Interval;
		int num;//picture size
		StopCallback stopCb;
		int clock;
		PIXEL **aniPic;
    protected:
	
    public:
        Animation(int x, int y, int w, int h, int gifinterval);
        ~Animation(void);
		void refreshAniPic();
        void Start();
        void Stop();
		void SetStopCallback(StopCallback cb);
	 	void LoadAniPic(unsigned char **pic, int number);
        int CtlFocus(int focus);
        int CtlEvent(Event *pEvent);
        int CtlShow(PIXEL *pDesBuf);
};

#endif
