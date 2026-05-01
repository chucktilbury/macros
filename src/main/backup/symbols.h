#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "string.h"
#include "define.h"

typedef struct symbol_t {
    string_t* tag;
    // struct symbol_t* parms;
    parm_list_t* parms;
    string_t* repl_text;
    int arity;
    int line;
    int col;
    string_t* fname;
    struct symbol_t* left;
    struct symbol_t* right;
} symbol_t;

symbol_t* create_symbol(string_t* tag);
void destroy_sym_table(void);

symbol_t* insert_symbol(string_t* tag);
void _insert_symbol(symbol_t* node, symbol_t* sym);
symbol_t* find_symbol(string_t* tag);
void dump_symbol_table(void);

#endif /* _SYMBOLS_H_ */
