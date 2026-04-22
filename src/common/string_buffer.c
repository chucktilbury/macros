
#include "common.h"

string_t* create_string(const char* str) {

    string_t* s = _ALLOC_TYPE(string_t);
    s->len = 0;
    s->cap = 0x01 << 3;
    s->buffer = _ALLOC_ARRAY(char, s->cap);

    if(str != NULL)
        append_string(s, str);

    return s;
}

void destroy_string(string_t* s) {

    if(s != NULL) {
        _FREE(s->buffer);
        _FREE(s);
    }
}

void append_string_str(string_t* s, string_t* str) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(str != NULL, "string_t initializer required");

    append_string(s, str->buffer);
}

void append_string(string_t* s, const char* str) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(str != NULL, "string_t initializer required");

    int len = strlen(str);
    if(s->len + len + 1 > s->cap) {
        while(s->len + len + 1 > s->cap)
            s->cap <<= 1;
        s->buffer = _REALLOC_ARRAY(s->buffer, char, s->cap);
    }

    strcpy(&s->buffer[s->len], str);
    s->len += len;
}

void append_string_char(string_t* s, int ch) {

    ASSERT(s != NULL, "invalid string_t pointer");
    ASSERT(ch != '\0', "invalid end of string")

    if(s->len + 2 > s->cap) {
        s->cap <<= 1;
        s->buffer = _REALLOC_ARRAY(s->buffer, char, s->cap);
    }

    s->buffer[s->len] = ch;
    s->len++;
    s->buffer[s->len] = '\0';
}

void clear_string(string_t* s) {

    ASSERT(s != NULL, "invalid string_t pointer");
    s->len = 0;
}

int comp_string_str(string_t* left, string_t* right) {

    return strcmp(left->buffer, right->buffer);
}

int comp_string(string_t* left, const char* right) {

    return strcmp(left->buffer, right);
}

string_t* upcase_string(string_t* s) {

    string_t* tmp = copy_string(s);
    for(int i = 0; tmp->buffer[i] != '\0'; i++)
        tmp->buffer[i] = toupper(tmp->buffer[i]);

    return tmp;
}

string_t* downcase_string(string_t* s) {

    string_t* tmp = copy_string(s);
    for(int i = 0; tmp->buffer[i] != '\0'; i++)
        tmp->buffer[i] = tolower(tmp->buffer[i]);

    return tmp;
}

// strip all instances of the characters given in the pattern
void strip_string(string_t* str, const char* pattern) {

    char* tpt;
    char* s = str->buffer;

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

    return create_string(str ? str->buffer : NULL);
}

const char* raw_string(string_t* str) {

    return str ? str->buffer : NULL;
}

void dump_raw_string(string_t* str) {

    for(int i = 0; i < str->len; i++) {
        int ch = str->buffer[i];
        if(isprint(ch) || ch == '\n')
            fputc(ch, stdout);
        else
            printf("0x%02X", ch);
    }
}
