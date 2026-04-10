#ifndef _CHAR_BUFFER_H_
#define _CHAR_BUFFER_H_

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

// for error messages
#define FILE_NAME (get_input_buffer() ? raw_string(get_file_name(get_input_buffer())) : NULL)
#define LINE_NO get_line_no(get_input_buffer())
#define COL_NO get_col_no(get_input_buffer())

#define EMITC(c) append_char_buffer_char(c)
#define EMITS(s) append_char_buffer_str(s)
#define EMITR(s) append_char_buffer(s)

char_buffer_t* create_char_buffer(const char* fname);
void destroy_char_buffer(char_buffer_t* buf);
size_t get_char_buffer_index(char_buffer_t* buf);
string_t* get_file_name(char_buffer_t* buf);
int get_line_no(char_buffer_t* buf);
int get_col_no(char_buffer_t* buf);
char_buffer_t* copy_char_buffer(char_buffer_t* buf);
void dump_char_buffer(char_buffer_t* buf);

// input functions
void set_input_buffer(char_buffer_t* buf);
char_buffer_t* get_input_buffer(void);
int get_char(void);
void consume_char(void);
void unget_string(size_t len);

// output functions
char_buffer_t* set_output_buffer(const char* fname);
char_buffer_t* get_output_buffer(void);
void append_char_buffer(const char* str);
void append_char_buffer_char(int ch);
void append_char_buffer_str(string_t* str);
void insert_char_buffer(size_t index, string_t* str);
void contract_char_buffer(size_t index, size_t len);
void replace_char_buffer(string_t* find, string_t* repl);
size_t search_char_buffer(int ch);

// file io
char_buffer_t* read_char_buffer(string_t* fname);
void write_char_buffer(char_buffer_t* buf);

#endif /* _CHAR_BUFFER_H_ */
