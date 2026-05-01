#ifndef _MISC_H_
#define _MISC_H_

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

#ifdef USE_TRACE
#define PRNCH TRACE("PRNCH: %s", prnch(crnt_char()))
#else
#define PRNCH
#endif

#endif /* _MISC_H_ */
