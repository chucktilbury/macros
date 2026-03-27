
#include "common.h"

void process_import(void) {

    ENTER;

    // should be a '\"'
    int ch = expect_char("\"");
    consume_char();

    // get the quoted string
    string_t* tmp = create_string(NULL);
    while(true) {
        test_end();
        ch = get_char();
        if(ch == '\"')
            break;
        else
            append_string_char(tmp, ch);
        consume_char();
    }
    TRACE(0, "name to open: %s", tmp->buf);

    open_file(tmp);

    RETURN();
}
