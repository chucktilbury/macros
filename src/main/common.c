
#include "common.h"

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

// When this is called, a '.' has already been read and it's the
// current character.
directive_type_t expect_directive(void) {
    ENTER;

    int ch = get_char();
    TRACE("entry character: %c (0x%02X)", ch, ch);
    string_t* tmp = create_string(NULL);
    append_string_char(tmp, ch);
    consume_char();
    ch = get_char();

    while(isalpha(ch)) {
        append_string_char(tmp, ch);
        consume_char();
        ch = get_char();
    }

    TRACE("string: \"%s\"", tmp->buf);
    directive_type_t type = _directive_type(tmp);
    if(type == NOT_A_DIRECTIVE)
        unget_string(tmp->len);

    destroy_string(tmp);

    RETURN(type);
}

// This is called when the next thing is expected to be a name. There could
// be leading white space.
string_t* expect_name(void) {
    ENTER;

    consume_space();
    test_end();

    int ch = get_char();
    TRACE("entry character: %c (0x%02X)", ch, ch);
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
        TRACE("string: \"%s\"", tmp->buf);
    }

    RETURN(tmp);
}

// when this is called, a digit has been read. Dont's care about leading 0's.
string_t* expect_number(void) {
    ENTER;

    int ch = get_char();
    TRACE("entry character: %c (0x%02X)", ch, ch);
    string_t* tmp = create_string(NULL);
    append_string_char(tmp, ch);
    consume_char();

    while(isdigit(ch)) {
        append_string_char(tmp, ch);
        consume_char();
        ch = get_char();
    }

    TRACE("string: \"%s\"", tmp->buf);

    RETURN(tmp);
}

// When this is called, a '@' has been read. If the symbol is found and there
// is repl_text then return that. If there is no repl then return an empty
// string. Else if the symbol is not found, then unget the name and return
// NULL.
string_t* expect_ref(void) {
    ENTER;

    int ch = get_char();
    TRACE("entry character: %c (0x%02X)", ch, ch);
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
        unget_string(name->len+1);

    destroy_string(name);
    RETURN(str);
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
    RETURN(1);
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
    RETURN(1);
}

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
