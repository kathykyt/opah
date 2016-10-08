#include "flagdefine.h"
#include "widgetmanage.h"
#include "memorymanage.h"
#include "log.h"

WidgetManage::WidgetManage(void)
{
	pRootWidget = NULL;
	pFocusWidget = NULL;
}

WidgetManage::~WidgetManage(void)
{

}

// 窗口系统增加子窗口
void WidgetManage::WidgetManageAddChildWidget(Widget* pChild, const char* pName)
{
	Widget* pSibling;	
	Widget* pTemp;
	if((pChild == NULL) && (pRootWidget == NULL))
	{
		ERROR_X("error! WidgetManage AddChildWidget Fail\n");
		return;
	}	
	pChild->pParentWidget = pRootWidget;
	pChild->WidgetSetName(pName);
	if(pRootWidget->pSonWidget == NULL)
	{
		pRootWidget->pSonWidget = pChild;
	}
	else
	{
		pTemp = pRootWidget->pSonWidget;
		pSibling = pTemp->pSiblingWidget;
		while(pSibling != NULL)
		{
			pTemp = pSibling;
			pSibling = pSibling->pSiblingWidget;
		}
		pTemp->pSiblingWidget = pChild;
	}
}

// 窗口系统删除子窗口
void WidgetManage::WidgetManageDelChildWidget(Widget* pChild)
{
	Widget* pSibling;
	Widget* pTemp;
	if((pChild == NULL) && (pRootWidget == NULL) && (pRootWidget == pChild))
	{
		ERROR_X("error! WidgetManage DelChildWidget Fail\n");
		return;
	}
	pChild->WidgetDel();
}

// 窗口系统设置子窗口成为焦点
void WidgetManage::WidgetManageSetChildWidgetFocus(Widget* pChild)
{
	if(pChild == NULL)
	{
		ERROR_X("error! WidgetManage SetFocus NULL Widget\n");
		return;
	}	
	pRootWidget->WidgetSetFocus(0);
	pChild->WidgetSetFocus(1);
}

void WidgetManage::InterfaceChangePatchEvent(Event* pEvent) {
	//UILog("WidgetManage::InterfaceChangePatchEvent\n");
	InterfaceChange_Event *pany_event = NULL;
	Widget* pMyInterface;
	pany_event = &(pEvent->uEvent.interfacechange_event);
	//UILog("pany_event type = %d\n", pany_event->event_type);
	if(pany_event->event_type == MsgTypeInterfaceChange) {
		pMyInterface = (Widget*)pany_event->pInterface;
		if(pMyInterface != NULL) {
			if(pMyInterface->pCallback != NULL) {
				pMyInterface->pCallback(pEvent);
			} else {
				printf("pMyInterface->pCallback == NULL\n");
			}
		}
	}
}

void WidgetManage::DevicePatchEvent(Event* pEvent) {
	Device_Event *pany_event = NULL;
	Widget* pFocusInterface;
	pany_event = &(pEvent->uEvent.device_event);
	if(pany_event->event_type == MsgTypeDevice) {
		pFocusInterface = pRootWidget->pSonWidget;
		while((pFocusInterface != NULL) && (pFocusInterface->focus == 0))
			pFocusInterface = pFocusInterface->pSiblingWidget;
		if (pFocusInterface->pCallback != NULL) {
			pFocusInterface->pCallback(pEvent);
		}
	}
}

// 窗口系统分发事件
void WidgetManage::WidgetManagePatchEvent(Event* pEvent)
{
	Any_Event *pany_event = NULL;
	Widget* pSibling;
	pany_event = &(pEvent->uEvent.any_event);
	if(pany_event->event_type == MsgTypeMouseDown)
	{
		pSibling = pRootWidget->pSonWidget;
		while((pSibling != NULL) && (pSibling->focus == 0))
			pSibling = pSibling->pSiblingWidget;
		if(pSibling == NULL)
			return;
		pFocusWidget = pSibling->WidgetTSFocus((Touch_Event*)pEvent);
		if(pFocusWidget != NULL)
		{
			//UILog("Down pFocusWidget-->x:%d,y:%d,w:%d,h:%d\n", pFocusWidget->x,pFocusWidget->y,pFocusWidget->width,pFocusWidget->height);
			pFocusWidget->pMyCtlObject->CtlEvent(pEvent);
			if(pFocusWidget->pCallback != NULL)
				pFocusWidget->pCallback(pEvent);
		}
	}
	else if((pany_event->event_type == MsgTypeMouseMove) || (pany_event->event_type == MsgTypeMouseUp))
	{
		if(pFocusWidget != NULL)
		{
			pFocusWidget->pMyCtlObject->CtlEvent(pEvent);
			if(pFocusWidget->pCallback != NULL)
				pFocusWidget->pCallback(pEvent);
			if(pany_event->event_type == MsgTypeMouseUp)
				pFocusWidget = NULL;
		}
	}
	else if(pany_event->event_type == MsgTypeKey)
	{

	}
}

// 窗口系统绘图
void WidgetManage::WidgetManageOnDraw(WidgetRefresh* pDraw)
{
	Widget* pSibling;
	pSibling = pRootWidget->pSonWidget;
	while((pSibling != NULL) && (pSibling->focus == 0))
		pSibling = pSibling->pSiblingWidget;
	if(pSibling == NULL)
		return;
	while(pSibling->loadbmpflag == 0)
		usleep(10000);
	//if(pSibling->pWidgetName != NULL)
	//	INFO_X("WidgetManageOnDraw:%s\n", pSibling->pWidgetName);
	pSibling->WidgetOnDraw(pDraw, GetBackupBuffer());
	//UILog("WidgetManageOnDraw end\n");
}

// 查看窗口系统的信息
void WidgetManage::WidgetManagePrintf(void)
{
	pRootWidget->WidgetPrintf(0);
}
