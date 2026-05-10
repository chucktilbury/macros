
#include "common.h"
#include "context.h"
#include "parms.h"

static context_t* stack = NULL;

static context_t* _create_context(string_t* tag, string_t* repl, bool flag) {

    context_t* cont = _ALLOC_TYPE(context_t);
    cont->tag = copy_string(tag);
    cont->repl = copy_string(repl);
    cont->flag = flag;

    if(stack != NULL)
        cont->next = stack;
    stack = cont;

    return cont;
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
    TRACE("name: %s, flag: TRUE", sym->tag->buffer);
    for(int i = 0; i < sym->arity; i++) {
        TRACE("name: %s, flag: FALSE", sym->parms->lst[i]->name->buffer);
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
        for(ptr = stack; ptr != NULL; ptr = next) {
            next = ptr->next;
            stack = next;
            _destroy_context(ptr);
            if(ptr->flag)
                break;
        }
    }
    RETURN();
}

/*
 * Search the context stack for the first name that appears and provide the
 * repl text for that name.
 */
context_t* find_context(string_t* tag) {
    ENTER;
    TRACE("find: %s", tag->buffer);
    if(stack != NULL) {
        context_t* ptr;
        for(ptr = stack; ptr != NULL; ptr = ptr->next) {
            if(!comp_string_str(ptr->tag, tag)) {
                TRACE("FOUND");
                RETURN(ptr);
            }
        }
    }
    TRACE("NOT FOUND");
    RETURN(NULL);
}
