
#include "macros.h"

bool process_directive(void) {

    ENTER;
    PRNCH;
    // bool result = false;
    if(get_char() != '.')
        RETURN(false);

    switch(expect_directive()) {
        case IF_DIRECTIVE:
            process_ifelse();
            RETURN(true);
            break;
        case ELSE_DIRECTIVE:
            error("misplaced else clause");
            break;
        case DEFINE_DIRECTIVE:
            process_define();
            RETURN(true);
            break;
        case IMPORT_DIRECTIVE:
            process_import();
            RETURN(true);
            break;
        case NOT_A_DIRECTIVE:
        default:
            EMITC('.');
            consume_char();
            break;
    }

    PRNCH;
    RETURN(false);
}

// the "/*" has already been read
int consume_multi_line_comment(void) {

    ENTER;
    int ch;
    int count = 0;
    bool finished = false;
    TRACEF;

    while(!finished) {
        ch = get_char();
        if(ch == '*') {
            consume_char();
            count++;
            ch = get_char();
            if(ch == '/') {
                count++;
                consume_char();
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
        consume_char();
        count++;
    }
    TRACE("count = %d", count);
    RETURN(count);
}

// the "//" has already been read
int consume_single_line_comment(void) {

    ENTER;
    int ch;
    int count = 0;
    TRACEF;

    while(true) {
        consume_char();
        count++;
        ch = get_char();
        if(ch == EOL) {
            consume_char();
            count++;
            break;
        }
        else if(ch == EOF) {
            consume_char();
            count++;
            warning("unexpected end of file in comment");
            break;
        }
    }
    TRACE("count = %d", count);
    RETURN(count);
}

bool process_comment(void) {

    ENTER;

    if(get_char() != '/')
        RETURN(false);
    else {
        consume_char();
        int ch = get_char();
        TRACE("char here: %c", ch);
        if(ch == '/') {
            consume_char();
            consume_single_line_comment();
            RETURN(true);
        }
        else if(ch == '*') {
            consume_char();
            consume_multi_line_comment();
            RETURN(true);
        }
        else {
            // printf("\n>>> here\n");
            EMITC('/');
            RETURN(false);
        }
    }

    RETURN(false);
}

bool process_eoi(void) {

    ENTER;
    if(get_char() == EOI) {
        TRACE("end of input");
        RETURN(true);
    }

    RETURN(false);
}

bool process_eof(void) {

    ENTER;
    if(get_char() == EOF) {
        if(IS_FILE) {
            TRACE("end of file");
            close_file();
            consume_char();
            RETURN(true);
        }
        else {
            TRACE("end of buffer");
            RETURN(true);
        }
    }

    RETURN(false);
}

/*
 * Scan a name from the input string. When this is entered the first character
 * must be a '@'
 */
string_t* scan_name(const char* input) {
    ENTER;

    bool finished = false;
    int index = 0;
    const char* input_buffer = input;
    string_t* name = create_string(NULL);

    while(!finished) {
        int ch = input_buffer[index];
        if(ch == '@') {
            index++;
            ch = input_buffer[index];
            // get the name
            if(isalpha(ch) || ch == '_') {
                append_string_char(name, ch);
                index++;
                ch = input_buffer[index];

                while(isalnum(ch) || ch == '_') {
                    append_string_char(name, ch);
                    index++;
                    ch = input_buffer[index];
                }
            }
            else {
                // not a name
                TRACE("not a name: \"%s\"", name->buffer);
                EMITC('@');
                EMITS(name);
                clear_string(name);
                index++;
                finished = true;
            }
            TRACE("name: \"%s\"", name->buffer);
        }
        else {
            TRACE("not a name");
            EMITC('@');
            finished = true;
        }
    }
    RETURN(name);
}

/*
 * Process a reference in an if block where a '@' has been seen and the name
 * has been determined. If it is not defined then simply emit it to the output
 * buffer. The input stream is drawn from the input file.
 */
void process_if_reference(void) {

    char_buffer_t* buf = get_input_buffer();
    string_t* name = scan_name(&buf->buffer[buf->index]);

    // find the global symbol

    // do the substitution
}

/*
 * Process a reference in a define block where there may be parameters. The
 * input stream is drawn from the symbol that was found.
 */
void process_def_reference(symbol_t* sym) {

}

/*
 * Process a if block between {}. Handle all directives and global replacements.
 */
void process_if_block(void) {
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
        else if(IS_EOI) {
            process_eoi();
            error("unexpected end of input");
        }
        else {
            //PRNCH;
            EMITC(get_char());
            consume_char();
        }
    }
    RETURN();
}

/*
 * Process a if block between {}. Handle all directives, local replacements,
 * and global replacements.
 */
void process_def_block(symbol_t* sym) {
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
            process_def_reference(sym);
        else if(IS_EOF)
            process_eof();
        else if(IS_EOI) {
            process_eoi();
            error("unexpected end of input");
        }
        else {
            //PRNCH;
            EMITC(get_char());
            consume_char();
        }
    }
    RETURN();
}
