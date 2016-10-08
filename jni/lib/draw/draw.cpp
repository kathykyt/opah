#include "draw.h"
#include "widgetmanage.h"
#include "log.h"

static pthread_t thrdDraw;
static CtlRoot* pRoot;
static pthread_mutex_t DrawMutex;
static volatile int DrawStartStop;
static volatile int EffectMode;
static volatile int DrawSet;

// �ȴ��ϴε����û�ͼ�߳���ɺ��ٴ��������ڽ������ת
void CheckDrawThread(void)
{
	while(DrawSet == 1)
		usleep(1000);
}

// ������ͼ�̻߳��ƽ���
int SetDrawThread(int StartStop, int effectmode)
{
	if(0 == pthread_mutex_lock(&DrawMutex))
	{
		DrawStartStop = StartStop;
		EffectMode = effectmode;
		DrawSet = 1;
		pthread_mutex_unlock(&DrawMutex);
		return 0;
	}
	return -1;
}

static void DrawThread(void)
{
	WidgetRefresh refresh;
	while(1)
	{
		if(0 == pthread_mutex_lock(&DrawMutex))
		{		
			if(DrawStartStop != DRAWTHREADSTOP)
			{
				refresh.WidgetRefreshNum = 0;
// ��ͼ�̻߳��ƽ���		
				//printf("00");
				pRoot->pWidgetManage->WidgetManageOnDraw(&refresh);
				
				if(DrawStartStop == DRAWTHREADONE)
					DrawStartStop = DRAWTHREADSTOP;
// ��ͼ�̻߳��ƽ�����л�buff�����潫��ʾ��ǰ��
				SwapBufferEffect(EffectMode);
// ��ͼ�̻߳��ƽ���󣬱���ͼƬ��buff�������һ���ؼ��ػ棬���ؼ��Ĵ��ڱ��漴�ɣ���Լʱ��
				if(EffectMode == EFFECT_NONE)
				{
					if(refresh.WidgetRefreshNum == 1)
						AreaDraw(GetBackupBuffer(), refresh.pWidgetRefresh->x, refresh.pWidgetRefresh->y, 
			         			 GetDisplayBuffer(), refresh.pWidgetRefresh->x, refresh.pWidgetRefresh->y, 
			         			 refresh.pWidgetRefresh->width, refresh.pWidgetRefresh->height, REPLACE_STYLE);
					else
						SaveBackupBuffer();
				}
				//INFO_X("ui memory:%d\n", RemainBuffer());
			}
			DrawSet = 0;
			pthread_mutex_unlock(&DrawMutex);
			
		}
		usleep(1000);
	}
}

void InitDrawThread(CtlRoot* proot)
{
	pRoot = proot;
	DrawStartStop = DRAWTHREADSTOP;
	DrawSet = 0;
	pthread_mutex_init(&DrawMutex, NULL);
	if(pthread_create(&thrdDraw,NULL,(void*(*)(void*))(DrawThread),NULL))
	{
		ERROR_X("InitDrawThread fail\n");
		exit(0);
	}
}

