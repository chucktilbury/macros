
#include "common.h"

int warnings = 0;
int errors = 0;

void warning(const char* fmt, ...) {

    if(file_stack != NULL)
        fprintf(stderr, "warning: %s:%d:%d: ",
                file_stack->name->buf, file_stack->line, file_stack->col);
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

    if(file_stack != NULL)
        fprintf(stderr, "error: %s:%d:%d: ",
                file_stack->name->buf, file_stack->line, file_stack->col);
    else
        fprintf(stderr, "error: ");

    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
    exit(1);
}

void consume_error(const char* where) {

    if(file_stack != NULL)
        fprintf(stderr, "error: %s:%d:%d: expected %s but got ",
                file_stack->name->buf, file_stack->line,
                file_stack->col, where);
    else
        fprintf(stderr, "error: expected %s but got ", where);

    string_t* tmp = create_string(NULL);
    for(int ch = get_char(), i = 0; i < 20 && ch != '\n'; i++) {
        append_string_char(tmp, ch);
        ch = consume_char();
    }
    fprintf(stderr, "%s\n", tmp->buf);
    destroy_string(tmp);

    exit(1);
}

int get_errors(void) {
    return errors;
}

int get_warnings(void) {
    return warnings;
}

// if the current char after spaces is in the str then pass, otherwise,
// publish an error. Returns true if character was found.
int expect_char(const char* str) {

    consume_space();
    test_end();

    int ch = get_char();
    if(!strchr(str, ch)) {
        if(strlen(str) > 1)
            error("expected one of \"%s\" but got \"%c\" (0x%02X)", str, ch, ch);
        else
            error("expected a \'%c\' but got \"%c\" (0x%02X)", str[0], ch, ch);
        consume_char(); // consume the error
    }

    return ch;
}

void test_end_of_file(void) {

    if(get_char() == EOF)
        error("unexpected end of file");
}

void test_end_of_input(void) {

    if(get_char() == EOI)
        error("unexpected end of input");
}

void test_end(void) {

    test_end_of_file();
    test_end_of_input();
}
