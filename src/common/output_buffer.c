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

// static const char* prnch(int ch) {
//
//     static char buffer[8];
//
//     memset(buffer, 0, sizeof(buffer));
//     if(isprint(ch))
//         buffer[0] = ch;
//     else
//         sprintf(buffer, "0x%02X", ch);
//
//     return buffer;
// }

void dump_output_buffer(void) {

    print_legend("begin output buffer");
    if(buffer != NULL) {
        printf("len: %d\ncap %d\n", buffer->len, buffer->cap);
        print_legend(NULL);
        // int ch;
        // int i = 0;
        // do {
        //     ch = buffer->buffer[i];
        //     printf("%s", prnch(ch));
        //     i++;
        // } while(ch != '\0');
        fwrite(buffer->buffer, buffer->len, sizeof(char), stdout);
    }
    else {
        printf("no buffers are open\n");
    }
    print_legend("end output buffer");
}
