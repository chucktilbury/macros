#ifndef _PARMS_H_
#define _PARMS_H_

#include <stdbool.h>
#include "string_buffer.h"
#include "symbols.h"

typedef struct parm_t {
    string_t* name;
    string_t* repl;
    // bool flag;
    int position;
    struct parm_t* next;
} parm_t;

typedef struct parm_list_t {
    parm_t** lst;
    int cap;
    int len;
} parm_list_t;

void get_param_names(symbol_t* sym);
parm_list_t* create_parm_list(void);
void destroy_parm_list(parm_list_t* lst);
void append_parm_list(parm_list_t* lst, string_t* name);
string_t* get_parm(parm_list_t* lst, string_t* name);
parm_t* find_parm(parm_list_t* lst, string_t* name);
void get_reference_parms(symbol_t* sym);

#endif /* _PARMS_H_ */
