#ifndef _MSGMANAGE_H_
#define _MSGMANAGE_H_

#include <pthread.h>

#define MSG_QUEUE_SIZE 100

// pls arrage SysMsgType in the order of priority
//The following is the event type defined.
enum SysMsgType
{
	MsgTypeNone = 0,
	MsgTypeLoadBmp = 1,		//	low priority
	MsgTypeConMgrWiFi,		// WiFi connectivity mgr
	MsgTypeDestory,
    	MsgTypeMedia,
	MsgTypeTimer,
    	MsgTypeKey,
	MsgTypeMouseMove,
	MsgTypeMouseUp,
	MsgTypeMouseDown,
	MsgTypeInterfaceChange,      //daniel, Interface update Msg
	MsgTypeDevice,
    	MsgPowerOff,			//	high priority
};

//daniel
typedef struct _InterfaceChange_Event
{
    int event_type;
    void *pInterface;
    void *pdata;

} InterfaceChange_Event;

typedef struct _ConMgr_Event
{
    int event_type;
    int ConMgrEventID;
    void *pdata;

} ConMgr_Event;


typedef struct _Device_Event
{
    int event_type;
    int DeviceEventID;
    void *pdata;

} Device_Event;

typedef struct _Key_Event
{
	int event_type;
	int keyCode;
}Key_Event;

typedef struct _Any_Event
{
	int event_type;
	void *pdata;  
}Any_Event;

typedef struct _Touch_Event
{
	int event_type;
	int x;
	int y;
	struct timeval tv;
}Touch_Event;

typedef struct _LoadBmp_Event
{
	int event_type;
	void (*loadbmp)(void);
	void* pCtlLoadBmp;
}LoadBmp_Event;

typedef struct _Timer_Event
{
	int event_type;
	unsigned int TimerID;
	int TimerStartStop;
}Timer_Event;

typedef struct _Event Event;
typedef struct _Msg Msg;

/* ===================================================
  if new event type is added. we need to update this union
============================================================
*/
struct _Event
{
	union 
	{
		Any_Event any_event;
		Key_Event key_event;
		Touch_Event ts_event;
		LoadBmp_Event loadbmp_event;
		Timer_Event timer_event;
		ConMgr_Event conmgr_event;
		InterfaceChange_Event interfacechange_event; //daniel
		Device_Event device_event;
	}uEvent;  
};

struct _Msg
{
	int EventNum;
	Event EventQue[MSG_QUEUE_SIZE];
};

struct MsgManage
{
	pthread_mutex_t MsgMutex;
	Msg MsgQue;
};

void MsgManageInit(void);
void ClearMsgQueue(void);
void ClearEventTypeMsgQueue(int event_type);
void ClearTSMsgQueue(void);
int isTSMsg(int event_type);
int GetMsgCnt(void);
int PostSysMsg(Event* pEvent);
int GetSysMsg(Event* pEvent);
void PrintfMsgQueue(int event_type);

int SendLoadBmpMsg(void (*loadbmp)(void), void* pInterface);
int SendTimerStartStopMsg(int TimerID, int TimerStartStop);
int SendConMgrWiFiMsg(int ConMgrEventId, void *data);
int SendInterfaceChangeMsg(void* pInterface);
int SendDeviceMsg(int DeviceEventId, void *data);

#endif

