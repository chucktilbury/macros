
#include "common.h"
#include "scanner.h"

static token_t* _crnt_token = NULL;
static token_t _eoi_token = { TOK_EOI, NULL, NULL, -1, -1 };

/*
 * Return the same token over and over until it is consumed.
 */
token_t* crnt_token(void) {

    if(_crnt_token != NULL)
        return _crnt_token;
    else
        return &_eoi_token;
}

/*
 * Scan another token. This consumes text until it finds a directive and then it
 * changes mode to support the directive. Different directives have different
 * modes, depending on what their structure is. So, esentially, this is a parser
 * as well as a scanner. The caller will expect to see certain tokens based
 * upon what is being parsed at the time.
 */
token_t* consume_token(void) {
}
