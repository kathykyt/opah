#ifndef _EZWIDGET_H_
#define _EZWIDGET_H_

#include "point.h"

#define EZ_MODE_AIR		100
#define EZ_MODE_HOTWATER	101
#define EZ_MODE_GOOUT	102

#define EZ_STATE_ROLLING	1
#define EZ_STATE_STOP		0

typedef void (*OnClickListener)(Event* pEvent, void *data);
typedef void (*OnRollListener)(Event* pEvent, void *data);

class Ezwidget: public Image
{
    private:
        int imgWidth, imgHeight;
        PIXEL *pUnselectImg, *pSelectedImg, * currentImg;
		
		OnClickListener listener;
		OnRollListener	rollListener;
		
    protected:

    public:
        Ezwidget(void* pezwin, int x, int y, int w, int h);
        ~Ezwidget(void);
        void SetPosition(Point pos);
        void GetPosition(Point *point);
        void SetActiveImage(bool flag);
		void SetOnClickListener(OnClickListener listener);
		void SetOnRollListener(OnRollListener listener);
        PIXEL *GetImage();
        void LoadImage(unsigned char *pUnselect, unsigned char *pSelected);
        int CtlFocus(int focus);
        int CtlEvent(Event *pEvent);
        int CtlShow(PIXEL *pDesBuf);
		double angel;
		void* pEzWin;
		int timerId;
		int type;
		int mode;
		int isRolling;
};

#endif
