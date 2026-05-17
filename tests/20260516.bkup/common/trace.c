
#include "common.h"

#ifdef USE_TRACE
int verbosity = 0;
int level = 0;

void init_trace(void) {

    verbosity = atoi(raw_string(get_cmd_opt("verbosity")));
}

static inline void _pad(int num) {

    for(int i = 0; i < num; i++)
        putc('-', stdout);
}

void print_legend(const char* str) {

    if(str != NULL) {
        int len = strlen(str);
        _pad((80-(len+2))/2);
        printf(" %s ", str);
        _pad((80-(len+2))/2 + ((!(len & 0x01))? 0: 1));
        putc('\n', stdout);
    }
    else {
        _pad(80);
        putc('\n', stdout);
    }
}

#endif
