#ifndef _FIND_FILE_H_
#define _FIND_FILE_H_

#include <stdio.h>
#include <stdbool.h>
#include "string_buffer.h"
#include "output_buffer.h"

#if 0
typedef struct file_t {
    output_buffer_t* buffer;
    struct file_t* next;
    bool is_open;
    int depth;
} file_t;

void open_file(string_t* fname);
void close_file(void);
#endif

const char* find_file(const char* fname);

#endif /* _FIND_FILE_H_ */
