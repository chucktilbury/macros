
#include "common.h"

int warnings = 0;
int errors = 0;
void dump_symbol_table(void);


void warning(const char* fmt, ...) {

    if(FILE_NAME != NULL)
        fprintf(stderr, "warning: %s:%d:%d: ", FILE_NAME, LINE_NO, COL_NO);
    else
        fprintf(stderr, "warning: ");

    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
    warnings++;
}

void error(const char* fmt, ...) {

    if(FILE_NAME != NULL)
        fprintf(stderr, "error: %s:%d:%d: ", FILE_NAME, LINE_NO, COL_NO);
    else
        fprintf(stderr, "error: ");

    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
#ifdef USE_TRACE
    if(verbosity >= DEFAULT_TRACE) {
        dump_input_buffer();
        dump_output_buffer();
        dump_symbol_table();
    }
#endif
    exit(1);
}

void consume_error(const char* where) {

    if(FILE_NAME != NULL)
        fprintf(stderr, "error: %s:%d:%d: expected %s but got ",
                FILE_NAME, LINE_NO, COL_NO, where);
    else
        fprintf(stderr, "error: expected %s but got ", where);

    string_t* tmp = create_string(NULL);
    for(int ch = crnt_char(), i = 0; i < 20 && ch != '\n'; i++) {
        append_string_char(tmp, ch);
        advance_char();
        ch = crnt_char();
    }
    fprintf(stderr, "%s\n", tmp->buffer);
    destroy_string(tmp);

    exit(1);
}

int get_errors(void) {
    return errors;
}

int get_warnings(void) {
    return warnings;
}

/*
    If the current char is in the str then pass, otherwise,
    publish an error. Returns true if character was found.
*/
int expect_char(const char* str) {

    int ch = crnt_char();
    if(!strchr(str, ch)) {
        if(strlen(str) > 1)
            error("expected one of \"%s\" but got \"%c\" (0x%02X)", str, ch, ch);
        else
            error("expected a \'%c\' but got \"%c\" (0x%02X)", str[0], ch, ch);
    }

    return ch;
}

static inline void _pad(int num) {

    for(int i = 0; i < num; i++)
        putc('-', stdout);
}

void print_legend(const char* str) {

    if(str != NULL) {
        int len = strlen(str);
        _pad((80-(len+2))/2);
        printf(" %s ", str);
        _pad((80-(len+2))/2 + ((!(len & 0x01))? 0: 1));
        putc('\n', stdout);
    }
    else {
        _pad(80);
        putc('\n', stdout);
    }
}

