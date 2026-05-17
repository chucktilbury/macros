#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "string_buffer.h"

typedef enum {
    TOK_EOF,
    TOK_EOB,
    TOK_EOI,
    TOK_DEFINE,
    TOK_IF,
    TOK_ELSE,
    TOK_INCLUDE,
    TOK_ERROR,
    TOK_MESSAGE,
    TOK_QSTR,
    TOK_NAME,
    TOK_NUMBER,
    TOK_RAW_TEXT,
    TOK_BLOCK,
} token_type_t;

typedef struct token_t {
    token_type_t type;
    string_t* str;
    string_t* fname;
    int line;
    int col;
} token_t;

token_t* crnt_token(void);
token_t* consume_token(void);

#endif /* _SCANNER_H_ */
