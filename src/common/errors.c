
#include "common.h"
#include <stdarg.h>

static int errors = 0;
static int warnings = 0;

int get_errors(void) {

    return errors;
}

int get_warnings(void) {

    return warnings;
}

void error(const char* mark, const char* file, int line, int col, const char* fmt, ...) {

    if(file != NULL)
        fprintf(stderr, "%s error: %s:%d:%d: ", mark, file, line, col);
    else
        fprintf(stderr, "%s error: ", mark);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    errors++;
    fputc('\n', stderr);
}

void warning(const char* mark, const char* file, int line, int col, const char* fmt, ...) {

    if(file != NULL)
        fprintf(stderr, "%s warning: %s:%d:%d: ", mark, file, line, col);
    else
        fprintf(stderr, "%s warning: ", mark);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    warnings++;
    fputc('\n', stderr);
}


