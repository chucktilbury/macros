
#include "common.h"
#include "expression.h"

// just copy the body to master
static void copy_body(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE(DEFAULT_TRACE, "char on entry: \'%c\'", ch);
    // TRACE(DEFAULT_TRACE, "master: cap: %d, len: %d", master->cap, master->len);
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
                // append_string_char(master, ch);
                EMITC(ch);
                consume_char();
                ch = get_char();
            }
        }
        else if(ch == '{') {
            count++;
            // append_string_char(master, ch);
            EMITC(ch);
            consume_char();
            ch = get_char();
        }
        else if(ch == '@') {
            process_subs();
            ch = get_char();
        }
        else {
            // append_string_char(master, ch);
            EMITC(ch);
            consume_char();
            ch = get_char();
        }
        test_end();
    }
    // TRACE(DEFAULT_TRACE, "master: cap: %d, len: %d", master->cap, master->len);
    RETURN();
}

static void ignore_expr(void) {

    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE(DEFAULT_TRACE, "char on entry: \'%c\'", ch);
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
    TRACE(DEFAULT_TRACE, "char on entry: \'%c\'", ch);

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
static int process_else(void) {

    ENTER;

    int changes = 0;
    bool finished = false;
    while(!finished) {
        string_t* s = process_word();
        if(ELSE_DIRECTIVE == process_directive_type(s)) {
            consume_space();
            int ch = get_char();
            TRACE(DEFAULT_TRACE, "entry char: %c (0x%02X)", ch, ch);
            if(ch == '(') {
                TRACE(DEFAULT_TRACE, "evaluate the expression");
                if(expression()) {
                    copy_body();
                    consume_else();
                    finished = true;
                    changes++;
                }
                else
                    ignore_body();
            }
            else if(ch == '{') {
                TRACE(DEFAULT_TRACE, "copy the body");
                copy_body();
                consume_else();
                finished = true;
                changes++;
            }
            else
                consume_error("an expression or a if/else body"); // no return
        }
        else {
            TRACE(DEFAULT_TRACE, "not an else clause: %s", s->buf);
            unget_string(s->len);
            finished = true;
        }
    }

    RETURN(changes);
}

int process_ifelse(void) {

    ENTER;
    int changes = 0;
    consume_space();
    test_end();

    // should be a '('
    int ch = get_char();
    TRACE(DEFAULT_TRACE, "char on entry: \'%c\'", ch);

    if(ch != '(')
        error(".if requires an expression"); // does not return

    if(expression()) {
        TRACE(DEFAULT_TRACE, "first .if is true");
        copy_body();
        consume_else();
        changes++;
    }
    else {
        ignore_body();
        changes += process_else();
    }

    RETURN(changes);
}
