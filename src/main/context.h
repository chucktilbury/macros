#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "string_buffer.h"
#include "symbols.h"

void push_context(symbol_t* sym);
void pop_context(void);
string_t* find_context(string_t* tag);

#endif /* _CONTEXT_H_ */
