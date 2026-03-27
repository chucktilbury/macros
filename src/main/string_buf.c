
#include "common.h"

string_t* create_string(const char* str) {

    string_t* s = _ALLOC_TYPE(string_t);
    s->len = 0;
    s->cap = 0x01 << 3;
    s->buf = _ALLOC_ARRAY(char, s->cap);

    if(str != NULL)
        append_string(s, str);

    return s;
}

void destroy_string(string_t* s) {

    if(s != NULL) {
        _FREE(s->buf);
        _FREE(s);
    }
}

void append_string(string_t* s, const char* str) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(str != NULL, "string_t initializer required");

    int len = strlen(str) + 1;
    if(s->len + len > s->cap) {
        while(s->len + len > s->cap)
            s->cap <<= 1;
        s->buf = _REALLOC_ARRAY(s->buf, char, s->cap);
    }

    strcpy(&s->buf[s->len], str);
    s->len = strlen(s->buf);
}

void append_string_char(string_t* s, int ch) {

    ASSERT(s != NULL, "invalid string_t pointer");

    if(s->len + 2 > s->cap) {
        s->cap <<= 1;
        s->buf = _REALLOC_ARRAY(s->buf, char, s->cap);
    }

    s->buf[s->len] = ch;
    s->len++;
    s->buf[s->len] = '\0';
}

void clear_string(string_t* s) {

    ASSERT(s != NULL, "invalid string_t pointer");
    s->len = 0;
}

int comp_string(string_t* left, string_t* right) {

    return strcmp(left->buf, right->buf);
}

// strip all instances of the characters given in the pattern
void strip_string(string_t* str, const char* pattern) {

    char* tpt;
    char* s = str->buf;

    for(int i = 0; pattern[i] != '\0'; i++) {
        while(true) {
            tpt = strchr(s, pattern[i]);
            if(tpt == NULL)
                break;
            else {
                memmove(tpt, tpt+1, strlen(tpt+1)+1);
                str->len--;
            }
        }
    }
}

string_t* copy_string(string_t* str) {

    return create_string(str->buf);
}
