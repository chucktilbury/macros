/*
 * Define a symbol. A symbol can have replacement text or not. No checking or
 * replacement is done on the replacement text when the define is parsed. It
 * is copied verbatim.
 *
 * The
 */

#include "common.h"


void process_define_parameters(symbol_t* sym) {

    ENTER;

    // should be a '('
    TRACE(0, "char on entry: \'%c\'", get_char());
    consume_char();
    consume_space();

    int ch = get_char();
    if(isalpha(ch) || ch == '_') {
        // get the first parameter
        string_t* name = process_name();
        sym->parms = create_symbol(name);
        consume_space();

        // see if there is another one
        ch = get_char();
        if(ch == ')') {
            consume_char();
            RETURN();
        }
        else if(ch == ',') {
            consume_char();
            name = process_name();
            insert_symbol(sym->parms, create_symbol(name));

            while(true) {
                test_end();
                consume_space();
                ch = get_char();
                if(ch == ')') {
                    consume_char();
                    RETURN();
                }
                else if(ch == ',') {
                    consume_char();
                    consume_space();
                    name = process_name();
                    insert_symbol(sym->parms, create_symbol(name));
                }
                else {
                    error("expected a ')' or a ',' but got '%c' (0x%02X)", ch, ch);
                    RETURN();
                }
            }
        }
    }
    else if(ch == ')') {
        consume_char();
        RETURN();
    }
    else {
        error("expected a parameter list but got: %c (0x%02X)", ch, ch);
        RETURN();
    }

    RETURN();
}

void process_define_body(symbol_t* sym) {

    ENTER;

    // should be a '{'
    TRACE(0, "char on entry: \'%c\'", get_char());
    consume_char();

    int ch;
    int count = 1;
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
                break;
            }
        }
        else if(ch == '{')
            count++;

        // fputc(ch, stderr);
        append_string_char(tmp, ch);
        consume_char();
    }

    RETURN(); // never happens
}

void process_define(void) {

    ENTER;
    consume_space();
    CHECK_EOF_ERROR("in \".define\" directive");

    // get the symbol name
    string_t* name = process_name();
    symbol_t* sym = create_symbol(name);

    // get the parameter list, which is optional
    consume_space();
    if(get_char() == '(')
        process_define_parameters(sym);

    // the body is optional.
    consume_space();
    if(get_char() == '{')
        process_define_body(sym);

    if(sym_table != NULL)
        insert_symbol(sym_table, sym);
    else
        sym_table = sym;

    RETURN();
}
