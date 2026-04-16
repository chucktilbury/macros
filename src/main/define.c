/*
 * Define a symbol. A symbol can have replacement text or not. No checking or
 * replacement is done on the replacement text when the define is parsed. It
 * is copied verbatim.
 *
 * The
 */

#include "common.h"


// void process_define_parameters(symbol_t* sym) {

//     ENTER;

//     // should be a '('
//     TRACE("char on entry: \'%c\'", get_char());
//     consume_char();
//     consume_space();
//     test_end();

//     bool finished = false;
//     while(!finished) {
//         consume_space();
//         test_end();
//         int ch = get_char();
//         if(ch == ',')
//             consume_char();
//         else if(ch == ')') {
//             consume_char();
//             finished = true;
//         }
//         else {
//             string_t* name = expect_name();
//             if(name == NULL)
//                 consume_error("a name");
//             else {
//                 symbol_t* s = create_symbol(name);
//                 if(sym->parms == NULL)
//                     sym->parms = s;
//                 else
//                     _insert_symbol(sym->parms, s);
//                 sym->arity++;
//             }
//         }
//     }

//     RETURN();
// }

void process_define_body(symbol_t* sym) {

    ENTER;

    // should be a '{'
    TRACE("char on entry: \'%c\'", get_char());
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
                RETURN();
            }
        }
        else if(ch == '{')
            count++;

        append_string_char(tmp, ch);
        consume_char();
    }

    RETURN(); // never happens
}

int process_define(void) {

    ENTER;
    consume_space();
    test_end();

    // get the symbol name
    string_t* name = expect_name();
    symbol_t* sym = insert_symbol(name);

    // get the parameter list, which is optional
    // consume_space();
    // if(get_char() == '(')
    //     process_define_parameters(sym);

    // the body is optional.
    consume_space();
    if(get_char() == '{')
        process_define_body(sym);

    int ch = get_char();
    TRACE("char after body: '%c' (0x%02X)", !isspace(ch) ? ch : ' ', ch);

    RETURN(1);
}
