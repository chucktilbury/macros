
#include "common.h"

void consume_space(void) {

    int ch = get_char();
    while(isspace(ch)) {
        consume_char();
        ch = get_char();
        // test_end();
    }
}

directive_type_t process_directive_type(string_t* s) {

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

// a name = [a-zA-Z_][a-zA-Z0-9_]*
string_t* process_name(void) {

    ENTER;

    consume_space();
    // test_end();

    int ch = get_char();
    string_t* name = create_string(NULL);
    ch = get_char();
    if(isalpha(ch) || ch == '_') {
        append_string_char(name, ch);
        consume_char();
        ch = get_char();
    }
    else {
        destroy_string(name);
        consume_error("a name");
    }

    while((isalnum(ch) || ch == '_') && ch != EOF) {
        append_string_char(name, ch);
        consume_char();
        ch = get_char();
    }

    TRACE(DEFAULT_TRACE, "name: \"%s\"", name->buf);
    RETURN(name);
}

// a word = [^ \t\n\r\f{}()]+
string_t* process_word(void) {

    ENTER;
    consume_space();
    // test_end();

    int ch = get_char();
    string_t* word = create_string(NULL);
    ch = get_char();

    if(!isspace(ch) && ch != '{' && ch != '}' && ch != '(' && ch != ')') {
        append_string_char(word, ch);
        consume_char();
        ch = get_char();
    }
    else {
        destroy_string(word);
        consume_error("a word");
    }

    while(!isspace(ch) && ch != '{' && ch != '}' && ch != '(' && ch != ')' && ch != EOF) {
        append_string_char(word, ch);
        consume_char();
        ch = get_char();
    }

    TRACE(DEFAULT_TRACE, "word: \"%s\"", word->buf);
    RETURN(word);
}

// a number = [0-9]*
string_t* process_number(void) {

    ENTER;
    consume_space();
    // test_end();

    int ch = get_char();
    string_t* number = create_string(NULL);
    ch = get_char();

    if(ch >= '0' && ch <= '9') {
        append_string_char(number, ch);
        consume_char();
        ch = get_char();
    }
    else {
        destroy_string(number);
        consume_error("a number");
    }

    while(ch >= '0' && ch <= '9' && ch != EOF) {
        append_string_char(number, ch);
        consume_char();
        ch = get_char();
    }

    TRACE(DEFAULT_TRACE, "number: \"%s\"", number->buf);
    RETURN(number);
}

int process_subs(void) {

    ENTER;

    TRACE(DEFAULT_TRACE, "consume the '@'");
    consume_char();

    string_t* tmp = process_word();
    symbol_t* sym = find_symbol(sym_table, tmp);
    if(sym != NULL) {
        EMITS(sym->repl_text);
        RETURN(1);
    }
    else {
        EMITC('@');
        EMITS(tmp);
        RETURN(0);
    }

    RETURN(0);
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
