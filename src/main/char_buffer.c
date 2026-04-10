
#include "common.h"

static char_buffer_t* input_buffer = NULL;
static char_buffer_t* output_buffer = NULL;

/***********************
 * GENERIC FUNCTIONS
 */
char_buffer_t* create_char_buffer(const char* fname) {
    ENTER;

    char_buffer_t* buf = _ALLOC_TYPE(char_buffer_t);
    buf->cap = 1 << 3;
    buf->len = 0;
    buf->index = 0;
    buf->buffer = _ALLOC_ARRAY(char, buf->cap);

    if(fname != NULL)
        buf->fname = create_string(fname);
    else
        buf->fname = NULL;

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

size_t get_char_buffer_index(char_buffer_t* buf) {
    ENTER;
    ASSERT(buf != NULL, "taking index of NULL char buffer");
    RETURN(buf->index);
}

int get_line_no(char_buffer_t* buf) {

    if(buf != NULL)
        return buf->line;
    else
        return -1;
}

int get_col_no(char_buffer_t* buf) {

    if(buf != NULL)
        return buf->col;
    else
        return -1;
}

string_t* get_file_name(char_buffer_t* buf) {

    if(buf != NULL)
        return buf->fname;
    else
        return NULL;
}

char_buffer_t* copy_char_buffer(char_buffer_t* buf) {
    ENTER;
    RETURN(NULL);
}

void dump_char_buffer(char_buffer_t* buf) {

    printf("\n---------- char buffer ----------\n");
    for(size_t i = 0; i < buf->len; i++) {
        int ch = buf->buffer[i];
        if(isprint(ch) || ch == '\n')
            fputc(ch, stdout);
        else
            printf("0x%02X", ch);
    }
    printf("---------------------------------\n");
}

/*****************************************
 * INPUT FUCTIONS
 */
void set_input_buffer(char_buffer_t* buf) {
    ENTER;

    input_buffer = buf; // could be NULL

    RETURN();
}

char_buffer_t* get_input_buffer(void) {
    // ENTER;
    // RETURN(input_buffer);
    return input_buffer;
}

int get_char(void) {

    if(input_buffer == NULL)
        return EOI;
    else
        return input_buffer->ch;
}

void consume_char(void) {

    if(input_buffer != NULL) {
        if((input_buffer->index < input_buffer->len) &&
           input_buffer->buffer[input_buffer->index] != '\0') {
            if(input_buffer->ch == EOL) {
                input_buffer->line++;
                input_buffer->col = 1;
            }
            else
                input_buffer->col++;

            input_buffer->ch = input_buffer->buffer[input_buffer->index];
            input_buffer->index++;
        }
        else {
            TRACE(10, "EOF FOUND");
            input_buffer->ch = EOF;
        }
    }
}

void unget_string(size_t len) {

    ASSERT(input_buffer != NULL, "attempt to unget string from NULL char buffer");

    if(input_buffer->index - len >= 0)
        input_buffer->index -= len;
    else
        input_buffer->index = 0;

    input_buffer->ch = input_buffer->buffer[input_buffer->index];
}

/******************************
 * OUTPUT FUNCTIONS
 */
char_buffer_t* set_output_buffer(const char* fname) {
    ENTER;
    output_buffer = create_char_buffer(fname);
    RETURN(output_buffer);
}

char_buffer_t* get_output_buffer(void) {
    // ENTER;
    // RETURN(output_buffer);
    return output_buffer;
}

void append_char_buffer(const char* str) {
    // ENTER;
    ASSERT(output_buffer != NULL, "operation on NULL char buffer");

    size_t len = strlen(str);
    if(output_buffer->len + len + 1 > output_buffer->cap) {
        while(output_buffer->len + len + 1 > output_buffer->cap)
            output_buffer->cap <<= 1;
        output_buffer->buffer = _REALLOC_ARRAY(output_buffer->buffer, char, output_buffer->cap);
    }

    strcat(output_buffer->buffer, str);
    output_buffer->len += len;

    // RETURN();
}

void append_char_buffer_char(int ch) {
    // ENTER;
    ASSERT(output_buffer != NULL, "operation on NULL char buffer");

    if(output_buffer->len + 2 > output_buffer->cap) {
        output_buffer->cap <<= 1;
        output_buffer->buffer = _REALLOC_ARRAY(output_buffer->buffer, char, output_buffer->cap);
    }

    output_buffer->buffer[output_buffer->len] = ch;
    output_buffer->len++;
    output_buffer->buffer[output_buffer->len] = '\0';

    // RETURN();
}

void append_char_buffer_str(string_t* str) {

    if(str != NULL)
        append_char_buffer(str->buf);
}

void insert_char_buffer(size_t index, string_t* str) {
    ENTER;
    RETURN();
}

void contract_char_buffer(size_t index, size_t len) {
    ENTER;
    RETURN();
}

void replace_char_buffer(string_t* find, string_t* repl) {
    ENTER;
    RETURN();
}

size_t search_char_buffer(int ch) {
    ENTER;
    RETURN(0);
}

/***************************
 * FILE FUNCTIONS
 */
char_buffer_t* read_char_buffer(string_t* fname) {
    ENTER;
    ASSERT(fname != NULL, "need a file name");

    TRACE(10, "read file into char buffer: %s", fname->buf);

    const char* fstr = find_file(fname->buf);
    if(fstr == NULL)
        error("cannot locate input file: \"%s\"", fname->buf);

    FILE* fp = fopen(fstr, "r");
    if(NULL == fp)
        error("cannot open input file: \"%s\": %s", fstr, strerror(errno));

    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    TRACE(10, "file size: %lu", size);

    char_buffer_t* buf = create_char_buffer(fstr);

    if(buf->len + size + 1 > buf->cap) {
        while(buf->len + size + 1 > buf->cap)
            buf->cap <<= 1;
        buf->buffer = _REALLOC_ARRAY(buf->buffer, char, buf->cap);
    }

    fread(buf->buffer, sizeof(char), size, fp);
    fclose(fp);
    buf->len = size;

    set_input_buffer(buf);
    consume_char();

    RETURN(buf);
}

void write_char_buffer(char_buffer_t* buf) {
    ENTER;
    ASSERT(buf != NULL, "write a NULL char buffer");

    FILE* fp;
    if(buf->fname != NULL) {
        fp = fopen(buf->fname->buf, "w");
        if(fp == NULL)
            FATAL("cannot open output file: %s: %s", buf->fname->buf, strerror(errno));
    }
    else
        fp = stdout;

    fwrite(buf->buffer, sizeof(char), buf->len, fp);

    if(buf->fname != NULL)
        fclose(fp);

    RETURN();
}
