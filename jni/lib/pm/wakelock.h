#ifndef WAKELOCK_H
#define WAKELOCK_H

#define WIFI_MGR_FLAG		1
#define WIFI_RX_LOOP_FLAG	2
#define GMMP_TCP_RX_FLAG	3
#define GMMP_HB_FLAG		4
#define GMMP_PR_FLAG		5	//periodic report thread
#define UI_EVENT_LOOP_FLAG	6
#define DISPLAY_FLAG		7
#define GMMP_PROCESS_FLAG	8
#define SMARTHOME_PROC_FLAG	9


#define HW_PERIODIC_WAKEUP_SEC	10   //in sec


#if __cplusplus
extern "C" {
#endif

extern void pm_wake_lock(int flags, int on_off);
extern int get_pm_wakelock(void);


#if __cplusplus
};
#endif


#endif

