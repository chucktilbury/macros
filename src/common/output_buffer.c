#include "common.h"


static string_t* buffer = NULL;

void create_output_buffer(void) {

    buffer = create_string(NULL);
}

void destroy_output_buffer(void) {

    destroy_string(buffer);
}

void save_output_buffer(string_t* fname) {

    FILE* fp;
    if(fname != NULL) {
        fp = fopen(fname->buffer, "w");
        if(fp == NULL)
            error("cannot open output file: \"%s\": %s", fname->buffer, strerror(errno));
    }
    else
        fp = stdout;

    fwrite(buffer->buffer, buffer->len, sizeof(char), fp);

    // don't close stdout...
    if(fname != NULL)
        fclose(fp);
}

void add_output_buffer_char(int ch) {

    append_string_char(buffer, ch);
}

void add_output_buffer_str(string_t* str) {

    append_string_str(buffer, str);
}

void add_output_buffer(const char* str) {

    append_string(buffer, str);
}

void dump_output_buffer(void) {

    print_legend("begin output buffer");
    printf("len: %d\ncap %d\n", buffer->len, buffer->cap);
    print_legend(NULL);
    fwrite(buffer->buffer, buffer->len, sizeof(char), stdout);
    print_legend("end output buffer");
}
