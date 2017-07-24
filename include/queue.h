#ifndef QUEUE_H__
#define QUEUE_H__

#include <stdint.h>

struct rqitem
{
	unsigned long data;
};

struct ringqueue
{
	int size;
	int head;
	int tail;
	struct rqitem *item;
};

#define rq_isfull(q)                           \
	(((q)->tail + 1) % (q)->size == (q)->head)

#define rq_isempty(q)                          \
	((q)->tail == (q)->head)


#endif
