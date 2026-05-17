
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "_string.h"

string_t* create_string(const char* str) {

    string_t* nstr = malloc(sizeof(string_t));
    assert(nstr != NULL);
    nstr->len = 0;
    nstr->cap = 1 << 3;
    nstr->buffer = malloc(nstr->cap);
    assert(nstr->buffer != NULL);

    if(str != NULL)
        append_string_str(nstr, str);

    return nstr;
}

void destroy_string(string_t* s) {

    assert(s != NULL);
    if(s != NULL) {
        free(s->buffer);
        free(s);
    }
}

void append_string(string_t* s, int ch) {

    assert(s != NULL);
    if(s->cap < s->len + 2) {
        s->cap <<= 1;
        s->buffer = realloc(s->buffer, s->cap);
        assert(s->buffer != NULL);
    }

    s->buffer[s->len] = ch;
    s->len++;
    s->buffer[s->len] = '\0';
}

void append_string_str(string_t* s, const char* str) {

    assert(s != NULL);
    int len = strlen(str);
    if(s->cap < s->len + len + 1) {
        while(s->cap < s->len + len + 1)
            s->cap <<= 1;
        s->buffer = realloc(s->buffer, s->cap);
        assert(s->buffer != NULL);
    }

    strcpy(&s->buffer[s->len], str);
    s->len = strlen(s->buffer);
}

void clear_string(string_t* s) {

    assert(s != NULL);
    s->len = 0;
    s->buffer[0] = '\0';
}

string_t* copy_string(string_t* str) {

    return create_string(str->buffer);
}

