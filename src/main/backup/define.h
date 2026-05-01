#ifndef _DEFINE_H_
#define _DEFINE_H_

// #include "symbols.h"
#include <stdbool.h>
#include "string_buffer.h"

typedef struct {
    string_t* name;
    string_t* repl;
    bool flag;
} parm_t;

typedef struct {
    parm_t** lst;
    int cap;
    int len;
} parm_list_t;

void process_define(void);
void process_reference(void);

parm_list_t* create_parm_list(void);
void destroy_parm_list(parm_list_t* lst);
void append_parm_list(parm_list_t* lst, string_t* name);
string_t* get_parm(parm_list_t* lst, string_t* name);
void set_parm(parm_list_t* lst, int index, string_t* repl);
void clear_parms(parm_list_t* lst);

#endif /* _DEFINE_H_ */
