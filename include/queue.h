#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <oaid_base.h>

namespace OAID {

#define rq_isfull(q) (((q)->tail + 1) % (q)->size == (q)->head)

#define rq_isempty(q)  ((q)->tail == (q)->head)

class queue : public oaid_base
{
public:
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

public:
    queue();

    void ringqueue_destroy(struct ringqueue *q);
    int rq_dequeue(struct ringqueue *q, struct rqitem *item);
    int rq_enqueue(struct ringqueue *q, struct rqitem item);
    int ringqueue_init(struct ringqueue *q, int size);
};

}

#endif // QUEUE_H
