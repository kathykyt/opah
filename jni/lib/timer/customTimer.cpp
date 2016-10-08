#include "customTimer.h"

CustomTimer::CustomTimer(void) {
	time_t timeT;
	struct tm *timeP;
	timeT = time(NULL);
	timeP = localtime(&timeT);
	year = timeP->tm_year;
	month = timeP->tm_mon;
	day = timeP->tm_mday;
	hour = timeP->tm_hour;
	min = timeP->tm_min;
}

CustomTimer::~CustomTimer(void) {

}

int CustomTimer::GetHour(void) {
    return hour;
}

int CustomTimer::GetMin(void) {
    return min;
}

int CustomTimer::GetYear(void) {
    return year;
}

int CustomTimer::GetMonthDay(void) {
    return month * 100 + day;
}

