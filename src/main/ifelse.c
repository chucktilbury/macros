
#include "common.h"

void process_ifelse(void) {

    ENTER;
    consume_space();
    int ch = get_char();
    if(ch == EOF) {
        error("unexpected end of file in \".if\" directive");
        RETURN();
    }
    RETURN();
}
