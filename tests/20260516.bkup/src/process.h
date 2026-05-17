#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "string_buffer.h"

typedef enum {
    NOT_A_DIRECTIVE,
    IF_DIRECTIVE,
    ELSE_DIRECTIVE,
    DEFINE_DIRECTIVE,
    IMPORT_DIRECTIVE,
    MESSAGE_DIRECTIVE,
    ERROR_DIRECTIVE,
} directive_type_t;

void consume_space(void);
void emit_space(void);
void test_end_error(void);
const char* prnch(int ch);
string_t* scan_name(void);
string_t* scan_literal_string(void);
directive_type_t directive_type(string_t* str);

void process_error(void);
void process_message(void);
void process_import(void);
void process_comment(void);
void process_directive(void);
void process_input(void);

#ifdef USE_TRACE
#define PRNCH TRACEX(60, "PRNCH: '%s':%s:%d:%d", prnch(crnt_char()), raw_string(get_file_name()), get_line_no(), get_col_no())
#else
#define PRNCH
#endif

#endif /* _PROCESS_H_ */
