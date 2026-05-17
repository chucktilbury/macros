#include "common.h"
#include <assert.h>

// static string_t* buffer = NULL;
static pointer_list_t* stack = NULL;
// #define BUFFER ((stack->len>0)?((string_t*)(stack->buffer[stack->len-1])):NULL)

static inline string_t* BUFFER(void) {
    assert(stack != NULL);
    return peek_ptr_list(stack);
}

void create_output_buffer(void) {
    ENTER;
    // buffer = create_string(NULL);
    stack = create_ptr_list();
    push_ptr_list(stack, create_string(NULL));

    RETURN();
}

void destroy_output_buffer(void) {
    ENTER;
    if(stack != NULL) {
        for(int i = 0; i < stack->len; i++)
            destroy_string(stack->buffer[i]);
        destroy_ptr_list(stack);
    }
    RETURN();
}

void save_output_buffer(string_t* fname) {
    ENTER;
    assert(stack != NULL);
    FILE* fp;
    if(fname != NULL) {
        fp = fopen(fname->buffer, "w");
        if(fp == NULL)
            error("cannot open output file: \"%s\": %s", fname->buffer, strerror(errno));
    }
    else
        fp = stdout;

    TRACE("write %d bytes to file name \"%s\"",
          ((string_t*)(stack->buffer[0]))->len, (fname != NULL) ? fname->buffer : "stdout");
    fwrite(((string_t*)(stack->buffer[0]))->buffer, ((string_t*)(stack->buffer[0]))->len, sizeof(char), fp);

    // don't close stdout...
    if(fname != NULL)
        fclose(fp);
    RETURN();
}

void add_output_buffer_char(int ch) {
    assert(stack != NULL);
    append_string_char(BUFFER(), ch);
}

void add_output_buffer_str(string_t* str) {

    assert(stack != NULL);
    append_string_str(BUFFER(), str);
}

void add_output_buffer(const char* str) {

    assert(stack != NULL);
    append_string(BUFFER(), str);
}

void push_output_buffer(string_t* str) {
    ENTER;
    TRACE("str ptr: %p", (void*)str);
    push_ptr_list(stack, str);
    RETURN();
}

string_t* pop_output_buffer(void) {
    ENTER;
    string_t* crnt = pop_ptr_list(stack);
    RETURN(crnt);
}

string_t* peek_output_buffer(void) {
    ENTER;
    string_t* str = peek_ptr_list(stack);
    RETURN(str);
}

void dump_output_buffer(void) {

    LEGEND("begin output buffer");
    if(BUFFER() != NULL) {
        printf("len: %d\ncap %d\n", BUFFER()->len, BUFFER()->cap);
        LEGEND(NULL);
        hexdump(BUFFER()->buffer, BUFFER()->len);
    }
    else {
        printf("no buffers are open\n");
    }
    LEGEND("end output buffer");
}
