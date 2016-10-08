#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "msgmanage.h"
#include "ctlobject.h"

#define ZMIN			0

typedef struct _WidgetRefresh WidgetRefresh;

struct _WidgetRefresh
{
	int WidgetRefreshNum;
	class Widget* pWidgetRefresh;
};

class Widget{
    private:
    protected:
    public:
		class Widget* pMyWidget;
		class CtlObject* pMyCtlObject;

		int x;
		int y;
		int width;
		int height;
		int z;					//	z order, 0 is lowest
		int focus;				//	focus status
		int usingClutter;		//	2D or 3D
		const char* pWidgetName;
		int loadbmpflag;
		int dirty;
		void* pPrivateData;		//	reserve for private data

		class Widget* pParentWidget;
		class Widget* pSiblingWidget;
		class Widget* pSonWidget;

		int (*pCallback)(Event* pEvent);

		Widget(void);
		~Widget(void);
		void WidgetDel(void);
		class Widget* WidgetGetSonWidgetFromName(const char* pName);
		void WidgetSetName(const char* pName);
		void WidgetSetSize(int widgetw, int widgeth);
		void WidgetSetPosition(int wingetx, int widgety);
		void WidgetSetZ(int wingetz);
		void WidgetSetFocus(int wingetfocus);
		void WidgetSetDirty(int wingetdirty);
		void WidgetSetLoadBmpFlag(int wingetloadbmp);
		void WidgetSetUsingClutter(int widgetusingclutter);
		void WidgetSetPrivateData(void* pwidgetPrivateData, int PrivateDataSize);
		void WidgetSetCallback(int (*callback)(Event* pEvent));
		void WidgetAddChild(Widget* pChild, int Mode);
		class Widget* WidgetTSFocus(Touch_Event* pTS);
		int WidgetOnDraw(WidgetRefresh* pDraw, PIXEL* pBuf);
		void WidgetPrintf(int depth);
};

#endif
