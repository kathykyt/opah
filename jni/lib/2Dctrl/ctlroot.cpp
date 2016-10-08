#include "ctlroot.h"
#include "widgetmanage.h"
#include "draw.h"
//#include "fsk.h"
#include "wakelock.h"

extern int delay_sleep_time_count;

// CtlRoot�ؼ�Ϊ���пؼ��ĸ��ؼ�
CtlRoot::CtlRoot(int x, int y, int w, int h):CtlObject(x, y, w, h)
{
	pWidgetManage = new WidgetManage();
	pWidgetManage->pRootWidget = pWidget;
	pConMgrCallback = NULL;
	//currentInterface = NULL;
	//lastInterface = NULL;
	selectWifiID = 0;
	selectMode = 2;
	currentMode = 2;
	settingValue = 0;
	volume = 3;
	//sensor temp init
	interior_temperature = 24;
	water_temperature_hope = 50;
	floor_heating_temperature_hope = 60;
	interior_temperature_hope = 26;
	floor_heating_temperature = 55;
	ontime = 30;//ryze add
	offtime = 3;//ryze add
	list = NULL;
	for (int i = 0; i < 24; i++) {
		list_of_24_hour[i] = 0;
	}
	last_24_hour = -1;
	
	wifi_icon = false;
	reserve_icon = false;
	ez_icon = false;
	goout_icon = false;
	
	power = 1;
//	fskMode = AIR_HEATING_MODE;
//	eco_mode = 0;
//	burning = 0;
//	errorCode = 0;
//	userWater = 0;
//	fskVersion = 12;
}

CtlRoot::~CtlRoot(void)
{
	if(pWidgetManage != NULL)
		delete(pWidgetManage);
}

int CtlRoot::CtlFocus(int focus)
{
	return 0;
}

int CtlRoot::CtlEvent(Event* pEvent)
{
	pWidgetManage->WidgetManagePatchEvent(pEvent);
	return 0;
}

int CtlRoot::CtlInterfaceChangeEvent(Event* pEvent)
{
	//UILog("CtlRoot::CtlInterfaceChangeEvent\n");
	pWidgetManage->InterfaceChangePatchEvent(pEvent);
	return 0;
}

int CtlRoot::CtlDeviceEvent(Event* pEvent)
{
	pWidgetManage->DevicePatchEvent(pEvent);
	return 0;
}

int CtlRoot::CtlShow(PIXEL* pDesBuf)
{
	return 0;
}

// ���ؼ�ע����棬��������ȥ������(���ֵĳ���û������)���ý���ĵ�һ���ӿؼ�ΪpChildCtl
void CtlRoot::CtlRootAddInterface(CtlObject* pChildCtl, const char* pInterfaceName)
{
	pWidgetManage->WidgetManageAddChildWidget(pChildCtl->pWidget, pInterfaceName);
}

// ���ݽ�������ֵõ�����ĵ�һ���ӿؼ�
CtlObject* CtlRoot::CtlRootGetCtl(const char* pInterfaceName)
{
	Widget* pInterfaceWidget;
	pInterfaceWidget = pWidgetManage->pRootWidget->WidgetGetSonWidgetFromName(pInterfaceName);
	if(pInterfaceWidget == NULL)
	{
		ERROR_X("error! can't find Ctl Interface Widget\n");
		return NULL;
	}
	return pInterfaceWidget->pMyCtlObject;
}

// ���ؼ�ɾ�����棬���������ڲ������ӿؼ�/���ӿؼ�...ȫ��ɾ�����ͷ��ڴ�
int CtlRoot::CtlRootDelInterface(const char* pInterfaceName)
{
	Widget* pInterfaceWidget;
	pInterfaceWidget = pWidgetManage->pRootWidget->WidgetGetSonWidgetFromName(pInterfaceName);
	if(pInterfaceWidget == NULL)
	{
		ERROR_X("error! can't find Del Interface Widget\n");
		return -1;
	}
	pWidgetManage->WidgetManageDelChildWidget(pInterfaceWidget);
	return 0;
}

// ���ؼ���������Ϊ���㣬��ʾ�Ͱ�������ֻ�Խ��������Ч
int CtlRoot::CtlRootSetInterfaceFocus(const char* pInterfaceName)
{
	Widget* pInterfaceWidget;
	//lastInterface = currentInterface;//Daniel modify here
	pInterfaceWidget = pWidgetManage->pRootWidget->WidgetGetSonWidgetFromName(pInterfaceName);
	if(pInterfaceWidget == NULL)
	{
		ERROR_X("error! can't find Focus Interface Widget\n");
		return -1;
	}
	pWidgetManage->WidgetManageSetChildWidgetFocus(pInterfaceWidget);
	//currentInterface = pInterfaceName;//Daniel modify here
	return 0;
}

// ���ؼ�������ת
int CtlRoot::CtlRootChangeInterface(const char* pInterfaceName, int effectmode)
{
	CheckDrawThread();
	//UILog("CtlRootChangeInterface\n");

	if(CtlRootSetInterfaceFocus(pInterfaceName) == 0)
	{
		pWidgetManage->pRootWidget->WidgetSetDirty(1);
		//pWidgetManage->pFocusWidget->WidgetSetDirty(1);
		ClearTSMsgQueue();
		SetDrawThread(DRAWTHREADONE, effectmode);
		//UILog("CtlRootChangeInterface 1\n");
		return 0;
	}
	//UILog("CtlRootChangeInterface 2\n");
	return -1;
}

int CtlRoot::conMgrSetCallback(int (*pCallback)(Event* pEvent))
{
	
	//if(pConMgrCallback == NULL) {
		pConMgrCallback = pCallback;
        //}
	return 0;
}


int CtlRoot::conMgrExecCallback(Event* pEvent)
{
	if(pConMgrCallback != NULL) {
		return pConMgrCallback(pEvent);
        }
	return -1;
}

// ���ؼ�ѭ����������ʾ�Ͱ�������ֻ�Խ��������Ч
void CtlRoot::CtlRootRun(void)
{
	Event event;
	Any_Event *pany_event = NULL;
	while(1)
	{
		if(GetSysMsg(&event) == 0)
		{
			pm_wake_lock(UI_EVENT_LOOP_FLAG, 1);

			//UILog("CtrlRoot::CtlRootRun\n");	
			pany_event = &(event.uEvent.any_event);
			switch(pany_event->event_type)
			{
				case MsgTypeLoadBmp:
					LoadBmp_Event* ploadbmp;
					UILog("CtrlRoot::CtlRootRun, MsgTypeLoadBmp\n");
					ploadbmp = &(event.uEvent.loadbmp_event);
					ploadbmp->loadbmp();
					((CtlObject*)(ploadbmp->pCtlLoadBmp))->pWidget->WidgetSetLoadBmpFlag(1);
					UILog("CtrlRoot::CtlRootRun, MsgTypeLoadBmp 2\n");
					break;
				case MsgTypeTimer:
					Timer_Event* ptimer;
					ptimer = &(event.uEvent.timer_event);
					TimerSet(ptimer->TimerID, ptimer->TimerStartStop);
					break;
				case MsgTypeDestory:

					break;
				case MsgTypeMedia:

					break;
				case MsgTypeKey:

					break;
				case MsgTypeMouseMove:
				case MsgTypeMouseUp:
				case MsgTypeMouseDown:
					delay_sleep_time_count = 0;
					CtlEvent(&event);
					//UILog("MsgTypeMouseDown\n");
					break;
				case MsgPowerOff:

					break;

				case MsgTypeConMgrWiFi:
					//UILog("MsgTypeConMgrWiFi\n");
					conMgrExecCallback(&event);				        
				        break;
				        
				case MsgTypeInterfaceChange:
					//UILog("CtlRoot::case MsgTypeInterfaceChange:\n");
					CtlInterfaceChangeEvent(&event);
					break;


				case	MsgTypeDevice:
					CtlDeviceEvent(&event);

					break;

				default:
					break;
			}

			//we can also set a time that if longer than a period that no events received, clear the flag to sleep.
			//pm_wake_lock(UI_EVENT_LOOP_FLAG, 0);

		} else {

			//we can also set a time that if longer than a period that no events received, clear the flag to sleep.
			pm_wake_lock(UI_EVENT_LOOP_FLAG, 0);
			usleep(1 * 100);
		}
	}
}

// ���ؼ����ڲ��Ĵ��ڹ�ϵ�ʹ��ڵ���Ϣ��ӡ
void CtlRoot::CtlRootPrintf(void)
{
	INFO_X("******CtlRootPrintf Start******\n");
	pWidgetManage->WidgetManagePrintf();
	INFO_X("******CtlRootPrintf  End ******\n");
}







