#include "ctlroot.h"
#include "flagdefine.h"
#include "powermanager.h"
#include "log.h"
#include "msgmanage.h"
#include "wakelock.h"
#include <linux/input.h>


PowerManager powerManager;
static int is_cpu_sleep = 0;
int delay_sleep_time_count = 0;

int get_lcd_status(void)
{
	int status;
	FILE *file = popen("cat /sys/class/axppower/axp_lcd_state","r");
	char tmp[100] = {0};
	if(file != NULL && fgets(tmp, sizeof(tmp), file) != NULL){
		status = atoi(tmp);
	}else{
		status = -1;
	}
	pclose(file);
	return status;
}

void set_cpu_wakeup_interval(unsigned char sec)
{
	INFO_X("set_cpu_wakeup_interval = %d\n", sec);
	char cmd[100];
	sprintf(cmd, "echo %d > /sys/class/fsk/fsk_wakeup_interval", sec);
	system(cmd);
}

void cpu_sleep(void)
{
	INFO_X("cpu_sleep\n");
/*
	char *buf = "mem";

	if((powerManager.pm_state_fd=open("/sys/power/state",O_WRONLY))<0)
	{
		ERROR_X("error! can not open /sys/power/state\n");
		return;
	}

	int ret ;
	ret = write(powerManager.pm_state_fd, buf, strlen(buf));
	INFO_X("cpu_sleep ret = %d\n", ret);
	
	close(powerManager.pm_state_fd);
*/
	system("echo mem > /sys/power/state");
}

void cpu_wakeup(void)
{
	INFO_X("cpu_wakeup\n");
/*
	char *buf = "on";

	if((powerManager.pm_state_fd=open("/sys/power/state",O_WRONLY))<0)
	{
		ERROR_X("error! can not open /sys/power/state\n");
		return;
	}

	write(powerManager.pm_state_fd, buf, strlen(buf)+1);

	close(powerManager.pm_state_fd);
*/
	system("echo on > /sys/power/state");
}


void Power_Loop(void)
{
	struct input_event data;

	for(;;)
	{
		if((powerManager.pm_input_fd=open("/dev/input/event1",O_RDONLY))<0)
		{
			ERROR_X("error! can not open /dev/input/event1\n");
			return;
		}

		read(powerManager.pm_input_fd, &data, sizeof(data));
		
		switch(data.type)
		{
			case EV_KEY:
				//INFO_X("data.code = %d\n", data.code);
				//INFO_X("data.value = %d\n", data.value);
				if(data.code==KEY_POWER){
					if(data.value){
						if(is_cpu_sleep){
							is_cpu_sleep = 0;
							//cpu_wakeup();
						}else{
							is_cpu_sleep = 1;
							//cpu_sleep();
						}
					}
				}
				break;
			default:
				break;
		}


		close(powerManager.pm_input_fd);

		memset(&data, 0, sizeof(data));
		
		//usleep(1 * 1000);
	}
}

void Wakelock_Loop(void)
{
	for(;;)
	{
		if(get_pm_wakelock())
		{
			system("echo 1 > /sys/class/axppower/axp_set_wakelock");
		}
		else
		{
			system("echo 0 > /sys/class/axppower/axp_set_wakelock");
		}
		usleep(10 * 1000);
	}
}

void delay_sleep_loop(void)
{
	for(;;)
	{
		usleep(1000 * 965);//1s
		//sleep(1);
		//INFO_X("delay_sleep_time_count = %d\n", delay_sleep_time_count);
		if(get_lcd_status() == 1){
			delay_sleep_time_count ++;
			if(delay_sleep_time_count >= DELAY_SLEEP_TIME){
				cpu_sleep();
			}
		}else{
			delay_sleep_time_count = 0;
		}
	}
}


int PowerManagerInit(CtlRoot* proot)
{
	INFO_X("PowerManagerInit\n");

	powerManager.root = proot;

	if(pthread_create(&(powerManager.g_thrdTemp),NULL,(void*(*)(void*))(Power_Loop),NULL))
	{
		ERROR_X("error! powerManager_Thread_Init fail\n");
		return -1;
	}

	if(pthread_create(&(powerManager.g_thrdWakelock),NULL,(void*(*)(void*))(Wakelock_Loop),NULL))
	{
		ERROR_X("error! powerManager_Thread_Init fail\n");
		return -1;
	}
	
	if(pthread_create(&(powerManager.g_thrdDelaySleep),NULL,(void*(*)(void*))(delay_sleep_loop),NULL))
	{
		ERROR_X("error! powerManager_Thread_Init fail\n");
		return -1;
	}

	//enable power key
	//system("echo 1 > /sys/class/axppower/axp_power_key");
	system("echo 1 > /sys/class/cap1133/power_key");

	//set interval test
	//set_cpu_wakeup_interval(30);

	INFO_X("PowerManagerInit\n");
	return 0;
}

