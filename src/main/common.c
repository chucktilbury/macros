
#include "common.h"

void consume_space(void) {

    int ch = get_char();
    while(isspace(ch)) {
        test_end();
        ch = consume_char();
    }
}

string_t* process_name(void) {

    ENTER;

    consume_space();

    int ch = get_char();
    string_t* name = create_string(NULL);
    ch = get_char();
    if(isalpha(ch) || ch == '_') {
        append_string_char(name, ch);
        ch = consume_char();
    }
    else {
        destroy_string(name);
        consume_error("a name");
    }

    do {
        append_string_char(name, ch);
        ch = consume_char();
    } while((isalnum(ch) || ch == '_') && ch != EOF);

    TRACE(0, "name: \"%s\"", name->buf);
    RETURN(name);
}
