#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "fileio.h"
#include "alloc.h"
#include "error.h"
#include "string_buf.h"
#include "trace.h"
#include "macros.h"
#include "symbols.h"
#include "pointer_list.h"
#include "string_list.h"
#include "cmdline.h"
#include "chario.h"

#include "define.h"
#include "ifelse.h"
#include "import.h"

typedef enum {
    IF_DIRECTIVE,
    ELSE_DIRECTIVE,
    DEFINE_DIRECTIVE,
    IMPORT_DIRECTIVE,
    NOT_A_DIRECTIVE,
} directive_type_t;

void consume_space(void);
directive_type_t process_directive_type(string_t* s);
string_t* process_word(void);
string_t* process_name(void);
string_t* process_number(void);
void test_end_of_file(void);
void test_end_of_input(void);
void test_end(void);

#endif /* _COMMON_H_ */
