#ifndef _CTLOBJECT_H_
#define _CTLOBJECT_H_

#include "flagdefine.h"
#include "widget.h"
#include "msgmanage.h"
#include "memorymanage.h"
#include "loadbmp.h"
#include "devdraw.h"
#include "touchscreen.h"
#include "specialeffect.h"
#include "key.h"
#include "screen.h"
#include "timermanage.h"
#include "log.h"

class CtlObject
{
    private:
	protected:
    public:
		class Widget* pWidget;
		
		CtlObject(int x, int y, int w, int h);
		virtual ~CtlObject(void);
		int CtlCallback(int (*pCallback)(Event* pEvent));
		int CtlRefresh(void);
		virtual int CtlFocus(int focus);
		virtual int CtlEvent(Event* pEvent);
		virtual int CtlShow(PIXEL* pDesBuf);
		void AddChild(CtlObject* pSon);
		void BindChild(CtlObject* pSon);
};

#endif
