
#include "common.h"
#include "context.h"
#include "parms.h"

typedef struct _context_t {
    bool flag;
    string_t* tag;
    string_t* repl;
    struct _context_t* next;
} context_t;

static context_t* stack = NULL;

static context_t* _create_context(string_t* tag, string_t* repl, bool flag) {
    ENTER;

    TRACE("tag: %s, flag %s", tag->buffer, flag ? "TRUE" : "FALSE");
    context_t* cont = _ALLOC_TYPE(context_t);
    cont->tag = copy_string(tag);
    cont->repl = copy_string(repl);
    cont->flag = flag;

    if(stack != NULL)
        cont->next = stack;
    stack = cont;

    RETURN(cont);
}

static void _destroy_context(context_t* cont) {

    if(cont != NULL) {
        destroy_string(cont->tag);
        destroy_string(cont->repl);
        _FREE(cont);
    }
}

/*
 * Push the context after the parameters have been successfully read.
 */
void push_context(symbol_t* sym) {
    ENTER;
    _create_context(sym->tag, sym->repl_text, true);
    for(int i = 0; i < sym->arity; i++) {
        _create_context(sym->parms->lst[i]->name, sym->parms->lst[i]->repl, false);
    }
    RETURN();
}

/*
 * Pop the context after the repl text has been expended for the symbol.
 */
void pop_context(void) {
    ENTER;
    if(stack != NULL) {
        context_t* ptr;
        context_t* next;
        for(ptr = stack; ptr != NULL && !ptr->flag; ptr = next) {
            next = ptr->next;
            stack = next;
            _destroy_context(ptr);
        }

        if(ptr != NULL && ptr->flag) {
            stack = ptr->next;
            _destroy_context(ptr);
        }
    }
    RETURN();
}

/*
 * Search the context stack for the first name that appears and provide the
 * repl text for that name.
 */
string_t* find_context(string_t* tag) {
    ENTER;
    if(stack != NULL) {
        context_t* ptr;
        for(ptr = stack; ptr != NULL; ptr = ptr->next) {
            if(!comp_string_str(ptr->tag, tag))
                RETURN(ptr->repl);
        }
    }
    RETURN(NULL);
}
