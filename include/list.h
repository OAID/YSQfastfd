#ifndef LIST_H__
#define LIST_H__

struct list_head
{
	struct list_head *prev , *next;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name)		\
	struct list_head name = LIST_HEAD_INIT(name);

#define INIT_LIST_HEAD(ptr)							\
	do{												\
		(ptr)->prev = (ptr); (ptr)->next = (ptr);	\
	}while(0)

static inline void __list_add(struct list_head *node,
		struct list_head *prev, struct list_head *next)
{
	node->prev = prev;
	node->next = next;
	next->prev = node;
	prev->next = node;
}

static inline void __list_del(struct list_head *prev,
			struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_add(struct list_head *node,
			struct list_head * head)
{
	__list_add(node, head, head->next);
}

static inline void list_add_tail(struct list_head *node,
			struct list_head *head)
{
	__list_add(node, head->prev, head);
}

static inline void list_del(struct list_head *entry)
{
	if ((!entry->prev) || (!entry->next))
		return;

	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

#define list_for_each(pos, head)                    \
 	for(pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)            \
		for (pos = (head)->next, n = pos->next;     \
			pos != (head); pos = n, n = pos->next)   	

#if 0
#define offsetof(type, member)        \
	((size_t)&((type *)0)->member)
#endif

#define list_entry(ptr, type, member) \
({ \
 	const typeof(((type *)0)->member) *_mptr = (ptr); \
	(type *)((unsigned long)_mptr - (unsigned long)(&((type *)0)->member)); \
})

#endif
