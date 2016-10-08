#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <android/log.h>

#define INFO_UIOS
#define WARN_UIOS
#define ERROR_UIOS

#define nativeLog(fmt...)    __android_log_print(ANDROID_LOG_INFO, "starfish", fmt) 

#ifdef INFO_UIOS
#define INFO_X(fmt...)   \
	    do {\
	        printf("[%s]#%d: ", __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\r\n"); \
	       }while(0)

#else
#define INFO_X(fmt...)	
#endif 

#ifdef WARN_UIOS
#define WARN_X(fmt...)   \
	    do {\
	        printf("[%s]#%d: ", __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\r\n"); \
	       }while(0)

#else
#define WARN_X(fmt...)	
#endif 

#ifdef ERROR_UIOS
#define ERROR_X(fmt...)   \
	    do {\
	        printf("[%s]#%d: ", __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\r\n"); \
	       }while(0)

#else
#define ERROR_X(fmt...)	
#endif

#endif
