
#include "common.h"
#include "misc.h"
//#include "symbols.h"
#include "define.h"
#include "ifelse.h"

/*
 * Discard spaces.
 */
void consume_space(void) {

    int ch = crnt_char();
    while(isspace(ch)) {
        advance_char();
        ch = crnt_char();
    }
}

void test_end_of_file(void) {

    if(crnt_char() == EOF)
        error("unexpected end of file");
}

void test_end_of_input(void) {

    if(crnt_char() == EOI)
        error("unexpected end of input");
}

/*
 * Create an error if the current character is EOF or EOI.
 */
void test_end_error(void) {

    test_end_of_file();
    test_end_of_input();
}

/*
 * Converts a single character into a string.
 */
const char* prnch(int ch) {

    static char buffer[8];

    memset(buffer, 0, sizeof(buffer));
    if(isprint(ch))
        buffer[0] = ch;
    else
        sprintf(buffer, "0x%02X", ch);

    return buffer;
}

/*
 * Scan a name into a string_t*. If a name cannot be scanned, then return NULL.
 */
string_t* scan_name(void) {

    ENTER;
    PRNCH;

    int ch = crnt_char();
    string_t* tmp = NULL;

    if(isalpha(ch) || ch == '_') {
        tmp = create_string(NULL);
        append_string_char(tmp, ch);
        advance_char();
        ch = crnt_char();

        while(isalnum(ch) || ch == '_') {
            append_string_char(tmp, ch);
            advance_char();
            ch = crnt_char();
        }
        TRACE("string: \"%s\"", tmp->buffer);
    }

    PRNCH;
    RETURN(tmp);
}

string_t* scan_literal_string(void) {
    ENTER;

    consume_space();
    PRNCH;

    int ch = crnt_char();
    string_t* tmp = create_string(NULL);
    advance_char(); // do not include the '\"'

    int finished = false;
    while(!finished) {
        ch = crnt_char();
        if(ch == '\\') {
            advance_char();
            ch = crnt_char();
            switch(ch) {
                case '\'':
                    append_string_char(tmp, '\'');
                    advance_char();
                    break;
                case '\"':
                    append_string_char(tmp, '\"');
                    advance_char();
                    break;
                case '\\':
                    append_string_char(tmp, '\\');
                    advance_char();
                    break;
                case 'a':
                    append_string_char(tmp, '\a');
                    advance_char();
                    break;
                case 'b':
                    append_string_char(tmp, '\b');
                    advance_char();
                    break;
                case 'f':
                    append_string_char(tmp, '\f');
                    advance_char();
                    break;
                case 'n':
                    append_string_char(tmp, '\n');
                    advance_char();
                    break;
                case 'r':
                    append_string_char(tmp, '\r');
                    advance_char();
                    break;
                case 't':
                    append_string_char(tmp, '\t');
                    advance_char();
                    break;
                case 'v':
                    append_string_char(tmp, '\v');
                    advance_char();
                    break;
                case 'e':
                    append_string_char(tmp, 0x1b);
                    advance_char();
                    break;
                case 'x': {
                    // next 2 characters must be hex digits
                    char buf[4] = { '\0', '\0', '\0', '\0' };
                    advance_char(); // the 'x'
                    ch = crnt_char();
                    if(!isxdigit(ch))
                        consume_error("a hex digit");
                    else
                        buf[0] = ch;

                    advance_char(); // first digit
                    ch = crnt_char();
                    if(!isxdigit(ch))
                        consume_error("a hex digit");
                    else
                        buf[1] = ch;

                    advance_char(); // second digit
                    append_string_char(tmp, (unsigned char)strtol(buf, NULL, 16));
                }
                default:
                    append_string_char(tmp, ch);
                    advance_char();
                    break;
            }
        }
        else if(ch == '\"') {
            advance_char();
            finished = true;
        }
        else if(ch == '\n') {
            advance_char();
            error("new line not allowed in quoted strings");
        }
        else {
            advance_char();
            append_string_char(tmp, ch);
        }

        test_end_error();
    }

    PRNCH;
    RETURN(tmp);
}

directive_type_t directive_type(string_t* str) {
    ENTER;

    directive_type_t retv = NOT_A_DIRECTIVE;

    if(str != NULL) {
        string_t* tmp = upcase_string(str);
        if(!comp_string(tmp, "DEFINE")) {
            TRACE("DEFINE_DIRECTIVE");
            retv = DEFINE_DIRECTIVE;
        }
        else if(!comp_string(tmp, "IF")) {
            TRACE("IF_DIRECTIVE");
            retv = IF_DIRECTIVE;
        }
        else if(!comp_string(tmp, "IMPORT") || !comp_string(tmp, "INCLUDE")) {
            TRACE("IMPORT_DIRECTIVE");
            retv = IMPORT_DIRECTIVE;
        }
        else if(!comp_string(tmp, "ELSE")) {
            TRACE("ELSE_DIRECTIVE");
            retv = ELSE_DIRECTIVE;
        }
        else if(!comp_string(tmp, "ERROR")){
            TRACE("ERROR_DIRECTIVE");
            retv = ERROR_DIRECTIVE;
        }
        else if(!comp_string(tmp, "MESSAGE")){
            TRACE("MESSAGE_DIRECTIVE");
            retv = MESSAGE_DIRECTIVE;
        }
#ifdef USE_TRACE
        else TRACE("NOT_A_DIRECTIVE");
#endif
        destroy_string(tmp);
    }

    RETURN(retv);
}

void process_import(void) {

    ENTER;

    string_t* tmp = scan_literal_string();

    if(tmp != NULL) {
        TRACE("name to open: %s", tmp->buffer);
        load_input_buffer(tmp);
        destroy_string(tmp);
    }
    else
        consume_error("a quoted string for .import directive");

    RETURN();
}

void process_error(void) {

    ENTER;

    string_t* tmp = scan_literal_string();

    if(tmp != NULL) {
        TRACE("error: %s", tmp->buffer);
        const char* fname = get_file_name()->buffer;
        if(fname != NULL)
            fprintf(stderr, "error: %s:%d:%d: %s", fname, get_line_no(), get_col_no(), tmp->buffer);
        else
            fprintf(stderr, "error: %s", tmp->buffer);
        destroy_string(tmp);
    }
    else
        consume_error("a quoted string for .error directive");
    exit(1);
    RETURN();
}

void process_message(void) {

    ENTER;

    string_t* tmp = scan_literal_string();

    if(tmp != NULL) {
        TRACE("message: %s", tmp->buffer);
        const char* fname = get_file_name()->buffer;
        if(fname != NULL)
            fprintf(stderr, "message: %s:%d:%d: %s", fname, get_line_no(), get_col_no(), tmp->buffer);
        else
            fprintf(stderr, "message: %s", tmp->buffer);
        destroy_string(tmp);
    }
    else
        consume_error("a quoted string for .message directive");

    RETURN();
}

static void _consume_sl_comment(void) {
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

static void _consume_ml_comment(void) {
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

void process_comment(void) {
    ENTER;

    advance_char();
    int ch = crnt_char();
    TRACE("char here: %c", ch);
    if(ch == '/') {
        advance_char();
        _consume_sl_comment();
        RETURN();
    }
    else if(ch == '*') {
        advance_char();
        _consume_ml_comment();
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
void process_directive(void) {
    ENTER;

    advance_char(); // the leading '.'
    string_t* name = scan_name();
    if(name != NULL) {
        TRACE("name: %s", name->buffer);
        switch(directive_type(name)) {
            case IF_DIRECTIVE:
                process_ifelse();
                break;
            case ELSE_DIRECTIVE:
                error("misplaced .else directive");
                break;
            case DEFINE_DIRECTIVE:
                process_define();
                break;
            case IMPORT_DIRECTIVE:
                process_import();
                break;
            case ERROR_DIRECTIVE:
                process_error();
                break;
            case MESSAGE_DIRECTIVE:
                process_message();
                break;
            case NOT_A_DIRECTIVE:
                TRACE("not a directive 1");
                EMITC('.');
                EMITS(name);
                break;
        }
        destroy_string(name);
    }
    else {
        TRACE("not a directive 2");
        EMITC('.');
    }

    RETURN();
}

