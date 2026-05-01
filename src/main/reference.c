#include "common.h"
#include "symbols.h"
#include "misc.h"
#include "parms.h"

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

/*
 * Process a reference in a local context. This is where a macro is being
 * expanded that has parameters.
 */
void process_local_reference(void) {
    ENTER;
    RETURN();
}

/*
 * Process a reference in a global context. This is where a macro has no
 * parameters.
 */
void process_global_reference(void) {
    ENTER;
    RETURN();
}
