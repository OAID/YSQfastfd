#include "queue.h"

namespace OAID {

queue::queue()
{

}

int queue::ringqueue_init(struct ringqueue *q, int size)
{
    if (size <= 0) {
        return -EINVAL;
    }

    q->item = (struct rqitem *)calloc(size, sizeof(struct rqitem));
    if (q->item == NULL) {
        return -ENOMEM;
    }

    q->size = size;
    q->head = 0;
    q->tail = 0;

    return 0;
}

int queue::rq_enqueue(struct ringqueue *q, struct rqitem item)
{
    if (rq_isfull(q)) {
        return -EINVAL;
    }

    q->item->data = item.data;
    q->tail = (q->tail + 1) % q->size;
    return 0;
}

int queue::rq_dequeue(struct ringqueue *q, struct rqitem *item)
{
    if (rq_isempty(q)) {
        return -EINVAL;
    }

    item->data = q->item->data;
    q->head = (q->head + 1) % q->size;

    return 0;
}

void queue::ringqueue_destroy(struct ringqueue *q)
{
    if (q->item != NULL) {
        free(q->item);
    }
}

}
