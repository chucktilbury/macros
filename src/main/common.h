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

#include "define.h"
#include "ifelse.h"
#include "import.h"

void consume_space(void);
string_t* process_name(void);

#endif /* _COMMON_H_ */
