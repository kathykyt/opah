#ifndef _EZSWITCH_H_
#define _EZSWITCH_H_

#include "image.h"

enum _ezSwitchType{
	EZ_SWITCH_TYPE_GOOD = 1,
	EZ_SWITCH_TYPE_FAIR,
	EZ_SWITCH_TYPE_POOR
};


typedef void (*OnSwitchListener)(int type, void *data);

class EzSwitch: public Image
{
    private:
		int levelBltMode;
		Image* levelFair, *levelPoor, *levelGood;
		OnSwitchListener switchListener;	
		int switcherPreX;
    protected:
		

    public:
        EzSwitch(int defaultType, int x, int y, int w, int h);
        ~EzSwitch(void);
		void SetOnSwitchListener(OnSwitchListener listener);
		OnSwitchListener getOnSwitchListener();
		Image* getLevel(int type);
		int getLevelType();
		void resetLevel(int type);
		void setLevel(int type);
        int CtlFocus(int focus);
        int CtlEvent(Event *pEvent);
        int CtlShow(PIXEL *pDesBuf);
		int type;
		int moveDistance;
		int timerId;	
		Image *pSwitcher;
};

#endif
