#include "common.h"
#include "symbols.h"
#include "process.h"
#include "parms.h"
#include <unistd.h>
// #include "context.h"
#include "reference.h"

#define RECURSION_LIMIT 20
static int _recursion_count = 0;

typedef struct _context_t {
    bool flag;
    string_t* tag;
    string_t* repl;
    struct _context_t* next;
} context_t;
static context_t* _stack = NULL;

static context_t* _create_context(string_t* tag, string_t* repl, bool flag) {

    context_t* cont = _ALLOC_TYPE(context_t);
    cont->tag = copy_string(tag);
    cont->repl = copy_string(repl);
    cont->flag = flag;
    // push_output_buffer(repl);

    if(_stack != NULL)
        cont->next = _stack;
    _stack = cont;

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
static void _push_context(symbol_t* sym) {
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
 * Pop the local context after the repl text has been expended for the symbol.
 */
static void _pop_context(void) {
    ENTER;
    if(_stack != NULL) {
        context_t* ptr;
        context_t* next;
        for(ptr = _stack; ptr != NULL; ptr = next) {
            next = ptr->next;
            TRACE("pop var: %s: %s", ptr->tag->buffer, ptr->flag ? "TRUE" : "FALSE");
            _stack = next;
            // pop_output_buffer();
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
static context_t* _find_context(string_t* tag) {
    ENTER;
    TRACE("find: %s", tag->buffer);
    if(_stack != NULL) {
        context_t* ptr;
        for(ptr = _stack; ptr != NULL; ptr = ptr->next) {
            if(!comp_string_str(ptr->tag, tag)) {
                TRACE("FOUND, tag = %s", ptr->flag ? "TRUE" : "FALSE");
                RETURN(ptr);
            }
        }
    }
    TRACE("NOT FOUND");
    RETURN(NULL);
}

static void _process_input(void) {
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
                finished = true;
                break;
            case EOI:
                TRACE("end of input");
                finished = true;
                break;
            default:
                // emit everything, including space
                // PRNCH;
                EMITC(crnt_char());
                advance_char();
                break;
        }
    }

    RETURN();
}

// static string_t* _expand_parm(string_t* parm) {
static void _expand_parm(string_t* parm) {
    ENTER;
    string_t* str = copy_string(parm);
    strip_string_ends(str);

    char* tmp = strchr(str->buffer, '@');
    string_t* ostr = create_string(NULL);
    if(tmp != NULL) {
        push_input_buffer(create_string(tmp));
        push_output_buffer(ostr);
        process_reference();
        _process_input();
        pop_output_buffer();
        TRACE("emit: %s", ostr->buffer);
        EMITS(ostr);
    }
    else
        append_string_str(ostr, str);
    destroy_string(str);
    destroy_string(ostr);
    RETURN();
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
            // append_string_str(lst->lst[index]->repl, _expand_parm(repl));
            _expand_parm(repl);
        }
        else {
            // lst->lst[index]->repl = _expand_parm(repl);
            _expand_parm(repl);
            _pop_context();
        }
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
            while(ch != ',' && ch != ')' && ch != EOF) {
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

/*
 * Parse and store a reference.
 */
void process_reference(void) {
    ENTER;
    _recursion_count++;
    if(_recursion_count > RECURSION_LIMIT)
        error("maximum recursion depth of %d is exceeded", RECURSION_LIMIT);
    TRACE("recursion count: %d", _recursion_count);

    PRNCH;
    TRACE("char should be a '@'");
    expect_char('@');
    advance_char();

    string_t* name = scan_name();
    if(name != NULL) {
        context_t* cont = _find_context(name);
        symbol_t* sym = find_symbol(name);
        if(sym != NULL) {
            if(sym->arity != 0) {
                _get_reference_parms(sym); // read and validate
                _push_context(sym);
            }
            else if(cont == NULL)
                _push_context(sym);
        }

        cont = _find_context(name);
        if(cont != NULL) {
            TRACE("context repl = %s", cont->repl->buffer);
            push_input_buffer(cont->repl); // pop on EOF
            _process_input();
            // if(cont->flag)
            _pop_context();
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

    _recursion_count--;
    RETURN();
}
