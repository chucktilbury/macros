#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <stdio.h>
#include <stdbool.h>
#include "string_buffer.h"
#include "char_buffer.h"

#define EOI -2
#define EOL '\n'

typedef struct file_t {
    char_buffer_t* buffer;
    struct file_t* next;
    bool is_open;
    int depth;
} file_t;

#define MAX_DEPTH 16

void open_file(string_t* fname);
void close_file(void);
const char* find_file(const char* fname);

#endif /* _FILEIO_H_ */
