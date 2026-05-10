
/*
    Abandoned
*/
#include "../src/main/common.h"

typedef struct {
    // dynamic buffer
    string_t* str;
    // last read index
    size_t lcol;
    // preserved for errors
    string_t* fname;
    size_t lno;
} line_t;

typedef struct {
    line_t** buffer;
    size_t len;
    size_t cap;
    size_t index;
} line_buffer_t;

line_buffer_t* create_line_buffer(void) {
}

void destroy_line_buffer(line_buffer_t* buf) {
}

void add_line_buffer(line_buffer_t* buf, string_t* str) {
}

line_t* iterate_line_buffer(line_buffer_t* buf, size_t* post) {
}


