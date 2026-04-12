
#include "common.h"

int verbosity = 0;
int level = 0;

void init_trace(void) {

    verbosity = atoi(raw_string(get_cmd_opt("verbosity")));
}

