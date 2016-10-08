#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "ctlroot.h"

struct TempManage
{
	int interval;
	int tempfd;
	int tempenablefd;
	int temp_mc;
	int (*on_temp_changed)(int);
	CtlRoot* root;
	pthread_t g_thrdTemp;
};

void Temp_Loop(void);

int TempManageInit(CtlRoot* proot, int interval_ms, int (*temp_changed_callback)(int));

#endif
