#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "string.h"

typedef struct symbol_t {
    string_t* tag;
    struct symbol_t* parms;
    string_t* repl_text;
    int line;
    int col;
    string_t* fname;
    struct symbol_t* left;
    struct symbol_t* right;
} symbol_t;

symbol_t* create_symbol(string_t* tag);
void destroy_sym_table(symbol_t* node);

void insert_symbol(symbol_t* root, symbol_t* sym);
symbol_t* find_symbol(symbol_t* root, string_t* tag);
void dump_symbol_table(symbol_t* node);

#endif /* _SYMBOLS_H_ */
