#include "ctlroot.h"
#include "flagdefine.h"
#include "temperature.h"
#include "log.h"
#include "msgmanage.h"

TempManage tempmanage;

void Temp_Open(void)
{
	char *buf = "1";

	if((tempmanage.tempenablefd=open("/sys/bus/i2c/devices/1-0040/temp_enable",O_RDWR))<0)
	{
		ERROR_X("error! can not open /sys/bus/i2c/devices/1-0040/temp_enable\n");
		return;
	}

	write(tempmanage.tempenablefd, buf, strlen(buf)+1);

	close(tempmanage.tempenablefd);
}

void Temp_Close(void)
{
	char *buf = "0";

	if((tempmanage.tempenablefd=open("/sys/bus/i2c/devices/1-0040/temp_enable",O_RDWR))<0)
	{
		ERROR_X("error! can not open /sys/bus/i2c/devices/1-0040/temp_enable\n");
		return;
	}

	write(tempmanage.tempenablefd, buf, strlen(buf)+1);

	close(tempmanage.tempenablefd);
}


void Temp_Loop(void)
{
	for(;;)
	{
		char temp[10];
		int temp_mc;
		if((tempmanage.tempfd=open("/sys/bus/i2c/devices/1-0040/temp1_input",O_RDONLY))<0)
		{
			ERROR_X("error! can not open /sys/bus/i2c/devices/1-0040/temp1_input\n");
			return;
		}

		read(tempmanage.tempfd, &temp, sizeof(temp));

		close(tempmanage.tempfd);

		temp_mc = atoi(temp);

		//INFO_X("get temp = %d\n", temp_mc);

		tempmanage.root->interior_temperature = temp_mc/1000;

		//INFO_X("get temp = %d\n", temp_mc);
		//INFO_X("get temp = %d\n", tempmanage.root->interior_temperature);

		if(tempmanage.on_temp_changed != NULL){
			//callback
			tempmanage.on_temp_changed(temp_mc);
		}

		memset(&temp, 0, sizeof(temp));
		
		usleep(tempmanage.interval * 1000);
	}
}

int TempManageInit(CtlRoot* proot, int interval_ms, int (*temp_changed_callback)(int))
{
	INFO_X("TempManageInit\n");

	tempmanage.root = proot;
	tempmanage.interval = interval_ms;

	if(tempmanage.interval <= 0){
		tempmanage.interval = 1000;
	}

	tempmanage.on_temp_changed = temp_changed_callback;

	Temp_Open();

	if(pthread_create(&(tempmanage.g_thrdTemp),NULL,(void*(*)(void*))(Temp_Loop),NULL))
	{
		ERROR_X("error! Temp_Thread_Init fail\n");
		return -1;
	}
	INFO_X("TempManageInit\n");
	return 0;
}

