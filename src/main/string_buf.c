
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

void append_string_str(string_t* s, string_t* str) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(str != NULL, "string_t initializer required");

    append_string(s, str->buf);
}

void append_string(string_t* s, const char* str) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(str != NULL, "string_t initializer required");

    int len = strlen(str);
    if(s->len + len + 1 > s->cap) {
        while(s->len + len + 1 > s->cap)
            s->cap <<= 1;
        s->buf = _REALLOC_ARRAY(s->buf, char, s->cap);
    }

    strcpy(&s->buf[s->len], str);
    s->len += len;
}

void append_string_char(string_t* s, int ch) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(ch != '\0', "invalid end of string")

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

int comp_string_str(string_t* left, string_t* right) {

    return strcmp(left->buf, right->buf);
}

int comp_string(string_t* left, const char* right) {

    return strcmp(left->buf, right);
}

string_t* upcase_string(string_t* s) {

    string_t* tmp = copy_string(s);
    for(int i = 0; tmp->buf[i] != '\0'; i++)
        tmp->buf[i] = toupper(tmp->buf[i]);

    return tmp;
}

string_t* downcase_string(string_t* s) {

    string_t* tmp = copy_string(s);
    for(int i = 0; tmp->buf[i] != '\0'; i++)
        tmp->buf[i] = tolower(tmp->buf[i]);

    return tmp;
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
                memmove(tpt, tpt + 1, strlen(tpt + 1) + 1);
                str->len--;
            }
        }
    }
}

string_t* copy_string(string_t* str) {

    if(str == NULL)
        return create_string(NULL);
    else
        return create_string(str->buf);
}

const char* raw_string(string_t* str) {

    return str->buf;
}

void dump_raw_string(string_t* str) {

    for(int i = 0; i < str->len; i++) {
        int ch = str->buf[i];
        if(isprint(ch) || ch == '\n')
            fputc(ch, stdout);
        else
            printf("0x%02X", ch);
    }
}

