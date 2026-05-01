#include "common.h"

#define MAX_DEPTH 10

typedef struct buffer_stack_t {
    string_t* str;
    int index;
    int line;
    int col;
    string_t* fname;
    int ch;
    int depth;
    struct buffer_stack_t* next;
} buffer_stack_t;

#define CRNT_CHAR ((bstack != NULL)? bstack->str->buffer[bstack->index]: EOI)
static buffer_stack_t* bstack = NULL;

static buffer_stack_t* create_input_buffer(string_t* str, string_t* fname) {
    ENTER;
    buffer_stack_t* buf = _ALLOC_TYPE(struct buffer_stack_t);
    buf->str = copy_string(str);
    buf->index = 0;
    buf->line = 1;
    buf->col = 1;
    buf->depth = 0;
    if(fname != NULL)
        buf->fname = copy_string(fname);
    else
        buf->fname = NULL;
    buf->next = NULL;
    RETURN(buf);
}

static void destroy_input_buffer(buffer_stack_t* bstack) {
    ENTER;
    if(bstack != NULL) {
        destroy_string(bstack->str);
        _FREE(bstack);
    }
    RETURN();
}

/*
 * Return true if the input buffer is valid.
 */
bool valid_input_buffer(void) {

    return (bstack != NULL);
}


/*
 * Open a file and load it as a input buffer.
 */
void load_input_buffer(string_t* fname) {
    ENTER;
    ASSERT(fname != NULL, "need a file name");

    TRACE("read file into scanner buffer: %s", fname->buffer);

    const char* fstr = find_file(fname->buffer);
    if(fstr == NULL)
        error("cannot locate input file: \"%s\"", fname->buffer);

    FILE* fp = fopen(fstr, "r");
    if(NULL == fp)
        error("cannot open input file: \"%s\": %s", fstr, strerror(errno));

    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    TRACE("file size: %d", size);

    buffer_stack_t* buf = create_input_buffer(create_string(NULL), fname);
    buf->fname = copy_string(fname);

    if(buf->str->len + size + 1 > buf->str->cap) {
        while(buf->str->len + size + 1 > buf->str->cap)
            buf->str->cap <<= 1;
        buf->str->buffer = _REALLOC_ARRAY(buf->str->buffer, char, buf->str->cap);
    }

    fread(buf->str->buffer, sizeof(char), size, fp);
    fclose(fp);
    buf->str->len = size;
    buf->ch = buf->str->buffer[0];

    // put it on the top of the stack
    if(bstack != NULL) {
        buf->depth = bstack->depth + 1;
        if(buf->depth >MAX_DEPTH)
            error("maximum incude depth of %d exceeded", MAX_DEPTH);
        buf->next = bstack;
    }
    bstack = buf;

    RETURN();
}

/*
 * Make the specified string the current buffer to input from.
 */
void push_input_buffer(string_t* str) {
    ENTER;
    buffer_stack_t* buf = create_input_buffer(str, NULL);
    if(valid_input_buffer()) {
        buf->depth = bstack->depth + 1;
        if(buf->depth >MAX_DEPTH)
            error("maximum incude depth of %d exceeded", MAX_DEPTH);
        buf->next = bstack;
    }
    bstack = buf;
    RETURN();
}

/*
 * Remove the current input buffer from the top of the stack and destroy it.
 */
void pop_input_buffer(void) {
    ENTER;
    if(valid_input_buffer()) {
        buffer_stack_t* buf = bstack;
        bstack = buf->next;
        destroy_input_buffer(buf);
    }
    RETURN();
}

int crnt_char(void) {

    if(bstack != NULL)
        return bstack->ch;
    else
        return EOI;
}

int advance_char(void) {

    if(valid_input_buffer()) {
        if((bstack->index < bstack->str->len - 1) && CRNT_CHAR != '\0') {
            if(CRNT_CHAR == EOL) {
                bstack->line++;
                bstack->col = 1;
            }
            else
                bstack->col++;

            bstack->index++;
            bstack->ch = CRNT_CHAR;
        }
        else {
            bstack->ch = EOF;
        }
    }
    return bstack->ch;
}

void unget_char(void) {

    if(valid_input_buffer()) {
        if(bstack->index > 0)
            bstack->index--;
    }
}

void unget_string(int len) {

    if(valid_input_buffer()) {
        if(bstack->index > len)
            bstack->index -= len;
        else
            bstack->index = 0;
    }
}

string_t* get_file_name(void) {

    if(valid_input_buffer())
        return bstack->fname;
    else
        return NULL;
}

int get_line_no(void) {

    if(valid_input_buffer())
        return bstack->line;
    else
        return 0;
}

int get_col_no(void) {

    if(valid_input_buffer())
        return bstack->col;
    else
        return 0;
}

void dump_input_buffer(void) {

    print_legend("begin input buffer");
    printf("file: %s\nindex: %d\nlen: %d\ncap %d\nnext: %p\n",
            (bstack->fname)?bstack->fname->buffer: "(none)",
            bstack->index, bstack->str->len,
            bstack->str->cap, (void*)bstack->next);
    print_legend(NULL);
    fwrite(bstack->str->buffer, bstack->str->len, sizeof(char), stdout);
    print_legend("end input buffer");
}
