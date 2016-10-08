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
		//daniel
		//const char* lastInterface;
		//const char* currentInterface;
		wifiMgr *wifiMan;
		
		int selectWifiID;
		int selectMode;
		int settingValue;
		char selectSsid[64];
		char settingValueStr[2];

		int volume;
		int currentMode;//UImode for change interface
		
		int power;
		int fskMode;
		int interior_temperature_hope;
		int interior_temperature;//interior_temperature
		int floor_heating_temperature_hope;//floor_heating_temperature_hope
		int floor_heating_temperature;
		int water_temperature_hope;//water_temperature_hope
		int water_temperature; //duke add
		int list_of_24_hour[24];
		int last_24_hour;
		int ontime;//ryze add
		int offtime;//ryze add
		List *list;//ryze add
		int eco_mode;
		int burning;
		int errorCode;
		int userWater;
		int fskVersion; // duke add
	
		bool wifi_icon;
		bool reserve_icon;
		bool ez_icon;
		bool goout_icon;
		//daniel ends
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

		//daniel
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
