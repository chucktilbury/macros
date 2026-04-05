
#include "common.h"

static char_buffer_t* crnt_buffer = NULL;

char_buffer_t* create_char_buffer(string_t* fname) {
    ENTER;

    char_buffer_t* buf = _ALLOC_TYPE(char_buffer_t);
    buf->cap = 1 << 3;
    buf->len = 0;
    buf->index = 0;
    buf->buffer = _ALLOC_ARRAY(char, buf->cap);

    buf->fname = copy_string(fname);
    buf->col = 1;
    buf->line = 1;
    buf->ch = EOF;

    RETURN(buf);
}

void destroy_char_buffer(char_buffer_t* buf) {
    ENTER;

    if(buf != NULL) {
        destroy_string(buf->fname);
        _FREE(buf->buffer);
        _FREE(buf);
    }

    RETURN();
}

char_buffer_t* get_char_buffer(void) {

    return crnt_buffer;
}

void append_char_buffer(const char* str) {
    ENTER;

    size_t len = strlen(str);
    if(crnt_buffer->len + len + 1 > crnt_buffer->cap) {
        while(crnt_buffer->len + len > crnt_buffer->cap)
            crnt_buffer->cap <<= 1;
        crnt_buffer->buffer = _REALLOC_ARRAY(crnt_buffer->buffer, char, crnt_buffer->cap);
    }

    strcat(crnt_buffer->buffer, str);
    crnt_buffer->len += len;

    RETURN();
}

void append_char_buffer_char(int ch) {
    ENTER;

    if(crnt_buffer->len + 1 > crnt_buffer->cap) {
        crnt_buffer->cap <<= 1;
        crnt_buffer->buffer = _REALLOC_ARRAY(crnt_buffer->buffer, char, crnt_buffer->cap);
    }

    crnt_buffer->buffer[crnt_buffer->len] = ch;
    crnt_buffer->len++;
    crnt_buffer->buffer[crnt_buffer->len] = '\0';

    RETURN();
}

void append_char_buffer_str(string_t* str) {
    ENTER;

    append_char_buffer(str->buf);

    RETURN();
}

void set_char_buffer(char_buffer_t* buf) {
    ENTER;

    crnt_buffer = buf; // could be NULL

    RETURN();
}

int get_char(void) {

    if(crnt_buffer == NULL)
        return EOI;
    else
        return crnt_buffer->ch;
}

void consume_char(void) {

    if(crnt_buffer != NULL) {
        if(crnt_buffer->index < crnt_buffer->len) {
            if(crnt_buffer->ch == EOL) {
                crnt_buffer->line++;
                crnt_buffer->col = 1;
            }
            else
                crnt_buffer->col++;

            crnt_buffer->ch = crnt_buffer->buffer[crnt_buffer->index];
            crnt_buffer->index++;
        }
        else {
            TRACE(10, "EOF FOUND");
            crnt_buffer->ch = EOF;
            // return EOF;
        }
    }
}

void unget_string(size_t len) {

    ASSERT(crnt_buffer != NULL, "attempt to unget string from NULL char buffer");

    if(crnt_buffer->index - len >= 0)
        crnt_buffer->index -= len;
    else
        crnt_buffer->index = 0;

    crnt_buffer->ch = crnt_buffer->buffer[crnt_buffer->index];
}

int get_line_no(void) {

    if(crnt_buffer != NULL)
        return crnt_buffer->line;
    else
        return -1;
}

int get_col_no(void) {

    if(crnt_buffer != NULL)
        return crnt_buffer->col;
    else
        return -1;
}

string_t* get_file_name(void) {

    if(crnt_buffer != NULL)
        return crnt_buffer->fname;
    else
        return NULL;
}

void read_char_buffer(size_t size, FILE* fp) {
    ENTER;
    ASSERT(crnt_buffer != NULL, "attempt to read into a NULL buffer");

    if(crnt_buffer->len + size + 1 < crnt_buffer->cap) {
        while(crnt_buffer->len + size < crnt_buffer->cap)
            crnt_buffer->cap <<= 1;
        crnt_buffer->buffer = _REALLOC_ARRAY(crnt_buffer->buffer, char, crnt_buffer->cap);
    }

    fread(crnt_buffer->buffer, sizeof(char), size, fp);
    crnt_buffer->len = size;

    RETURN();
}

void write_char_buffer(FILE* fp) {
    ENTER;
    ASSERT(crnt_buffer != NULL, "attempt to write a NULL buffer");

    fwrite(crnt_buffer->buffer, sizeof(char), crnt_buffer->len, fp);

    RETURN();
}
