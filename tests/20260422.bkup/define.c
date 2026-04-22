/*
 * Define a symbol. A symbol can have replacement text or not. No checking or
 * replacement is done on the replacement text when the define is parsed. It
 * is copied verbatim.
 *
 * The
 */

#include "common.h"

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

static parm_t* find_parm(parm_list_t* lst, string_t* name) {

    parm_t* p = NULL;

    for(int i = 0; i < lst->len; i++)
        if(!comp_string_str(name, lst->lst[i]->name)) {
            p = lst->lst[i];
            break;
        }

    return p;
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
        p->repl = NULL;

        lst->lst[lst->len] = p;
        lst->len++;
    }
    else
        warning("duplicate parameter name \"%s\" is ignored", name->buf);

    RETURN();
}

/*
    This is called while the macro is being expanded into the output. The
    name appears in the value of the macro and gets expanded upon reference.

    Return NULL of the parameter name is not found. That is not an error.
    Just echo the name with the '@'.

    Return a blank string if the value of the parameter is blank.

    Return the full string of the named parameter has a value.
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

        // expansion was requested
        p->flag = true;
    }
    else
        RETURN(NULL);

    RETURN(s);
}

/*
    Set the value of the parameter when it is referenced. Macro parameters
    are positional. If the parameter is just the name without the '@' then
    it is treated as simple text. If the '@' is present and the name has a
    value in the symbol table, then the repl value is that.
*/
void set_parm(parm_list_t* lst, int index, string_t* repl) {
    ENTER;

    if(index >= 0 && index < lst->len) {
        if(lst->lst[index]->repl != NULL) {
            clear_string(lst->lst[index]->repl);
            append_string_str(lst->lst[index]->repl, repl);
        }
        else
            lst->lst[index]->repl = copy_string(repl);

        lst->lst[index]->flag = false;
    }
    else
        error("parameter index is out of range: %d (max = %d)", index, lst->len - 1);

    RETURN();
}

void clear_parms(parm_list_t* lst) {
    ENTER;
    (void)lst;
    RETURN();
}

/*
    Read the parameters for a definition. The parameters are stored by name in
    the order in which they are encountered.
*/
void process_define_parameters(symbol_t* sym) {

    ENTER;

    // should be a '('
    TRACE("char on entry: \'%c\'", get_char());
    consume_char();
    consume_space();
    test_end();

    bool finished = false;
    while(!finished) {
        consume_space();
        test_end();
        int ch = get_char();
        if(ch == ',')
            consume_char();
        else if(ch == ')') {
            consume_char();
            finished = true;
        }
        else {
            string_t* name = expect_name();
            if(name == NULL)
                consume_error("a name");
            else {
                append_parm_list(sym->parms, name);
                sym->arity++;
            }
        }
    }

    RETURN();
}

void process_define_body(symbol_t* sym) {

    ENTER;

    // should be a '{'
    TRACE("char on entry: \'%c\'", get_char());
    consume_char();

    int ch;
    int count = 1;
    int size = 0;
    string_t* tmp = create_string(NULL);
    while(true) {
        test_end();
        ch = get_char();
        if(ch == '}') {
            count--;
            if(count == 0) {
                if(tmp->len > 0)
                    sym->repl_text = tmp;
                consume_char();
                TRACE("size: %d", size);
                RETURN();
            }
        }
        else if(ch == '{')
            count++;

        append_string_char(tmp, ch);
        consume_char();
        size++;
    }

    RETURN(); // never happens
}

int process_define(void) {

    ENTER;
    consume_space();
    test_end();

    // get the symbol name
    string_t* name = expect_name();
    if(name != NULL) {
        symbol_t* sym = insert_symbol(name);

        // get the parameter list, which is optional
        consume_space();
        if(get_char() == '(') {
            process_define_parameters(sym);

            // the body is not optional.
            consume_space();
            if(get_char() == '{')
                process_define_body(sym);
            else
                error("a definition body is required if there are macro parameters");
        }
        else {
            // the body is optional.
            if(get_char() == '{')
                process_define_body(sym);
        }
        int ch = get_char();
        TRACE("char after body: '%c' (0x%02X)", !isspace(ch) ? ch : ' ', ch);
    }
    else
        error("expected name for .define");

    RETURN(1);
}
