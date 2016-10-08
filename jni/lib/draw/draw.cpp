#include "draw.h"
#include "widgetmanage.h"
#include "log.h"

static pthread_t thrdDraw;
static CtlRoot* pRoot;
static pthread_mutex_t DrawMutex;
static volatile int DrawStartStop;
static volatile int EffectMode;
static volatile int DrawSet;

// 等待上次的设置绘图线程完成后再处理，常用于界面的跳转
void CheckDrawThread(void)
{
	while(DrawSet == 1)
		usleep(1000);
}

// 启动绘图线程绘制界面
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
// 绘图线程绘制界面		
				//printf("00");
				pRoot->pWidgetManage->WidgetManageOnDraw(&refresh);
				
				if(DrawStartStop == DRAWTHREADONE)
					DrawStartStop = DRAWTHREADSTOP;
// 绘图线程绘制界面后切换buff，界面将显示到前端
				SwapBufferEffect(EffectMode);
// 绘图线程绘制界面后，保存图片的buff，如果是一个控件重绘，将控件的窗口保存即可，节约时间
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

