#include <unistd.h>
#include "msgmanage.h"
#include "flagdefine.h"
#include "log.h"

MsgManage msgmanage;

// ��Ϣ����ע��
void MsgManageInit(void)
{
	pthread_mutex_init(&(msgmanage.MsgMutex), NULL);
	memset((unsigned char*)(&msgmanage.MsgQue), 0, sizeof(Msg));
}

// �����Ϣ����
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

// ���ָ��type����Ϣ�����е���Ϣ
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

// ��մ�������Ϣ
void ClearTSMsgQueue(void)
{
	if(GetMsgCnt() == 0)
		return;
	ClearEventTypeMsgQueue(MsgTypeMouseMove);
	ClearEventTypeMsgQueue(MsgTypeMouseUp);
	ClearEventTypeMsgQueue(MsgTypeMouseDown);
}

// �Ƿ�Ϊ��������Ϣ
int isTSMsg(int event_type)
{
	if((event_type == MsgTypeMouseMove) || (event_type == MsgTypeMouseUp) || (event_type == MsgTypeMouseDown))
		return 1;
	return 0;
}

// ��Ϣ��������Ϣ�ĸ���
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

// ������Ϣ����Ϣ������
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

// ����Ϣ������ȡ��Ϣ����Ϣ�������ȵ�˳��ȡ�������ȼ���SysMsgType�ж���
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

// ��ӡ��Ϣ�����е���Ϣ
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

// ���ͼ��ؽ���ͼƬ����Ϣ
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

// ���Ϳ�ʼ��ֹͣ��ʱ����Ϣ
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


