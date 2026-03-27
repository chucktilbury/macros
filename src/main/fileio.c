
#include "common.h"

void open_file(string_t* fname) {

    ASSERT(fname != NULL, "file name required");
    ENTER;

    FILE* fp;
    if(NULL == (fp = fopen(fname->buf, "r")))
        FATAL("cannot open input file: %s: %s", fname->buf, strerror(errno));

    TRACE(0, "opening file: %s", fname->buf);
    file_t* f = _ALLOC_TYPE(file_t);
    f->fp = fp;
    f->name = copy_string(fname);
    f->line = 1;
    f->col = 1;
    f->is_open = true;

    if(file_stack != NULL) {
        TRACE(0, "push file stack");
        f->next = file_stack;
    }
    file_stack = f;

    f->ch = consume_char();
    RETURN();
}

void close_file(void) {

    ASSERT(file_stack != NULL, "attempt to close a file but none have been opend");
    ASSERT(file_stack->is_open == true, "attempt to close a file that has already been closed");
    ENTER;

    file_t* f = file_stack;
    TRACE(0, "closing file: \"%s\"", f->name->buf);
    fclose(f->fp);
    f->is_open = false;

    // pop the stack but do not destroy the first node
    if(f->next != NULL) {
        TRACE(0, "pop file stack");
        file_stack = f->next;
        destroy_string(f->name);
        _FREE(f);
    }

    RETURN();
}

int get_char(void) {

    return file_stack->ch;
}

int consume_char(void) {

    ASSERT(file_stack != NULL, "attempt to read char but no file has been opend");

    if(file_stack->is_open) {
        if(file_stack->ch != EOF) {
            if(file_stack->ch == EOL) {
                file_stack->line++;
                file_stack->col = 1;
            }
            else
                file_stack->col++;

            file_stack->ch = fgetc(file_stack->fp);
        }
    }
    else
        file_stack->ch = EOI;

    return file_stack->ch;
}

int get_line_no(void) {

    ASSERT(file_stack != NULL, "no file has been opend");
    return file_stack->line;
}

int get_col_no(void) {

    ASSERT(file_stack != NULL, "no file has been opend");
    return file_stack->col;
}

string_t* get_file_name(void) {

    ASSERT(file_stack != NULL, "no file has been opend");
    return file_stack->name;
}
