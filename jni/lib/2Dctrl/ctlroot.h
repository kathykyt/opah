#ifndef _CTLROOT_H_
#define _CTLROOT_H_

#include "ctlobject.h"
#include "wifi_manager.h"
#include "list.h"

class CtlRoot:public CtlObject
{
    private:
		int (*pConMgrCallback)(Event* pEvent);
	protected:

    public:
		
		wifiMgr *wifiMan;
		
		int selectWifiID;
		int selectMode;
		int settingValue;
		char selectSsid[64];
		char settingValueStr[2];

		int volume;
		int currentMode;//UImode for change interface
		
		int power;
	
		int list_of_24_hour[24];
		int last_24_hour;
		List *list;

		bool wifi_icon;
		bool reserve_icon;
		bool ez_icon;
		bool goout_icon;
		
		class WidgetManage* pWidgetManage;
		CtlRoot(int x, int y, int w, int h);
		~CtlRoot(void);
		void CtlRootAddInterface(CtlObject* pChildCtl, const char* pInterfaceName);
		CtlObject* CtlRootGetCtl(const char* pInterfaceName);
		int CtlRootDelInterface(const char* pInterfaceName);
		int CtlFocus(int focus);
		int CtlEvent(Event* pEvent);
		int CtlInterfaceChangeEvent(Event* pEvent);
		int CtlDeviceEvent(Event* pEvent);
		int CtlShow(PIXEL* pDesBuf);
		int CtlRootSetInterfaceFocus(const char* pInterfaceName);
		int CtlRootChangeInterface(const char* pInterfaceName, int effectmode);
		void CtlRootRun(void);
		void CtlRootPrintf(void);
		int conMgrSetCallback(int (*pCallback)(Event* pEvent));
		int conMgrExecCallback(Event* pEvent);

		void setValue(int value);
		int GetHWValue(void);
		void SetHWValue(int value);
		int WriteFsk(void);
		int FskModeToCurrentMode(void);
		int WritePowerOffToFsk(void);
		int Write24HourToFsk(void);
		int Notify24Hour(int currentHour);
};

#endif

#define LOG_NEED 1
#if LOG_NEED == 1
#define UILog printf
#else
#define UILog
#endif
