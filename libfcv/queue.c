/*
  Copyright (C) 2017 Open Intelligent Machines Co.,Ltd
 
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Authour : Haibing Xu <hxu@openailab.com>
*/

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <queue.h>

int ringqueue_init(struct ringqueue *q, int size)
{
	if (size <= 0) {
		return -EINVAL;
	}

	q->item = calloc(size, sizeof(struct rqitem));
	if (q->item == NULL) {
		return -ENOMEM;
	}

	q->size = size;
	q->head = 0;
	q->tail = 0;

	return 0;
}

int rq_enqueue(struct ringqueue *q, struct rqitem item)
{
	if (rq_isfull(q)) {
		return -EINVAL;
	}

	q->item->data = item.data;
	q->tail = (q->tail + 1) % q->size;
	return 0;
}

int rq_dequeue(struct ringqueue *q, struct rqitem *item)
{
	if (rq_isempty(q)) {
		return -EINVAL;
	}

	item->data = q->item->data;
	q->head = (q->head + 1) % q->size;

	return 0;
}

void ringqueue_destroy(struct ringqueue *q)
{
	if (q->item != NULL) {
		free(q->item);
	}
}
