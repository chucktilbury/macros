
#include "common.h"
#include "parms.h"
#include "symbols.h"
#include "parms.h"
#include "process.h"

/*
 * Read the parameters for a definition. The parameters are stored by
 * position in the order in which they are encountered. Whatever is between
 * the commas with the spaces stripped from the ends is what is stored.
 */
void get_param_names(symbol_t* sym) {

    ENTER;
    // symbol_t* sym = peek_symbol_context();
    ASSERT(sym != NULL, "attempt to define reference parms on empty symbol stack");

    // should be a '('
    TRACE("char on entry: \'%c\'", crnt_char());
    expect_char('(');

    advance_char();
    consume_space();
    test_end_error();

    bool finished = false;
    while(!finished) {
        consume_space();
        test_end_error();
        int ch = crnt_char();
        if(ch == ',')
            advance_char();
        else if(ch == ')') {
            advance_char();
            finished = true;
        }
        else {
            string_t* name = scan_name();
            if(name == NULL)
                consume_error("a name");
            else {
                append_parm_list(sym->parms, name);
                sym->arity++;
            }
        }
    }
    TRACE("arity: %d", sym->arity);
    TRACE("buffer len: %d", sym->parms->len);

    RETURN();
}

parm_list_t* create_parm_list(void) {
    ENTER;

    parm_list_t* lst = _ALLOC_TYPE(parm_list_t);
    lst->cap = 1 << 3;
    lst->len = 0;
    lst->lst = _ALLOC_ARRAY(parm_t*, lst->cap);

    RETURN(lst);
}

void destroy_parm_list(parm_list_t* lst) {
    ENTER;

    if(lst != NULL) {
        for(int i = 0; i < lst->len; i++) {
            destroy_string(lst->lst[i]->name);
            if(lst->lst[i]->repl != NULL)
                destroy_string(lst->lst[i]->repl);
            _FREE(lst->lst[i]);
        }

        _FREE(lst->lst);
        _FREE(lst);
    }

    RETURN();
}

parm_t* find_parm(parm_list_t* lst, string_t* name) {

    ENTER;
    parm_t* p = NULL;
    TRACE("find name: %s", name->buffer);
    TRACE("list len: %d", lst->len);
    for(int i = 0; i < lst->len; i++) {
        TRACE("compare: %s", lst->lst[i]->name->buffer);
        if(!comp_string_str(name, lst->lst[i]->name)) {
            p = lst->lst[i];
            break;
        }
    }
    RETURN(p);
}

void append_parm_list(parm_list_t* lst, string_t* name) {
    ENTER;

    if(!find_parm(lst, name)) {
        if(lst->len + 1 > lst->cap) {
            lst->cap <<= 1;
            lst->lst = _REALLOC_ARRAY(lst->lst, parm_t*, lst->cap);
        }

        parm_t* p = _ALLOC_TYPE(parm_t);
        p->name = copy_string(name);
        p->repl = create_string(NULL);

        lst->lst[lst->len] = p;
        lst->len++;
    }
    else
        warning("duplicate parameter name \"%s\" is ignored", name->buffer);

    RETURN();
}

/*
 * This is called while the macro is being expanded into the output. The
 * name appears in the value of the macro and gets expanded upon reference.
 *
 * Return NULL of the parameter name is not found. That is not an error.
 * Just echo the name with the '@'.
 *
 * Return a blank string if the value of the parameter is blank.
 *
 * Return the full string of the named parameter has a value.
 */
string_t* get_parm(parm_list_t* lst, string_t* name) {
    ENTER;

    string_t* s = NULL;
    parm_t* p = NULL;

    if(NULL != (p = find_parm(lst, name))) {
        if(p->repl == NULL)
            s = create_string(NULL);
        else
            s = copy_string(p->repl);
    }
    else
        RETURN(NULL);

    RETURN(s);
}
