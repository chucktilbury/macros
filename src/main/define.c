/*
 * Define a symbol. A symbol can have replacement text or not. No checking or
 * replacement is done on the replacement text when the define is parsed. It
 * is copied verbatim.
 *
 * The
 */

#include "common.h"
#include "define.h"
#include "symbols.h"
#include "misc.h"
#include "parms.h"

static void _define_body(symbol_t* sym) {

    ENTER;

    // should be a '{'
    TRACE("char on entry: \'%c\'", crnt_char());
    advance_char();

    int ch;
    int count = 1;
    string_t* tmp = create_string(NULL);
    while(true) {
        test_end_error();
        ch = crnt_char();
        if(ch == '}') {
            count--;
            if(count == 0) {
                if(tmp->len > 0)
                    sym->repl_text = tmp;
                advance_char();
                RETURN();
            }
        }
        else if(ch == '{') {
            count++;
        }
        // else if(crnt_char() == '@')
        //     process_define_reference(tmp);

        append_string_char(tmp, crnt_char());
        advance_char();
    }

    RETURN(); // never happens
}

/*
 * High level processor for define directive. The word ".define" has already
 * been consumed.
 */
void process_define(void) {
    ENTER;
    PRNCH;

    consume_space();
    test_end_error();

    // get the symbol name
    string_t* name = scan_name();
    if(name != NULL) {
        symbol_t* sym = insert_symbol(name);

        // get the parameter list, which is optional
        consume_space();
        if(crnt_char() == '(') {
            get_param_names(sym);

            // the body is not optional.
            consume_space();
            if(crnt_char() == '{')
                _define_body(sym);
            else
                error("a definition body is required if there are macro parameters");
        }
        else {
            // the body is optional.
            if(crnt_char() == '{')
                _define_body(sym);
        }
    }
    else
        error("expected name for .define");

    PRNCH;
    RETURN();
}
