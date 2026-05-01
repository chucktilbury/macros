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
void dump_output_buffer(void);

#define EMITC(c) add_output_buffer_char(c)
#define EMITS(s) add_output_buffer_str(s)
#define EMITR(s) add_output_buffer(s)

#endif /* _OUTPUT_BUFFER_H_ */
