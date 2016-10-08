#include <linux/input.h>
#include "flagdefine.h"
#include "touchscreen.h"
#include "log.h"
#include "msgmanage.h"

TouchManage touchmanage;

void Touch_Loop(void)
{
	int mt_x_pos = 0, mt_y_pos = 0, mt_is_press = 0;
	int x_pos = -1, y_pos = -1, is_press = -1;
	int mt_num = 0;
	int x_prepos = 0, y_prepos = 0;
	struct input_event data;
	Touch_Event tsevent;

	for(;;)
	{
		read(touchmanage.touchfd, &data, sizeof(data));
/*
		printf("touchscreen type = %d, code = %d, value = %d, tv.tv_sec = %d, tv.tv_usec = %d\n", 
			data.type, data.code, data.value, data.time.tv_sec, data.time.tv_usec);
*/
		//INFO_X("touchscreen type = %d, code = %d, value = %d, tv.tv_sec = %d, tv.tv_usec = %d\n",
		//		data.type, data.code, data.value, data.time.tv_sec, data.time.tv_usec);
		// ï¿½Å³ï¿½ï¿½ï¿½Åµï¿½Ó°ï¿½ï¿½
		if((data.time.tv_sec == 0) && (data.time.tv_usec == 0))
			continue;
		switch(data.type)
		{
			case EV_SYN:	//	EV_SYN ---> 0
				if(data.code == SYN_REPORT)
				{
					if(mt_num <= 1)
					{
#ifndef GOLDFISH

#if defined(LCD_ROTATE_90)
						x_pos = (LCDW-1) - mt_y_pos;
						y_pos = mt_x_pos;
#elif defined(LCD_ROTATE_270)
						x_pos = mt_y_pos;
						y_pos = (LCDH-1) - mt_x_pos;
#else
						x_pos = (LCDW-1) - mt_x_pos;
						y_pos = (LCDH-1) - mt_y_pos;
#endif

#else

						x_pos = mt_x_pos;
						y_pos = mt_y_pos;
#endif
						is_press = mt_is_press;
						if(is_press==1){
							//INFO_X("Down, x:%d, y:%d\n", x_pos, y_pos);
							//printf("Down, x:%d, y:%d\n", x_pos, y_pos);
							memset(&tsevent, 0, sizeof(Touch_Event));
							tsevent.event_type = MsgTypeMouseDown;
							tsevent.x = x_pos;
							tsevent.y = y_pos;
							x_prepos = x_pos;
							y_prepos = y_pos;
							memcpy(&(tsevent.tv), &(data.time), sizeof(struct timeval));
							PostSysMsg((Event*)(&tsevent));
							is_press = 2;
						}else if(is_press==2){
							if((x_prepos != x_pos) || (y_prepos != y_pos))
							{
								//INFO_X("Move, x:%d, y:%d\n", x_pos, y_pos);
								//printf("Move, x:%d, y:%d\n", x_pos, y_pos);
								memset(&tsevent, 0, sizeof(Touch_Event));
								tsevent.event_type = MsgTypeMouseMove;
								tsevent.x = x_pos;
								tsevent.y = y_pos;
								x_prepos = x_pos;
								y_prepos = y_pos;
								memcpy(&(tsevent.tv), &(data.time), sizeof(struct timeval));
								PostSysMsg((Event*)(&tsevent));
								is_press = 2;
							}
							else
							{
								//INFO_X(" Same Move, x:%d, y:%d\n", x_pos, y_pos);
							}
						}else if(is_press==0){
							//INFO_X("Up, x:%d, y:%d\n", x_pos, y_pos);
							//printf("Up, x:%d, y:%d\n", x_pos, y_pos);
							memset(&tsevent, 0, sizeof(Touch_Event));
							tsevent.event_type = MsgTypeMouseUp;
							tsevent.x = x_pos;
							tsevent.y = y_pos;
							x_prepos = x_pos;
							y_prepos = y_pos;
							memcpy(&(tsevent.tv), &(data.time), sizeof(struct timeval));
							PostSysMsg((Event*)(&tsevent));
							is_press = -1;
						}
						mt_is_press = is_press;
					}
					mt_num = 0;
				}else if(data.code == SYN_MT_REPORT){
					mt_num++;
				}
				break;
// ¶Ô´¥ÃþÆÁµÄÈ¡Öµ½øÐÐµ÷Õû
			case EV_ABS:		//	EV_ABS ---> 3
				if(data.code==ABS_X){
				    mt_x_pos = data.value;
#if TSXMIRROR					
					mt_x_pos = LCDW-mt_x_pos;
#endif
				}else if(data.code==ABS_MT_POSITION_X){
					mt_x_pos = data.value;
#if TSXMIRROR					
					mt_x_pos = LCDW-mt_x_pos;
#endif
				}else if(data.code==ABS_Y){
				    mt_y_pos = data.value;
#if TSYMIRROR					
					mt_y_pos = LCDH-mt_y_pos;
#endif
				}else if(data.code==ABS_MT_POSITION_Y){
				    mt_y_pos = data.value;
#if TSYMIRROR					
					mt_y_pos = LCDH-mt_y_pos;
#endif
				}
				//printf("EV_ABS x:%d,y:%d\n",mt_x_pos,mt_y_pos);
				break;
			case EV_KEY:		//	EV_KEY ---> 1
				if(data.code==BTN_TOUCH){
					mt_is_press = data.value; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½--1ï¿½ï¿½ï¿½ï¿½ï¿½É¿ï¿½--0
					//printf("EV_KEY mt_is_press:%d\n",mt_is_press);
				}
				break;
			default:
				break;
		}
	}
}

void TouchManageInit(void)
{
#ifdef  OUTPUT_RGB565_FB
    if ((touchmanage.touchfd = open("/dev/input/event0", O_RDONLY)) < 0) {
        if ((touchmanage.touchfd = open("/dev/event0", O_RDONLY)) < 0) {
            ERROR_X("error! can not open /dev/event0 or /dev/input/event0\n");
            return;
        }
    }

#else

    if ((touchmanage.touchfd = open("/dev/input/event2", O_RDONLY)) < 0) {
        if ((touchmanage.touchfd = open("/dev/event2", O_RDONLY)) < 0) {
            ERROR_X("error! can not open /dev/event0 or /dev/input/event0\n");
            return;
        }
    }

#endif

    if (pthread_create(&(touchmanage.g_thrdTouch), NULL, (void *(*)(void *))(Touch_Loop), NULL)) {
        ERROR_X("error! Touch_Thread_Init fail\n");
    }
}

