#ifndef _TIMERMANAGE_H_
#define _TIMERMANAGE_H_

#include <time.h>
#include "flagdefine.h"
#include "ctlobject.h"

#define INTERVAL_SEC		0
#define INTERVAL_USEC		10000
#define TIMER_STEP			10000

#define TIMERSTOP		0
#define TIMERSTART		1

typedef struct{
	int Second;			// 0-60
	int Minute;			// 0-59
	int Hour;			// 0-23
	int DayOfMonth;		// 1-31
	int Month;			// 0-11 since January
	int Year;			// since 1900
	int DayOfWeek;		// 0-6 since sundy
	int DayOfYear;		// 0-365 since January 1
}EXT_RTC;

typedef struct{
	unsigned int Born;
	unsigned int ms;
	unsigned int SingleShot;
	void (*Action)(void* pctl);
	void* pTimerCtl;
	int isusing;
}TimerItem_t;

struct TimerManage
{
	int fd_rtc;
	int MaxTimerItem;
	unsigned int LastTimeCnt;
	unsigned int TimerCurTime;
	TimerItem_t* pTimerItem;
	pthread_mutex_t TimerMutex;
	pthread_t thrdTimer;
};

int AddTimerAction(unsigned int ms, int SingleShot, void (*Action)(void* pctl), void* pCtl);
int TimerSet(int TimerID, int TimerStartStop);
int TimerDel(int* pTimerID);
void TimerInit(int MaxTimer);
void TimerDestory(void);
int GetMinSec(void);
struct rtc_time read_rtc_time(void);
int set_rtc_time(void);

#endif
