#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stddef.h>

typedef struct _queue_element_t_ {
    void* data;
    struct _queue_element_t_* next;
} queue_element_t;

typedef struct _queue_t_ {
    queue_element_t* head;
    queue_element_t* tail;
    queue_element_t* crnt;
} queue_t;

queue_t* create_queue(void);
void add_queue(queue_t* que, void* data);
void* post_queue(queue_t* que);
void reset_queue(queue_t* que, void* post);
void* consume_queue(queue_t* que);
void* advance_queue(queue_t* que);
void* peek_queue(queue_t* que);
size_t len_queue(queue_t* que);

#endif /* _QUEUE_H_ */
