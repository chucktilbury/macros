
#include "macros.h"
#include "expression.h"

static void process_if_reference(void) {
    ENTER;
    process_reference();
    RETURN();
}

static void process_if_input(void) {

    // PRNCH;
    // int ch = get_char();
    // EMITC(ch);
    // consume_char();
    ENTER;
    int count = 1;
    int finished = 0;

    while(!finished) {
        if(get_char() == '{') {
            TRACE("seen '{': count = %d", count);
            count++;
            EMITC(get_char());
            consume_char();
        }
        else if(get_char() == '}') {
            count--;
            TRACE("seen '}': count = %d", count);
            if(count <= 0) {
                TRACE("finished!");
                consume_char();
                consume_space();
                finished = true;
                continue;
            }
        }
        else if(IS_COMMENT)
            process_comment();
        else if(IS_DIRECTIVE)
            process_directive();
        else if(IS_REFERENCE)
            process_if_reference();
        else if(IS_EOF)
            process_eof();
        else if(IS_EOI)
            error("unexpected end of file in if/else");
        else {
            //PRNCH;
            EMITC(get_char());
            consume_char();
        }
    }
    process_eoi();

    RETURN();

}

// just copy the body to master
static void copy_body(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE("char on entry: \'%c\'", ch);

    if(ch != '{')
        error(".if/.else requires a body"); // does not return
    consume_char();

    int count = 1;
    bool finished = false;

    while(!finished) {
        ch = get_char();
        if(ch == '}') {
            count--;
            TRACE("seen '}'");
            if(count == 0) {
                TRACE("finished!");
                consume_char();
                consume_space();
                finished = true;
                continue;
            }
            else {
                EMITC(ch);
                consume_char();
                ch = get_char();
            }
        }
        else if(ch == '{') {
            TRACE("seen '{'");
            count++;
            EMITC(ch);
            consume_char();
            ch = get_char();
        }
        else {
            process_if_input(); // indirect recursive call to process funcitons
            finished = true;
        }
        test_end();
    }

    RETURN();
}

static void ignore_expr(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE("char on entry: \'%c\'", ch);
    if(ch != '(')
        RETURN(); // expression is optional
    consume_char();

    int count = 1;
    bool finished = false;
    ch = get_char();

    while(!finished) {
        if(ch == ')') {
            count--;
            if(count == 0) {
                consume_char();
                finished = true;
                continue;
            }
            else {
                consume_char();
                ch = get_char();
            }
        }
        else if(ch == '(') {
            count++;
            consume_char();
            ch = get_char();
        }
        else {
            consume_char();
            ch = get_char();
        }

        test_end();
    }
    RETURN();
}

static void ignore_body(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE("char on entry: \'%c\'", ch);

    if(ch != '{')
        error(".if/.else requires a body"); // does not return

    consume_char();

    int count = 1;
    bool finished = false;
    ch = get_char();

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

        consume_char();
        ch = get_char();
        // test_end();
    }
    RETURN();
}

static void ignore_else(void) {

    ENTER;

    consume_space();
    // test_end();

    while(ELSE_DIRECTIVE == expect_directive()) {
        consume_space();
        // test_end();
        int ch = get_char();
        TRACE("char: %c", ch);
        if(ch == '(') {
            ignore_expr();
            ignore_body();
        }
        else if(ch == '{') {
            ignore_body();
        }
        consume_space();
        // test_end();
    }

    RETURN();
}

// Consume all of the following "else" clauses if there are any.
// An else clause without an expression is "true".
static void process_else(void) {

    ENTER;

    bool finished = false;
    while(!finished) {
        consume_space();
        if(ELSE_DIRECTIVE == expect_directive()) {
            consume_space();
            int ch = get_char();
            TRACE("entry char: %c (0x%02X)", ch, ch);
            if(ch == '(') {
                TRACE("evaluate the expression");
                if(expression()) {
                    copy_body();
                    ignore_else();
                    finished = true;
                }
                else
                    ignore_body();
            }
            else if(ch == '{') {
                TRACE("copy the body");
                copy_body();
                ignore_else();
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
    test_end();

    // should be a '('
    int ch = get_char();
    TRACE("char on entry: \'%c\'", ch);

    if(ch != '(')
        error(".if requires an expression"); // does not return

    if(expression()) {
        TRACE("first .if is true");
        copy_body();
        ignore_else();
    }
    else {
        ignore_body();
        process_else();
    }

    RETURN();
}
