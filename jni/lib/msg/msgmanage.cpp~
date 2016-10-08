#include <unistd.h>
#include "msgmanage.h"
#include "flagdefine.h"
#include "log.h"

MsgManage msgmanage;

// 消息队列注册
void MsgManageInit(void)
{
	pthread_mutex_init(&(msgmanage.MsgMutex), NULL);
	memset((unsigned char*)(&msgmanage.MsgQue), 0, sizeof(Msg));
}

// 清空消息队列
void ClearMsgQueue(void)
{
	if(0 == pthread_mutex_lock(&msgmanage.MsgMutex) )
	{
		msgmanage.MsgQue.EventNum = 0;
		pthread_mutex_unlock(&msgmanage.MsgMutex);
		return;
	}
	ERROR_X("ClearMsgQueue err\n");
}

// 清空指定type的消息队列中的消息
void ClearEventTypeMsgQueue(int event_type)
{
	int i;
	Any_Event *pany_event = NULL;	
	if(0 == pthread_mutex_lock(&msgmanage.MsgMutex) )
	{
		for(i=0; i<msgmanage.MsgQue.EventNum; i++)
		{
			pany_event = &(msgmanage.MsgQue.EventQue[i].uEvent.any_event);
			if(event_type == pany_event->event_type)
			{
				if(msgmanage.MsgQue.EventNum > i+1)
					memcpy(&(msgmanage.MsgQue.EventQue[i]),&(msgmanage.MsgQue.EventQue[i+1]),sizeof(Event)*(msgmanage.MsgQue.EventNum-i-1));
				msgmanage.MsgQue.EventNum--;
			}
		}
		pthread_mutex_unlock(&msgmanage.MsgMutex);
		return;
	}
	ERROR_X("ClearMsgQueueEventType err\n");
}

// 清空触摸屏消息
void ClearTSMsgQueue(void)
{
	if(GetMsgCnt() == 0)
		return;
	ClearEventTypeMsgQueue(MsgTypeMouseMove);
	ClearEventTypeMsgQueue(MsgTypeMouseUp);
	ClearEventTypeMsgQueue(MsgTypeMouseDown);
}

// 是否为触摸屏消息
int isTSMsg(int event_type)
{
	if((event_type == MsgTypeMouseMove) || (event_type == MsgTypeMouseUp) || (event_type == MsgTypeMouseDown))
		return 1;
	return 0;
}

// 消息队列中消息的个数
int GetMsgCnt(void)
{
	int MsgCnt;
	if(0 == pthread_mutex_lock(&msgmanage.MsgMutex))
	{
		MsgCnt = msgmanage.MsgQue.EventNum;
		pthread_mutex_unlock(&msgmanage.MsgMutex);
		return MsgCnt;
	}
	return -1;
}

// 发送消息到消息队列中
int PostSysMsg(Event* pEvent)
{
	if(0 == pthread_mutex_lock(&msgmanage.MsgMutex) )
	{
		if(msgmanage.MsgQue.EventNum < MSG_QUEUE_SIZE-2)
		{
			memcpy(&(msgmanage.MsgQue.EventQue[msgmanage.MsgQue.EventNum]), pEvent, sizeof(Event));
			msgmanage.MsgQue.EventNum++;
			pthread_mutex_unlock(&msgmanage.MsgMutex);
			return 0;
		}
		ERROR_X("PostSysMsg MsgCnt exceed length, error!\n");
		pthread_mutex_unlock(&msgmanage.MsgMutex);
	}
	return -1;
}

// 从消息队列中取消息，消息按照优先的顺序被取出。优先级在SysMsgType中定义
int GetSysMsg(Event* pEvent)
{
	int i;
	int PriorityHighEventType;
	int PriorityHighEventCnt;
	Any_Event *pany_event = NULL;
	if(0 == pthread_mutex_lock(&msgmanage.MsgMutex) )
	{
		if(msgmanage.MsgQue.EventNum)
		{
			PriorityHighEventCnt = 0;
			PriorityHighEventType = MsgTypeNone;
			for(i=0; i<msgmanage.MsgQue.EventNum; i++)
			{
				pany_event = &(msgmanage.MsgQue.EventQue[i].uEvent.any_event);
				if(PriorityHighEventType < pany_event->event_type)
				{
					PriorityHighEventType = pany_event->event_type;
					PriorityHighEventCnt = i;
				}
			}
			memcpy(pEvent,&(msgmanage.MsgQue.EventQue[PriorityHighEventCnt]),sizeof(Event));
			if(msgmanage.MsgQue.EventNum > PriorityHighEventCnt+1)
				memcpy(&(msgmanage.MsgQue.EventQue[PriorityHighEventCnt]),&(msgmanage.MsgQue.EventQue[PriorityHighEventCnt+1]),sizeof(Event)*(msgmanage.MsgQue.EventNum-PriorityHighEventCnt-1));
			msgmanage.MsgQue.EventNum--;
			pthread_mutex_unlock(&msgmanage.MsgMutex);
			return 0;
		}
		pthread_mutex_unlock(&msgmanage.MsgMutex);
	}
	return -1;
}

// 打印消息队列中的消息
void PrintfMsgQueue(int event_type)
{
	int i;
	Any_Event *pany_event = NULL;	
	INFO_X("***PrintfMsgQueue Start***\n");
	if(0 == pthread_mutex_lock(&msgmanage.MsgMutex) )
	{
		for(i=0; i<msgmanage.MsgQue.EventNum; i++)
		{
			pany_event = &(msgmanage.MsgQue.EventQue[i].uEvent.any_event);
			INFO_X("%d:  event_type=%d\n", i, pany_event->event_type);
		}
		pthread_mutex_unlock(&msgmanage.MsgMutex);
		INFO_X("***PrintfMsgQueue End***\n");
		return;
	}
	INFO_X("***PrintfMsgQueue Error***\n");
}

// 发送加载界面图片的消息
int SendLoadBmpMsg(void (*loadbmp)(void), void* pInterface)
{
	LoadBmp_Event event;
	memset(&event, 0, sizeof(LoadBmp_Event));
	event.event_type = MsgTypeLoadBmp;
	event.loadbmp = loadbmp;
	event.pCtlLoadBmp = pInterface;
	PostSysMsg((Event*)(&event));
	return 0;
}

// 发送开始或停止定时器消息
int SendTimerStartStopMsg(int TimerID, int TimerStartStop)
{
	Timer_Event event;
	memset(&event, 0, sizeof(Timer_Event));
	event.event_type = MsgTypeTimer;
	event.TimerID = TimerID;
	event.TimerStartStop = TimerStartStop;
	PostSysMsg((Event*)(&event));
	return 0;
}

int SendConMgrWiFiMsg(int ConMgrEventId, void *data) {

	ConMgr_Event event;

	memset(&event, 0, sizeof(ConMgr_Event));
	event.event_type = MsgTypeConMgrWiFi;
	event.ConMgrEventID = ConMgrEventId;
	event.pdata = data;
	PostSysMsg((Event*)(&event));
	return 0;

}

int SendDeviceMsg(int DeviceEventId, void *data) {

      Device_Event event;

      memset(&event, 0, sizeof(Device_Event));
      event.event_type = MsgTypeDevice;
      event.DeviceEventID = DeviceEventId;
      event.pdata = data;
      PostSysMsg((Event*)(&event));
      return 0;
}

int SendInterfaceChangeMsg(void* pInterface) {
	InterfaceChange_Event event;
	memset(&event, 0, sizeof(InterfaceChange_Event));
	event.event_type = MsgTypeInterfaceChange;
	event.pInterface = pInterface;
	PostSysMsg((Event*)(&event));
	return 0;
}


