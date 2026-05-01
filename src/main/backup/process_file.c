
#include "common.h"
#include "input_buffer.h"
#include "output_buffer.h"
#include "root_context.h"

static void consume_sl_comment(void) {
    ENTER;
    int ch;

    while(true) {
        advance_char();
        ch = crnt_char();
        if(ch == EOL) {
            advance_char();
            break;
        }
        else if(ch == EOF) {
            advance_char();
            warning("unexpected end of file in comment");
            break;
        }
    }
    RETURN();
}

static void consume_ml_comment(void) {
    ENTER;
    int ch;
    bool finished = false;

    while(!finished) {
        ch = crnt_char();
        if(ch == '*') {
            advance_char();
            ch = crnt_char();
            if(ch == '/') {
                advance_char();
                break;
            }
            else if(ch == EOF) {
                warning("unexpected end of file in comment");
                finished = true;
            }
        }
        else if(ch == EOF) {
            warning("unexpected end of file in comment");
            finished = true;
        }
        advance_char();
    }
    RETURN();
}

static void consume_comment(void) {
    ENTER;

    advance_char();
    int ch = crnt_char();
    TRACE("char here: %c", ch);
    if(ch == '/') {
        advance_char();
        consume_sl_comment();
        RETURN();
    }
    else if(ch == '*') {
        advance_char();
        consume_ml_comment();
        RETURN();
    }
    else {
        // printf("\n>>> here\n");
        EMITC('/');
        RETURN();
    }

    RETURN();
}

/*
 * Process the entry point to a directive from the root context.
 */
static void process_directive(void) {

}

/*
 * Process a reference in the root context.
 */
static void process_reference(void) {

}

/*
 * Scan the input for one of the objects that is acceptable outside of a
 * directive. If the character does not introduce an object then put it in the
 * output buffer.
 */
void process_file(void) {
    ENTER;

    bool finished = false;

    while(!finished) {
        int ch = crnt_char();
        switch(ch) {
            case '/':
                TRACE("comment");
                consume_comment();
                break;
            case '.':
                TRACE("directive");
                process_directive();
                break;
            case '@':
                // a reference outside of a directive.
                TRACE("reference");
                process_reference();
                break;
            case EOF:
                TRACE("end of file");
                pop_input_buffer();
                break;
            case EOI:
                TRACE("end of input");
                finished = true;
                break;
            default:
                EMITC(ch);
                advance_char();
                break;
        }
    }

    RETURN();
}

