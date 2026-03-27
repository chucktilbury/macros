
#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "common.h"

typedef enum {
    SYM_CODE,
    SYM_DATA
} symbol_type_t;

typedef struct {
    string_t* name;
    string_t* fname;
    int line;
    int col;
} symbol_t;

void create_symbol_table(void);
void add_symbol(string_t* name);
symbol_t* get_symbol(string_t* name);
void dump_sym_table(void);

#endif /* _SYMBOLS_H_ */
