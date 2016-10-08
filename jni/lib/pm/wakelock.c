
#include "wakelock.h"


#if __cplusplus
extern "C" {
#endif

static int pm_wake_lock_flag = 0;


int get_delta_time(long currTime, long lastTime)
{

   if (currTime >= lastTime) {
     return (currTime - lastTime);
   }
   else 
   {
	//we are wrapping around?)
	printf("error: we have not take care the time eclipse wrap around issue\n");
	return 0;  //this is not correct, need some time to think.
	
   }	


}

/*
==========================================================================
   This function can set the bit flag asking system not to sleep
   on_off = 1 need to lock, don't sleep
   on_off = 0, can go sleep
==========================================================================
*/
void pm_wake_lock(int flags, int on_off) {

    //update the pm_wake_lock_flag
    pm_wake_lock_flag &= ~(1 << flags);
    pm_wake_lock_flag |= ((on_off & 0x1) << flags);


    if (pm_wake_lock_flag == 0) {
	//printf("system is able to sleep\n");
    }

}



int get_pm_wakelock(void) {

    return pm_wake_lock_flag;


}


#if __cplusplus
};
#endif


