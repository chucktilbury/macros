
#include "alloc.h"
#include "queue.h"

queue_t* create_queue(void) {

    queue_t* ptr = _ALLOC_TYPE(queue_t);

    return ptr; // memory already cleared by _ALLOC_*()
}

void add_queue(queue_t* que, void* data) {

    queue_element_t* ptr = _ALLOC_TYPE(queue_element_t);
    ptr->data = data;

    if(que->tail != NULL)
        que->tail->next = ptr;
    else {
        que->head = ptr;
        que->crnt = ptr;
    }
    que->tail = ptr;
}

void* post_queue(queue_t* que) {

    return (void*)que->crnt;
}

void reset_queue(queue_t* que, void* post) {

    que->crnt = (queue_element_t*)post;
}

void* consume_queue(queue_t* que) {

    // assumes GC is active
    que->head = que->crnt;
    return que->head->data;
}

void* advance_queue(queue_t* que) {

    if(que->crnt->next != NULL) {
        que->crnt = que->crnt->next;
        return que->crnt->data;
    }
    else
        return NULL;
}

void* peek_queue(queue_t* que) {

    if(que->crnt != NULL)
        return que->crnt->data;
    else
        return NULL;
}

size_t len_queue(queue_t* que) {

    size_t count = 0;

    for(queue_element_t* ptr = que->head; ptr != NULL; ptr = ptr->next)
        count++;

    return count;
}


