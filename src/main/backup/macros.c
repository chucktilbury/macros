
#include "macros.h"

// static inline bool _isin(const char* s, int c) {
//     return (strchr(s, c) != NULL)? true: false;
// }


static inline directive_type_t _directive_type(string_t* s) {

    ENTER;
    if(!comp_string(s, ".if"))
        RETURN(IF_DIRECTIVE);
    else if(!comp_string(s, ".else"))
        RETURN(ELSE_DIRECTIVE);
    else if(!comp_string(s, ".define"))
        RETURN(DEFINE_DIRECTIVE);
    else if(!comp_string(s, ".import") || !comp_string(s, ".include"))
        RETURN(IMPORT_DIRECTIVE);

    RETURN(NOT_A_DIRECTIVE);
}

void consume_space(void) {

    int ch = get_char();
    while(isspace(ch)) {
        consume_char();
        ch = get_char();
        // test_end();
    }
}

// when this is called, a '\"' has been seen but not consumed
string_t* expect_literal_string(void) {
    ENTER;

    consume_space();
    PRNCH;

    int ch = get_char();
    string_t* tmp = create_string(NULL);
    consume_char(); // do not include the ""

    int finished = false;
    while(!finished) {
        ch = get_char();
        if(ch == '\\') {
            consume_char();
            ch = get_char();
            switch(ch) {
                case '\'':
                    append_string_char(tmp, '\'');
                    consume_char();
                    break;
                case '\"':
                    append_string_char(tmp, '\"');
                    consume_char();
                    break;
                case '\\':
                    append_string_char(tmp, '\\');
                    consume_char();
                    break;
                case 'a':
                    append_string_char(tmp, '\a');
                    consume_char();
                    break;
                case 'b':
                    append_string_char(tmp, '\b');
                    consume_char();
                    break;
                case 'f':
                    append_string_char(tmp, '\f');
                    consume_char();
                    break;
                case 'n':
                    append_string_char(tmp, '\n');
                    consume_char();
                    break;
                case 'r':
                    append_string_char(tmp, '\r');
                    consume_char();
                    break;
                case 't':
                    append_string_char(tmp, '\t');
                    consume_char();
                    break;
                case 'v':
                    append_string_char(tmp, '\v');
                    consume_char();
                    break;
                case 'e':
                    append_string_char(tmp, 0x1b);
                    consume_char();
                    break;
                case 'x': {
                    // next 2 characters must be hex digits
                    char buf[4] = { '\0', '\0', '\0', '\0' };
                    consume_char(); // the 'x'
                    ch = get_char();
                    if(!isxdigit(ch))
                        consume_error("a hex digit");
                    else
                        buf[0] = ch;

                    consume_char(); // first digit
                    ch = get_char();
                    if(!isxdigit(ch))
                        consume_error("a hex digit");
                    else
                        buf[1] = ch;

                    consume_char(); // second digit
                    append_string_char(tmp, (unsigned char)strtol(buf, NULL, 16));
                }
                default:
                    append_string_char(tmp, ch);
                    consume_char();
                    break;
            }
        }
        else if(ch == '\"') {
            consume_char();
            finished = true;
        }
        else if(ch == '\n') {
            consume_char();
            error("new line not allowed in quoted strings");
        }
        else {
            consume_char();
            append_string_char(tmp, ch);
        }

        test_end();
    }

    PRNCH;
    RETURN(tmp);
}

// When this is called, a '.' has already been read and it's the
// current character.
directive_type_t expect_directive(void) {
    ENTER;
    PRNCH;

    int ch = get_char();
    string_t* tmp = create_string(NULL);
    append_string_char(tmp, ch);
    consume_char();
    ch = get_char();

    while(isalpha(ch)) {
        append_string_char(tmp, ch);
        consume_char();
        ch = get_char();
    }

    TRACE("string: \"%s\"", tmp->buffer);
    directive_type_t type = _directive_type(tmp);
    if(type == NOT_A_DIRECTIVE)
        unget_string(tmp->len);

    destroy_string(tmp);

    PRNCH;
    RETURN(type);
}

// This is called when the next thing is expected to be a name. There could
// be leading white space.
string_t* expect_name(void) {
    ENTER;
    PRNCH;

    consume_space();
    test_end();

    int ch = get_char();
    string_t* tmp = NULL;

    if(isalpha(ch) || ch == '_') {
        tmp = create_string(NULL);
        append_string_char(tmp, ch);
        consume_char();
        ch = get_char();

        while(isalnum(ch) || ch == '_') {
            append_string_char(tmp, ch);
            consume_char();
            ch = get_char();
        }
        TRACE("string: \"%s\"", tmp->buffer);
    }
    else {
        TRACE("not a name");
        // unget_char();
    }

    PRNCH;
    RETURN(tmp);
}

// when this is called, a digit has been read. Dont's care about leading 0's.
string_t* expect_number(void) {
    ENTER;
    PRNCH;

    int ch = get_char();
    string_t* tmp = create_string(NULL);
    append_string_char(tmp, ch);
    consume_char();

    while(isdigit(ch)) {
        append_string_char(tmp, ch);
        consume_char();
        ch = get_char();
    }

    TRACE("string: \"%s\"", tmp->buffer);

    PRNCH;
    RETURN(tmp);
}

/*
    When this is called, a '@' has been read. If the symbol is found and
    there is repl_text then return that. If there is no repl then return
    an empty string. Else if the symbol is not found, then unget the name
    and return NULL.
*/
string_t* expect_ref(void) {
    ENTER;
    PRNCH;

    if(get_char() != '@')
        RETURN(NULL);

    consume_char();

    string_t* name = expect_name();
    string_t* str = NULL;

    symbol_t* sym = find_symbol(name);
    if(sym != NULL) {
        if(sym->repl_text != NULL)
            str = copy_string(sym->repl_text);
        else
            str = create_string(NULL);
    }
    else
        unget_string(name->len + 1);

    destroy_string(name);

    PRNCH;
    RETURN(str);
}

// static bool process_eof(void) {
//
//     if(get_char() == EOF) {
//         if(IS_FILE) {
//             TRACE("end of file");
//             close_file();
//             consume_char();
//             return true;
//         }
//         else {
//             TRACE("end of buffer");
//             return true;
//         }
//     }
//     return false;
// }
//
// static bool process_eoi(void) {
//
//     if(get_char() == EOI) {
//         TRACE("end of input");
//         return true;
//     }
//
//     return false;
// }
//
// void process_file(void) {
//
//     ENTER;
//
//     bool finished = false;
//
//     while(!finished) {
//         if(process_comment()) {
//             TRACE("process_comment");
//         }
//         else if(process_directive()) {
//             TRACE("process_directive");
//         }
//         else if(process_reference()) {
//             TRACE("process_reference");
//         }
//         else if(process_eof()) {
//             TRACE("process_eof");
//         }
//         else if(process_eoi()) {
//             TRACE("process_eoi");
//             finished = true;
//         }
//         else {
//             PRNCH;
//             EMITC(get_char());
//             consume_char();
//         }
//     }
//
//     RETURN();
// }

void test_end_of_file(void) {

    if(get_char() == EOF)
        error("unexpected end of file");
}

void test_end_of_input(void) {

    if(get_char() == EOI)
        error("unexpected end of input");
}

void test_end(void) {

    test_end_of_file();
    test_end_of_input();
}

#if 0

    int changes = 0;

    while(true) {
        int ch = get_char();
        switch(ch) {
            case '/':
                consume_char();
                ch = get_char();
                TRACE("char here: %c", ch);
                if(ch == '/') {
                    consume_char();
                    changes += consume_single_line_comment();
                }
                else if(ch == '*') {
                    consume_char();
                    changes += consume_multi_line_comment();
                }
                else {
                    // printf("\n>>> here\n");
                    //  append_string_char(master, '/');
                    EMITC('/');
                }
                break;
            case '.':
                changes += process_directive();
                break;
            case '@':
                changes += process_reference();
                break;
            case EOF:
                if(IS_FILE) {
                    TRACE("end of file");
                    close_file();
                    consume_char();
                }
                else {
                    TRACE("end of buffer");
                    RETURN(changes);
                }
                break;
            case EOI:
                TRACE("end of input");
                RETURN(changes);
            default:
                // PRNCH;
                EMITC(ch);
                consume_char();
                break;
        }
    }
#endif
