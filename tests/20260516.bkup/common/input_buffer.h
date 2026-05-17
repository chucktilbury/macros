#ifndef _INPUT_BUFFER_H_
#define _INPUT_BUFFER_H_

#include <stdbool.h>
#include "string_buffer.h"

void load_input_buffer(string_t* fname);
void push_input_buffer(string_t* str);
void pop_input_buffer(void);
int crnt_char(void);
int advance_char(void);
void unget_char(void);
void unget_string(int len);

string_t* get_file_name(void);
int get_line_no(void);
int get_col_no(void);
bool valid_input_buffer(void);
void dump_input_buffer(void);

#define FILE_NAME raw_string(get_file_name())
#define LINE_NO get_line_no()
#define COL_NO get_col_no()

#define EOI -2
#define EOL '\n'

#endif /* _INPUT_BUFFER_H_ */
