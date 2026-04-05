#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <stdio.h>
#include <stdbool.h>
#include "string_buf.h"

typedef struct file_t {
    string_t* name;
    //FILE* fp;
    string_t* buffer;
    size_t index;
    size_t size;
    int line;
    int col;
    struct file_t* next;
    int ch;
    bool is_open;
} file_t;

void open_file(string_t* fname);
void close_file(void);
int get_char(void);
void consume_char(void);
void unget_string(string_t* s);

int get_line_no(void);
int get_col_no(void);
string_t* get_file_name(void);

#endif /* _FILEIO_H_ */
