#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#include <pthread.h>

#define TSXMIRROR		0
#define TSYMIRROR		0

#define SYN_REPORT      0  
#define SYN_CONFIG      1  
#define SYN_MT_REPORT   2  
#define ABS_MT_TOUCH_MAJOR 0x30 /* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR 0x31 /* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR 0x32 /* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR 0x33 /* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION 0x34 /* Ellipse orientation */
#define ABS_MT_POSITION_X 0x35 /* Center X ellipse position */
#define ABS_MT_POSITION_Y 0x36 /* Center Y ellipse position */
#define ABS_MT_TOOL_TYPE 0x37 /* Type of touching device */
#define ABS_MT_BLOB_ID 0x38 /* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID 0x39 /* Unique ID of initiated contact */
#define ABS_MT_PRESSURE 0x3a /* Pressure on contact area */

struct TouchManage
{
	int touchfd;
	pthread_t g_thrdTouch;
};

void Touch_Loop(void);
void TouchManageInit(void);

#endif
