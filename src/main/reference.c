#include "common.h"
#include "symbols.h"
#include "process.h"
#include "parms.h"
#include <unistd.h>
#include "context.h"

void process_reference(void);
#define RECURSION_LIMIT 20
static int recursion_count = 0;

static string_t* _expand_parm(string_t* parm) {

    return copy_string(parm);
}

/*
 * Set the value of the parameter when it is referenced. Macro parameters are
 * positional. If the parameter is just the name without the '@' then it is \
 * treated as simple text. If the '@' is present and the name has a value in
 * the symbol table, then the repl value is that.
 */
static void _set_parm(parm_list_t* lst, int index, string_t* repl) {
    ENTER;

    if(index >= 0 && index < lst->len) {
        if(lst->lst[index]->repl != NULL) {
            clear_string(lst->lst[index]->repl);
            append_string_str(lst->lst[index]->repl, _expand_parm(repl));
        }
        else
            lst->lst[index]->repl = _expand_parm(repl);
    }
    else
        error("too many parameters defined, expected %d", lst->len - 1);

    RETURN();
}

/*
 * Read everything in the parameters, if anything, and assign the value to the
 * replacement value of the parameter.
 */
static void _get_reference_parms(symbol_t* sym) {
    ENTER;
    // symbol_t* sym = peek_symbol_context();
    ASSERT(sym != NULL, "attempt to get reference parms on empty symbol stack");
    PRNCH;

    if(sym->arity != 0)
        consume_space();
    else
        RETURN();

    TRACE("sym->arity = %d", sym->arity);
    if(crnt_char() == '(') {
        advance_char();
        bool finished = false;
        int index = 0;
        string_t* repl = create_string(NULL);
        while(!finished) {
            int ch = crnt_char();
            while(ch != ',' && ch != ')') {
                append_string_char(repl, ch);
                advance_char();
                ch = crnt_char();
            }
            TRACE("value: %s", repl->buffer);
            _set_parm(sym->parms, index, repl);

            if(ch == ',') {
                index++;
                advance_char();
            }
            else if(ch == ')') {
                advance_char();
                finished = true;
            }
            clear_string(repl);
        }
        TRACE("index = %d", index);
        if(index + 1 != sym->arity) {
            error("expected %d parameters to macro \"%s\" but got %d",
                  sym->arity, sym->tag->buffer, index);
        }
    }
    else if(sym->arity != 0) {
        error("expected %d parameters to macro \"%s\" but got 0",
              sym->arity, sym->tag->buffer);
    }
    #ifdef USE_TRACE
    else
        TRACE("no parms present");
    #endif
    PRNCH;
    RETURN();
}

static void _process_input(context_t* cont) {
    ENTER;

    bool finished = false;
    while(!finished) {
        int ch = crnt_char();
        switch(ch) {
            case '/':
                process_comment();
                break;
            case '.':
                process_directive();
                break;
            case '@':
                process_reference();
                break;
            case EOF:
                TRACE("end of file");
                pop_input_buffer();
                if(cont->flag)
                    pop_context();
                break;
            case EOI:
                TRACE("end of input");
                finished = true;
                break;
            default:
                // emit everything, including space
                PRNCH;
                EMITC(crnt_char());
                advance_char();
                break;
        }
    }

    RETURN();
}

/*
 * Parse and store a reference.
 */
void process_reference(void) {
    ENTER;
    recursion_count++;
    if(recursion_count >RECURSION_LIMIT)
        error("maximum recursion depth of %d is exceeded", RECURSION_LIMIT);
    TRACE("recursion count: %d", recursion_count);

    PRNCH;
    TRACE("char should be a '@'");
    expect_char('@');
    advance_char();

    string_t* name = scan_name();
    if(name != NULL) {
        context_t* cont = find_context(name);
        symbol_t* sym = find_symbol(name);
        if(sym != NULL) {
            if(sym->arity != 0) {
                _get_reference_parms(sym); // read and validate
                push_context(sym);
            }
            else if(cont == NULL)
                push_context(sym);
        }

        cont = find_context(name);
        if(cont != NULL) {
            push_input_buffer(cont->repl); // pop on EOF
            _process_input(cont);
        }
        else {
            TRACE("not a reference 1");
            EMITC('@');
            EMITS(name);
        }
    }
    else {
        TRACE("not a reference 2");
        EMITC('@');
    }

    recursion_count--;
    RETURN();
}

