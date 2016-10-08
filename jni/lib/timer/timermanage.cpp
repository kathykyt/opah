#include "log.h"
#include "memorymanage.h"
#include "timermanage.h"


TimerManage timer;

void TimerLoop(void)
{
	int i;
	while(1)
	{
		if(timer.TimerCurTime != timer.LastTimeCnt)
		{
			timer.LastTimeCnt = timer.TimerCurTime;
			for(i=0; i<timer.MaxTimerItem; i++)
			{
				if(timer.pTimerItem[i].isusing)
				{
					if(timer.pTimerItem[i].Born > 0)
					{
						if((timer.LastTimeCnt - timer.pTimerItem[i].Born) >= timer.pTimerItem[i].ms)
						{
							timer.pTimerItem[i].Action(timer.pTimerItem[i].pTimerCtl);
							if(pthread_mutex_lock(&timer.TimerMutex) == 0)
							{
								if(timer.pTimerItem[i].SingleShot)
									timer.pTimerItem[i].Born = 0;
								else
									timer.pTimerItem[i].Born = timer.LastTimeCnt;
								pthread_mutex_unlock(&timer.TimerMutex);
							}
						}
					}
				}
			}
		}
		usleep(TIMER_STEP);
	}
}

void IncTimeCnt(int TimerCur)
{
	timer.TimerCurTime++;
}

void ActTimer(void)
{
	struct itimerval itv, oldtv;
	struct sigaction act;
	itv.it_value.tv_sec = INTERVAL_SEC;
	itv.it_value.tv_usec = INTERVAL_USEC;
	itv.it_interval = itv.it_value;
	setitimer(ITIMER_REAL, &itv, &oldtv);
	act.sa_handler = IncTimeCnt;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, NULL);
}

// 增加定时器，每隔指定的毫秒时间将执行Action函数，pCtl为Action函数的参数，通常为控件指针
// 1 second == 1000 millisecond
int AddTimerAction(unsigned int ms, int SingleShot, void (*Action)(void* pctl), void* pCtl)
{
	int i;
	for(i=0; i<timer.MaxTimerItem; i++)
	{
		if(timer.pTimerItem[i].isusing == 0)
			break;
	}
	if(i == timer.MaxTimerItem)
	{
		ERROR_X("error! AddTimerAction all Timer are using\n");
		return -1;
	}
	else
	{
		if(pthread_mutex_lock(&timer.TimerMutex) == 0)
		{
			timer.pTimerItem[i].ms = (ms*160)/1000;
			timer.pTimerItem[i].SingleShot = SingleShot;
			timer.pTimerItem[i].Action = Action;
			timer.pTimerItem[i].pTimerCtl = pCtl;
			timer.pTimerItem[i].isusing = 1;
			timer.pTimerItem[i].Born = timer.TimerCurTime;
			pthread_mutex_unlock(&timer.TimerMutex);
			return i;
		}
	}
	ERROR_X("error! AddTimerAction fail\n");
	return -1;
}

// 重新设置定时器的启动暂停
int TimerSet(int TimerID, int TimerStartStop)
{
	if(TimerID < timer.MaxTimerItem)
	{
		if(pthread_mutex_lock(&timer.TimerMutex) == 0)
		{
			if(TimerStartStop == TIMERSTOP)
				timer.pTimerItem[TimerID].Born = 0;
			else
				timer.pTimerItem[TimerID].Born = timer.TimerCurTime;
			pthread_mutex_unlock(&timer.TimerMutex);
			return 0;
		}
	}
	ERROR_X("error! TimerSet ID:%d not exist\n", TimerID);
	return -1;
}

// 释放定时器
int TimerDel(int* pTimerID)
{
	if((*pTimerID >= 0) && (*pTimerID < timer.MaxTimerItem))
	{
		//INFO_X("TimerDel ID:%d\n", *pTimerID);
		if(pthread_mutex_lock(&timer.TimerMutex) == 0)
		{
			timer.pTimerItem[*pTimerID].isusing = 0;
			*pTimerID = -1;
			pthread_mutex_unlock(&timer.TimerMutex);
			return 0;
		}
	}
	return -1;
}

// 整个定时器系统初始化
void TimerInit(int MaxTimer)
{
//	timer.fd_rtc = open("/dev/rtc0", O_RDONLY);
//	if(timer.fd_rtc < 0)
//		ERROR_X("error! /dev/rtc0 fail\n");
	timer.MaxTimerItem = MaxTimer;
	timer.pTimerItem = (TimerItem_t*)uimalloc(timer.MaxTimerItem*sizeof(TimerItem_t));
	memset(timer.pTimerItem, 0, timer.MaxTimerItem*sizeof(TimerItem_t));
	timer.LastTimeCnt = 0;
	timer.TimerCurTime = 1;
	pthread_mutex_init(&(timer.TimerMutex), NULL);
	pthread_create(&timer.thrdTimer, NULL, (void*(*)(void*))TimerLoop, NULL);
	ActTimer();
}

// 整个定时器系统摧毁，通常不要调用
void TimerDestory(void)
{
	if(timer.pTimerItem != NULL)
		uifree(timer.pTimerItem);
}

#if 0

struct rtc_time read_rtc_time(void)
{
	struct rtc_time rtc;
	int r = ioctl(fd_rtc, RTC_RD_TIME, &rtc);
	if (r != 0) 
	{
		ERROR_X("read_rtc_time error %d, %s\n", errno, strerror(errno));
	}
	return rtc;
#if 0	
	struct tm tm = {
		.tm_sec = rtc.tm_sec,
		.tm_min = rtc.tm_min,
		.tm_hour = rtc.tm_hour,
		.tm_mday = rtc.tm_mday,
		.tm_mon = rtc.tm_mon,
		.tm_year = rtc.tm_year,
		.tm_wday = rtc.tm_wday,
		.tm_yday = rtc.tm_yday,
		.tm_isdst = rtc.tm_isdst,
	};
	time_t time = mktime(&tm);	
	return time;
#endif	
}

int set_rtc_time(void)
{
	// call gettimeofday
	struct timeval nowtime, reftime;
	struct tm new_broken_time;

	gettimeofday(&nowtime, NULL);
	reftime.tv_sec = nowtime.tv_sec;
	reftime.tv_usec = 0;

	new_broken_time = *localtime((time_t *)&reftime.tv_sec);
	struct rtc_time rtc_time;
	rtc_time.tm_sec = new_broken_time.tm_sec;
	rtc_time.tm_min = new_broken_time.tm_min;
	rtc_time.tm_hour = new_broken_time.tm_hour;
	rtc_time.tm_mday = new_broken_time.tm_mday;
	rtc_time.tm_mon = new_broken_time.tm_mon;
	rtc_time.tm_year = new_broken_time.tm_year;
	rtc_time.tm_wday = new_broken_time.tm_wday;
	rtc_time.tm_yday = new_broken_time.tm_yday;
	rtc_time.tm_isdst = new_broken_time.tm_isdst;
	
	int r = ioctl(fd_rtc, RTC_SET_TIME, &rtc_time);
	if (r != 0) {
		ERROR_X("sys_to_rtc error %d, %s\n", errno, strerror(errno));
		return -1;
	}
	return 0;
}

#endif

