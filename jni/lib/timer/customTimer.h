#ifndef _CUSTOMTIMER_H_
#define _CUSTOMTIMER_H_

#include <time.h>

class CustomTimer
{
    private:
	protected:
		int year;
		int month;
		int day;
		int hour;
		int min;

    public:
		CustomTimer(void);
		~CustomTimer(void);
		int GetHour(void);
		int GetMin(void);
		int GetYear(void);
		int GetMonthDay(void);		
};

#endif
