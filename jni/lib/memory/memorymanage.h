#ifndef _MEMORYMANAGE_H_
#define _MEMORYMANAGE_H_

#include <pthread.h>

struct __mem__
{
	struct __mem__  *next;    /* single-linked list */
	unsigned int    len;      /* length of following block */
};

typedef struct __mem__ __memt__;
typedef __memt__ *__memp__;

// 20M used to malloc in project app
#define MALLOC_MEM_SIZE (1024*1024*20)
#define	HLEN	(sizeof(__memt__))
#define AVAIL	__mem_avail__[0]
#define MIN_POOL_SIZE	(HLEN * 10)
#define MIN_BLOCK	(HLEN * 4)

#pragma pack(4)
struct MemoryManage
{
	unsigned char MemPool[MALLOC_MEM_SIZE];
	__memt__ __mem_avail__[2];
	pthread_mutex_t MemoryMutex;
};
#pragma pack()

void MemoryInit(void);
void* uimalloc(unsigned int size);
void uifree(void* pMem);
unsigned int RemainBuffer(void);
                                                                                                  
#endif
