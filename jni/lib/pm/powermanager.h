#ifndef _POWERMANAGER_H_
#define _POWERMANAGER_H_

#include "ctlroot.h"


#define KEY_POWER		116	/* SC System Power Down */

#define DELAY_SLEEP_TIME	10 * 60 //5

struct PowerManager
{
	int pm_input_fd;
	int pm_state_fd;
	CtlRoot* root;
	pthread_t g_thrdTemp;
	pthread_t g_thrdWakelock;
	pthread_t g_thrdDelaySleep;
};

void Power_Loop(void);

int PowerManagerInit(CtlRoot* proot);

void cpu_sleep(void);
void cpu_wakeup(void);

void set_cpu_wakeup_interval(unsigned char sec);
int get_lcd_status(void);



#endif
