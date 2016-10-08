#ifndef _WIDGETMANAGE_H_
#define _WIDGETMANAGE_H_

#include "widget.h"
#include "msgmanage.h"

class WidgetManage
{
    private:
    protected:
    public:
		class Widget* pRootWidget;
		class Widget* pFocusWidget;

		WidgetManage(void);
		~WidgetManage(void);
		void WidgetManageAddChildWidget(Widget* pChild, const char* pName);
		void WidgetManageDelChildWidget(Widget* pChild);
		void WidgetManageSetChildWidgetFocus(Widget* pChild);
		void WidgetManagePatchEvent(Event* pEvent);
		void WidgetManageOnDraw(WidgetRefresh* pDraw);
		void WidgetManagePrintf(void);
		void InterfaceChangePatchEvent(Event* pEvent);
		void DevicePatchEvent(Event* pEvent);
};

#endif
