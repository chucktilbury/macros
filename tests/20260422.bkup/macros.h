#ifndef _MACROS_H_
#define _MACROS_H_

#include "fileio.h"
#include "string.h"
#include "symbols.h"

typedef enum {
    IF_DIRECTIVE,
    ELSE_DIRECTIVE,
    DEFINE_DIRECTIVE,
    IMPORT_DIRECTIVE,
    NOT_A_DIRECTIVE,
} directive_type_t;

void process_file(void);
void test_end_of_file(void);
void test_end_of_input(void);
void test_end(void);
void consume_space(void);

directive_type_t expect_directive(void);
string_t* expect_name(void);
string_t* expect_number(void);
string_t* expect_ref(void);
string_t* expect_literal_string(void);
// int consume_single_line_comment(void);
// int consume_multi_line_comment(void);

#endif /* _MACROS_H_ */
