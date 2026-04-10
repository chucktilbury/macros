
#include "common.h"
#include "expression.h"

// just copy the body to master
static void copy_body(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE(10, "char on entry: \'%c\'", ch);
    //TRACE(10, "master: cap: %d, len: %d", master->cap, master->len);
    if(ch != '{')
        error(".if/.else requires a body"); // does not return
    consume_char();

    int count = 1;
    bool finished = false;

    ch = get_char();
    while(!finished) {
        if(ch == '}') {
            count--;
            if(count == 0) {
                consume_char();
                finished = true;
                continue;
            }
            else {
                //append_string_char(master, ch);
                EMITC(ch);
                consume_char();
                ch = get_char();
            }
        }
        else if(ch == '{') {
            count++;
            //append_string_char(master, ch);
            EMITC(ch);
            consume_char();
            ch = get_char();
        }
        else if(ch == '@') {
            process_subs();
            ch = get_char();
        }
        else {
            //append_string_char(master, ch);
            EMITC(ch);
            consume_char();
            ch = get_char();
        }
        test_end();
    }
    //TRACE(10, "master: cap: %d, len: %d", master->cap, master->len);
    RETURN();
}

static void ignore_expr(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE(10, "char on entry: \'%c\'", ch);
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
    TRACE(10, "char on entry: \'%c\'", ch);

    if(ch != '{')
        error(".if/.else requires a body"); // does not return

    consume_char();

    int count = 1;
    bool finished = false;
    ch = get_char();

    while(!finished) {
        TRACE(105, "count: %d = %c", count, ch);
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
        test_end();
    }
    RETURN();
}

static void consume_else(void) {

    ENTER;

    bool finished = false;

    while(!finished) {
        consume_space();
        string_t* s = process_word();
        if(ELSE_DIRECTIVE == process_directive_type(s)) {
            consume_space();
            int ch = get_char();
            if(ch == '(') {
                ignore_expr();
                ignore_body();
            }
            else if(ch == '{') {
                ignore_body();
            }
            else
                consume_error("an expression or a if/else body");
        }
        else {
            unget_string(s->len);
            finished = true;
        }

    }

    RETURN();
}

// Consume all of the following "else" clauses if there are any.
// An else clause without an expression is "true".
static void process_else(void) {

    ENTER;

    bool finished = false;
    while(!finished) {
        string_t* s = process_word();
        if(ELSE_DIRECTIVE == process_directive_type(s)) {
            consume_space();
            int ch = get_char();
            TRACE(10, "entry char: %c (0x%02X)", ch, ch);
            if(ch == '(') {
                TRACE(10, "evaluate the expression");
                if(expression()) {
                    copy_body();
                    consume_else();
                    finished = true;
                }
                else
                    ignore_body();
            }
            else if(ch == '{') {
                TRACE(10, "copy the body");
                copy_body();
                consume_else();
                finished = true;
            }
            else
                consume_error("an expression or a if/else body"); // no return
        }
        else {
            TRACE(10, "not an else clause: %s", s->buf);
            unget_string(s->len);
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
    TRACE(10, "char on entry: \'%c\'", ch);

    if(ch != '(')
        error(".if requires an expression"); // does not return

    if(expression()) {
        TRACE(10, "first .if is true");
        copy_body();
        consume_else();
    }
    else {
        ignore_body();
        process_else();
    }

    RETURN();
}

