#include "log.h"
#include "flagdefine.h"
#include "memorymanage.h"

MemoryManage memorymanage;

// 将整个数组作为内存池，大小为MALLOC_MEM_SIZE，方便内存监控，避免内存碎片
void MemoryInit(void)
{
	pthread_mutex_init(&(memorymanage.MemoryMutex), NULL);
	memorymanage.AVAIL.next = (struct __mem__ *)(memorymanage.MemPool);
	memorymanage.AVAIL.len  = MALLOC_MEM_SIZE;
	(memorymanage.AVAIL.next)->next = NULL;
	(memorymanage.AVAIL.next)->len  = MALLOC_MEM_SIZE - HLEN;
}

// 分配内存
void* uimalloc(unsigned int size)
{
	__memp__ q;			/* ptr to free block */
	__memp__ p;			/* q->next */
	unsigned int k;		/* space remaining in the allocated block */

	if(0 == pthread_mutex_lock(&(memorymanage.MemoryMutex)))
	{
		if(size&0x03)
			size = (size&0xFFFFFFFC)+4;
		q = &(memorymanage.AVAIL);		/* q is the pointer to the next available block*/

		while (1)
		{
			if((p = q->next) == NULL)	/* P points to the next block is NULL, end of list */
			{
				pthread_mutex_unlock(&(memorymanage.MemoryMutex));
				return NULL;			/* FAILURE */
    		}
		    if (p->len >= size)			/* Found Space */
		      break;
		    q = p;
  		}

  		k = p->len - size;		/* calculate remaining bytes in block */
  		if(k < MIN_BLOCK)		/* bytes too small for new block */
  		{
    		q->next = p->next;
			pthread_mutex_unlock(&(memorymanage.MemoryMutex));
    		return (&p[1]);		/* SUCCESS */
  		}

  		k -= HLEN;				/* split P into two blocks */
  		p->len = k;
  		q = (__memp__ )(((unsigned char*) (&p [1])) + k);
  		q->len = size;
  		pthread_mutex_unlock(&(memorymanage.MemoryMutex));
  		return (&q[1]);			/* SUCCESS */
	}
	return NULL;
}

// 释放内存
void uifree(void* pMem)
{
	__memp__ q;		/* ptr to free block */
	__memp__ p;		/* q->next */
	__memp__ p0;	/* block to free */

	if(0 == pthread_mutex_lock(&(memorymanage.MemoryMutex)))
	{
		if((pMem == NULL) || (memorymanage.AVAIL.len == 0))
		{
			pthread_mutex_unlock(&(memorymanage.MemoryMutex));
			return;
		}
		p0 = (__memp__)pMem;
		p0 = &p0[-1];					/* get address of header */
		q = &(memorymanage.AVAIL);		/* Location of first available block */

		while (1)
		{
			p = q->next;
			if ((p == NULL) || ((void *)p > pMem))
				break;
			q = p;
		}

		if((p != NULL) && ((((unsigned char *)pMem) + p0->len) == (unsigned char *)p))		/* merge block */
		{
			p0->len += p->len + HLEN;
			p0->next = p->next;
		}
		else
		{
			p0->next = p;
		}

		if((((unsigned char *)q) + q->len + HLEN) == (unsigned char *)p0)	/* merge block */
  		{
			q->len += p0->len + HLEN;
			q->next = p0->next;
		}
		else
		{
			q->next = p0;
		}

		pthread_mutex_unlock(&(memorymanage.MemoryMutex));
	}
}

// 内存信息打印，可以监控是否有碎片产生
unsigned int RemainBuffer(void)
{
  __memp__ q;			/* ptr to free block */
  __memp__ p;			/* q->next */
  unsigned int k;		/* space remaining in the allocated block */
		
  q = &(memorymanage.AVAIL);
  p = q->next;
  k = p->len ;			/* calculate remaining MAX bytes in block */
  return(k);
}
