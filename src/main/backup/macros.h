#ifndef _MACROS_H_
#define _MACROS_H_

#define PRNCH TRACE("char: '%s':%d:%d", prnch(get_char()), LINE_NO, COL_NO)

#include "common.h"
#include "symbols.h"
#include "macros.h"
#include "define.h"
#include "ifelse.h"
#include "import.h"
#include "process.h"

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

#define IS_EOF (get_char() == EOF)
#define IS_EOI (get_char() == EOI)
#define IS_REFERENCE (get_char() == '@')
#define IS_DIRECTIVE (get_char() == '.')
#define IS_COMMENT (get_char() == '/')

#endif /* _MACROS_H_ */
