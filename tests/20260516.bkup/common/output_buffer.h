#ifndef _OUTPUT_BUFFER_H_
#define _OUTPUT_BUFFER_H_

#include "string_buffer.h"

// one global buffer
void create_output_buffer(void);
void destroy_output_buffer(void);

void save_output_buffer(string_t* fname);
void add_output_buffer(const char* str);
void add_output_buffer_char(int ch);
void add_output_buffer_str(string_t* str);
void push_output_buffer(string_t* str);
string_t* pop_output_buffer(void);
string_t* peek_output_buffer(void);
void dump_output_buffer(void);

#define EMITC(c) do { \
    TRACE("EMIT: %s", prnch(c)); \
    add_output_buffer_char(c); \
} while(false)

#define EMITS(s) do { \
    for(int i = 0; i < s->len; i++) \
        EMITC(s->buffer[i]); \
} while(false)

#define EMITR(s) do { \
    for(int i = 0; s[i] != 0; i++) \
        EMITC(s[i]); \
} while(false)

#endif /* _OUTPUT_BUFFER_H_ */
