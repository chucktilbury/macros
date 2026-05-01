#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdbool.h>

void process_input(void);
bool process_directive(void);
int consume_multi_line_comment(void);
int consume_single_line_comment(void);
bool process_comment(void);
bool process_eoi(void);
bool process_eof(void);

#endif
