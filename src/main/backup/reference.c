#include "common.h"
#include "symbols.h"
#include "misc.h"
#include "parms.h"

#if 0
/*
 * This function is called recursively to process all occourences of any global
 * reference. The string returned is intended to be emitted directly to the
 * ouput.
 */
static string_t* _process_local_repl(string_t* str) {

    return str;
}

/*
 * Assumes that the name of the reference is already scanned and that if the
 * symbol does not exist, the caller will handle putting it back into the input
 * stream.
 *
 * If the symbol does not exist, then return NULL. If the symbol exists, but it
 * has no replacement value then return an empty string. Otherwise, return the
 * replacement value.
 */
static string_t* _get_global_reference_value(string_t* name) {
    ENTER;
    PRNCH;

    string_t* str = NULL;

    symbol_t* sym = find_symbol(name);
    if(sym != NULL) {
        if(sym->repl_text != NULL)
            str = copy_string(sym->repl_text);
        else
            str = create_string(NULL); // symbol has no value
    }

    PRNCH;
    RETURN(str);
}

/*
 * Return the value of the local reference parameter string. These are set when
 * the reference is encountered.
 *
 * If the symbol exists, then the string returned has the value, which could be
 * blank. If the name is not defined then return NULL
 */
static string_t* _get_local_reference_value(symbol_t* sym, string_t* name) {
    ENTER;

    string_t* str = get_parm(sym->parms, name);

    RETURN(str);
}
#endif

/*
 * Process a reference in a local context. This is where a macro is being
 * expanded that has parameters.
 */
void process_reference(void) {
    ENTER;
    PRNCH;
    advance_char(); // the leading '@'
    string_t* name = scan_name();
    TRACE("name: %s", name->buffer);
    if(name != NULL) {
        symbol_t* sym = find_symbol(name);
        if(sym != NULL) {
            TRACE("symbol name: %s", sym->tag->buffer);
            TRACE("arity: %d", sym->arity);

            push_symbol_context(sym);
            get_reference_parms();
            string_t* parm = get_parm(sym->parms, name);
            if(parm != NULL) {
                TRACE("hhere1");
                push_input_buffer(parm);
                process_file();
                pop_input_buffer();
            }
            else if(sym->repl_text != NULL) {
                TRACE("hhere2");
                push_input_buffer(sym->repl_text);
                process_file();
                pop_input_buffer();
            }
            pop_symbol_context();
        }
        else {
            TRACE("not a reference 1");
            EMITC('@');
            EMITS(name);
        }

        destroy_string(name);
    }
    else {
        TRACE("not a reference 2");
        EMITC('@');
    }

    RETURN();
}

#if 0
if(sym->arity == 0) {
    // TODO: check for invalid parameters first
    if(sym->repl_text != NULL) {
        push_input_buffer(sym->repl_text);
        process_file();
        pop_input_buffer();
        }
        // else don't emit anything'
        }
        else {
            TRACE("with %d parameters", sym->arity);
            TRACE("symbol name: %s", sym->tag->buffer);
            get_reference_parms();
            string_t* parm = get_parm(sym->parms, name);
            if(parm != NULL) {
                TRACE("with parms");
                push_input_buffer(parm);
                process_file();
                pop_input_buffer();
                }
                else if(sym->repl_text != NULL) {
                    TRACE("with symbol");
                    push_input_buffer(sym->repl_text);
                    process_file();
                    pop_input_buffer();
                    }
                    else
                        process_file();
                    }


symbol_t* sym = find_symbol(name);
if(sym != NULL) {
    TRACE("arity = %d", sym->arity);
    if(sym->arity == 0) {
        // TODO: check for invalid parameters first
        if(sym->repl_text != NULL) {
            push_input_buffer(sym->repl_text);
            process_file();
            }
            }
            else {
                TRACE("with parameters");

                process_file();
                // read valid parameters
                // recurse the system
                }
                }
                else {
                    TRACE("not a reference 1");
                    EMITC('@');
                    EMITS(name);
                    }
#endif
