
#include "common.h"

int process_import(void) {

    ENTER;

    string_t* tmp = expect_literal_string();
    TRACE("name to open: %s", tmp->buf);

    open_file(tmp);

    destroy_string(tmp);

    RETURN(1);
}
