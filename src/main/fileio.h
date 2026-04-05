#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <stdio.h>
#include <stdbool.h>
#include "string_buf.h"
#include "chario.h"

typedef struct file_t {
    char_buffer_t* buffer;
    struct file_t* next;
    bool is_open;
} file_t;

void open_file(string_t* fname);
void close_file(void);
// int get_char(void);
// void consume_char(void);
// void unget_string(string_t* s);

// int get_line_no(void);
// int get_col_no(void);
// string_t* get_file_name(void);

#endif /* _FILEIO_H_ */
