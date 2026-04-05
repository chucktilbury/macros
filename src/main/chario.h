#ifndef _CHARIO_H_
#define _CHARIO_H_

#include <stddef.h>
#include "string_buf.h"

typedef struct {
    char* buffer;
    size_t len;
    size_t cap;
    size_t index;
    int ch;
    int line;
    int col;
    string_t* fname;
} char_buffer_t;

#define FILE_NAME (get_char_buffer() ? raw_string(get_file_name()) : NULL)
#define LINE_NO get_line_no()
#define COL_NO get_col_no()

char_buffer_t* create_char_buffer(string_t* str);
void destroy_char_buffer(char_buffer_t* buf);
char_buffer_t* get_char_buffer(void);
void append_char_buffer(const char* str);
void append_char_buffer_char(int ch);
void append_char_buffer_str(string_t* str);
void set_char_buffer(char_buffer_t* buf);
int get_char(void);
void consume_char(void);
void unget_string(size_t len);
void read_char_buffer(size_t size, FILE* fp);
void write_char_buffer(FILE* fp);
int get_line_no(void);
int get_col_no(void);
string_t* get_file_name(void);

#endif /* _CHARIO_H_ */
