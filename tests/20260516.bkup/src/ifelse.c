
#include "common.h"
#include "expression.h"
#include "process.h"
#include "reference.h"

// static void _if_reference(void) {
//     ENTER;
//     process_reference();
//     RETURN();
// }

static void _if_input(void) {

    // PRNCH;
    // int ch = get_char();
    // EMITC(ch);
    // consume_char();
    ENTER;
    int count = 1;
    int finished = 0;

    while(!finished) {
        if(crnt_char() == '{') {
            TRACE("seen '{': count = %d", count);
            count++;
            EMITC(crnt_char());
            advance_char();
        }
        else if(crnt_char() == '}') {
            count--;
            TRACE("seen '}': count = %d", count);
            if(count <= 0) {
                TRACE("finished!");
                advance_char();
                finished = true;
                //continue;
            }
        }
        else if(crnt_char() == '/')
            process_comment();
        else if(crnt_char() == '.')
            process_directive();
        // else if(IS_REFERENCE)
        //     process_if_reference();
        else if(crnt_char() == EOF)
            error("unexpected end of file in if/else");
        else if(crnt_char() == EOI)
            error("unexpected end of input in if/else");
        else {
            PRNCH;
            EMITC(crnt_char());
            advance_char();
        }
    }

    RETURN();
}

// just copy the body to master
static void _copy_body(void) {

    ENTER;

    consume_space();
    test_end_error();

    int ch = crnt_char();
    TRACE("char on entry: \'%c\'", ch);

    if(ch != '{')
        error(".if/.else requires a body"); // does not return
    advance_char();

    int count = 1;
    bool finished = false;

    while(!finished) {
        ch = crnt_char();
        if(ch == '}') {
            count--;
            TRACE("seen '}'");
            if(count == 0) {
                TRACE("finished!");
                advance_char();
                finished = true;
            }
            else {
                EMITC(ch);
                advance_char();
                ch = crnt_char();
            }
        }
        else if(ch == '{') {
            TRACE("seen '{'");
            count++;
            EMITC(ch);
            advance_char();
            ch = crnt_char();
        }
        else {
            _if_input(); // indirect recursive call to process funcitons
            finished = true;
        }
        //test_end_error();
    }

    RETURN();
}

static void _ignore_expr(void) {

    ENTER;

    consume_space();
    test_end_error();

    int ch = crnt_char();
    TRACE("char on entry: \'%c\'", ch);
    if(ch != '(')
        RETURN(); // expression is optional
    advance_char();

    int count = 1;
    bool finished = false;
    ch = crnt_char();

    while(!finished) {
        if(ch == ')') {
            count--;
            if(count == 0) {
                advance_char();
                finished = true;
                continue;
            }
            else {
                advance_char();
                ch = crnt_char();
            }
        }
        else if(ch == '(') {
            count++;
            advance_char();
            ch = crnt_char();
        }
        else {
            advance_char();
            ch = crnt_char();
        }

        test_end_error();
    }
    RETURN();
}

static void _ignore_body(void) {

    ENTER;

    consume_space();
    test_end_error();

    int ch = crnt_char();
    TRACE("char on entry: \'%c\'", ch);

    if(ch != '{')
        error(".if/.else requires a body"); // does not return

    advance_char();

    int count = 1;
    bool finished = false;
    ch = crnt_char();

    while(!finished) {
        TRACEX(100, "count: %d = %c", count, ch);
        if(ch == '}') {
            count--;
            if(count == 0)
                finished = true;
        }
        else if(ch == '{') {
            count++;
        }

        advance_char();
        ch = crnt_char();
    }
    RETURN();
}

static directive_type_t _expect_directive(void) {
    ENTER;
    directive_type_t retv = NOT_A_DIRECTIVE;

    emit_space();
    test_end_error();

    if(crnt_char() != '.')
        RETURN(NOT_A_DIRECTIVE);
    advance_char();

    string_t* name = scan_name();
    if(name != NULL) {
        retv = directive_type(name);
        destroy_string(name);
    }
    else {
        TRACE("not a directive");
        EMITC('.');
    }

    RETURN(retv);
}

static void _ignore_else(void) {
    ENTER;

    emit_space();

    while(ELSE_DIRECTIVE == _expect_directive()) {
        consume_space();
        int ch = crnt_char();
        TRACE("char: %c", ch);
        if(ch == '(') {
            _ignore_expr();
            _ignore_body();
        }
        else if(ch == '{') {
            _ignore_body();
        }
    }

    RETURN();
}

// Consume all of the following "else" clauses if there are any.
// An else clause without an expression is "true".
static void _process_else(void) {

    ENTER;

    bool finished = false;
    while(!finished) {
        consume_space();
        if(ELSE_DIRECTIVE == _expect_directive()) {
            consume_space();
            int ch = crnt_char();
            TRACE("entry char: %c (0x%02X)", ch, ch);
            if(ch == '(') {
                TRACE("evaluate the expression");
                if(expression()) {
                    _copy_body();
                    _ignore_else();
                    finished = true;
                }
                else
                    _ignore_body();
            }
            else if(ch == '{') {
                TRACE("copy the body");
                _copy_body();
                _ignore_else();
                finished = true;
            }
            else
                consume_error("an expression or a if/else body"); // no return
        }
        else {
            finished = true;
        }
    }

    RETURN();
}

void process_ifelse(void) {

    ENTER;
    consume_space();
    test_end_error();

    // should be a '('
    int ch = crnt_char();
    TRACE("char on entry: \'%c\'", ch);

    if(ch != '(')
        error(".if requires an expression"); // does not return

    if(expression()) {
        TRACE("first .if is true");
        _copy_body();
        _ignore_else();
    }
    else {
        _ignore_body();
        _process_else();
    }

    RETURN();
}
